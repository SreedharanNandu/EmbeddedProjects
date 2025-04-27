#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "type.h"
#include "i2c.h"


/* 
From device to device, the I2C communication protocol may vary, 
in the example below, the protocol uses repeated start to read data from or 
write to the device:
For master read: the sequence is: STA,Addr(W),offset,RE-STA,Addr(r),data...STO 
for master write: the sequence is: STA,Addr(W),offset,RE-STA,Addr(w),data...STO
Thus, in state 8, the address is always WRITE. in state 10, the address could 
be READ or WRITE depending on the I2C command.
*/   


/*****************************************************************************
** Function name:      I2CStart
**
** Descriptions:      Create I2C start condition, a timeout
**            value is set if the I2C never gets started,
**            and timed out. It's a fatal error. 
**
** parameters:         None
** Returned value:      true or false, return false if timed out
** 
*****************************************************************************/
void I2CStart(void)
{
   LPC_I2C->CONCLR = 0xFF; // Clear all bits
   LPC_I2C->CONSET |= (I2CONSET_I2EN | I2CONSET_STA);
   while(!(LPC_I2C->CONSET & I2CONSET_SI));      //--- End START
   while(!(LPC_I2C->STAT & 0x08));      
}

/*****************************************************************************
** Function name:      I2CStop
**
** Descriptions:      Set the I2C stop condition, if the routine
**            never exit, it's a fatal bus error.
**
** parameters:         None
** Returned value:      true or never return
** 
*****************************************************************************/
void I2CStop( void )
{
  LPC_I2C->CONSET = I2CONSET_STO;      /* Set Stop flag */ 
  LPC_I2C->CONCLR = I2CONCLR_SIC;  /* Clear SI flag */ 
            
  /*--- Wait for STOP detected ---*/
  while( LPC_I2C->CONSET & I2CONSET_STO );
}
/*****************************************************************************
** Function name:   Send_I2C
**
** Descriptions:   The routine to complete a I2C transaction
**         from start to stop. All the intermitten
**         steps are handled in the interrupt handler.
**         Before this routine is called, the read
**         length, write length and I2C master buffer
**         need to be filled.
**
** parameters:      None
** Returned value:   Any of the I2CSTATE_... values. See i2c.h
** 
*****************************************************************************/
void Send_I2C( unsigned char data ) 
{
   LPC_I2C->DAT = data;
   LPC_I2C->CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;                //-- Clear SI
   while(!(LPC_I2C->CONSET & I2CONSET_SI));      //-- End wr POINT
}

/*****************************************************************************
** Function name:      I2CInit
**
** Descriptions:      Initialize I2C controller
**
** parameters:         I2c mode is either MASTER or SLAVE
** Returned value:      true or false, return false if the I2C
**            interrupt handler was not installed correctly
** 
*****************************************************************************/
void I2CInit( void ) 
{
  /* It seems to be bit0 is for I2C, different from
  UM. To be retested along with SSP reset. SSP and I2C
  reset are overlapped, a known bug, for now, both SSP 
  and I2C use bit 0 for reset enable. Once the problem
  is fixed, change to "#if 1". */
#if 1
  LPC_SYSCON->PRESETCTRL |= (0x1<<1);
#else
  LPC_SYSCON->PRESETCTRL |= (0x1<<0);
#endif
  LPC_SYSCON->SYSAHBCLKCTRL |= (1<<5);
  LPC_IOCON->PIO0_4 &= ~0x3F;   /*  I2C I/O config */
  LPC_IOCON->PIO0_4 |= 0x01;      /* I2C SCL */
  LPC_IOCON->PIO0_5 &= ~0x3F;   
  LPC_IOCON->PIO0_5 |= 0x01;      /* I2C SDA */

  /*--- Clear flags ---*/
  LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_SIC | I2CONCLR_STAC | I2CONCLR_I2ENC;    

  /*--- Reset registers ---*/
#if FAST_MODE_PLUS
  LPC_IOCON->PIO0_4 |= (0x1<<9);
  LPC_IOCON->PIO0_5 |= (0x1<<9);
  LPC_I2C->SCLL   = I2SCLL_HS_SCLL;
  LPC_I2C->SCLH   = I2SCLH_HS_SCLH;
#else
  LPC_I2C->SCLL   = I2SCLL_SCLL;
  LPC_I2C->SCLH   = I2SCLH_SCLH;
#endif


  /* Disable the I2C Interrupt */
  NVIC_DisableIRQ(I2C_IRQn);

  LPC_I2C->CONSET = I2CONSET_I2EN;
  
}


/******************************************************************************
**                            End Of File
******************************************************************************/

