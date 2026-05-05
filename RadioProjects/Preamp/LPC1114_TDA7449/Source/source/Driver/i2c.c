/******************************************************************************
* File Name          : i2c.c
*******************************************************************************/
#include "LPC11xx.h"
#include "i2c.h"



//===========================================================================
//================ Common routines ==========================================
//===========================================================================
void Init_I2C(void)
{
   LPC_SYSCON->PRESETCTRL |= (0x1<<1);
   LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
   LPC_IOCON->PIO0_4 &= ~0x3F;/*  I2C I/O config */
   LPC_IOCON->PIO0_4 |= 0x01;/* I2C SCL */
   LPC_IOCON->PIO0_5 &= ~0x3F;
   LPC_IOCON->PIO0_5 |= 0x01;/* I2C SDA */
   LPC_I2C->SCLL = 240;//48Mhz/(240+240) = 100khz
   LPC_I2C->SCLH = 240;//NOTE:increasing speed affects RDS scanning freq due to I2C transaction timeout delay
   LPC_I2C->CONCLR = 0xFF;           //-- Clear all flags
   LPC_I2C->CONSET = 0x40;           //-- Set Master Mode
   LPC_I2C->CONSET = I2C_FLAG_I2EN; //--- Enable I2C

}
/*****************************************************************************
** Function name:    I2CStart
**
** Descriptions:    Create I2C start condition, a timeout
**        value is set if the I2C never gets started,
**        and timed out. It's a fatal error. 
**
** parameters:      None
** Returned value:    true or false, return false if timed out
** 
*****************************************************************************/
void Start_I2C( void )
{
   /*--- Issue a start condition ---*/
   LPC_I2C->CONCLR = 0xFF; // Clear all bits
   LPC_I2C->CONSET |= (I2CONSET_I2EN | I2CONSET_STA);
   while(!(LPC_I2C->CONSET & I2CONSET_SI)){};      //--- End START
   while(!(LPC_I2C->STAT & 0x08)){};      
    
}

/*****************************************************************************
** Function name:    I2CStop
**
** Descriptions:    Set the I2C stop condition, if the routine
**        never exit, it's a fatal bus error.
**
** parameters:      None
** Returned value:    true or never return
** 
*****************************************************************************/
void Stop_I2C( void )
{
  LPC_I2C->CONCLR = I2CONCLR_SIC;  /* Clear SI flag */ 
  LPC_I2C->CONSET |= (I2CONSET_AA | I2CONSET_STO);      /* Set Stop flag */ 
  /*--- Wait for STOP detected ---*/
  while( LPC_I2C->CONSET & I2CONSET_STO ){};
}

/*****************************************************************************
** Function name:    I2CEngine
**
** Descriptions:    The routine to complete a I2C transaction
**        from start to stop. All the intermitten
**        steps are handled in the interrupt handler.
**        Before this routine is called, the read
**        length, write length, I2C master buffer,
**        and I2C command fields need to be filled.
**        see i2cmst.c for more details. 
**
** parameters:      None
** Returned value:    true or false, return false only if the
**        start condition can never be generated and
**        timed out. 
** 
*****************************************************************************/
void Send_I2C( unsigned char data ) 
{
   unsigned long lock=0;
   LPC_I2C->DAT = data;
   LPC_I2C->CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;                //-- Clear SI
   lock = 0;
   while(!(LPC_I2C->CONSET & I2CONSET_SI))
   {
      if(lock == I2C_WHILE_TIMEOUT)
      {
         lock = 0;
         Stop_I2C();
         break;
      }
      lock++;
   };//-- End wr POINT
}



