/*----------------------------------------------------------------------------
 * Name:   App_I2C_Intf.c
 *----------------------------------------------------------------------------*/
#include "LPC11xx.h"
#include "App_i2c_Intf.h"
#include "i2c.h"
#include "App_Si.h"





/*****************************************************************************
*                             Function Definition                             
******************************************************************************/

/*****************************************************************************
** Function name :  
** Descriptions  :  
** parameters    :   None
** Returned value:  
*****************************************************************************/
unsigned char Write_Si_I2C(unsigned char *wptr,
                           unsigned char wlen)
{
   unsigned long lock=0u;
   unsigned char chip_address = SI_SLAVE_ADDRESS;

   __disable_irq();
   Start_I2C();
   //-- Set ADDRESS
   Send_I2C(chip_address & 0xFEu);
   if(LPC_I2C->STAT != 0x18u)//ack check
   {
      Stop_I2C();
      return 0u;
   }
   while(wlen)
   {
      Send_I2C(*wptr);
      lock=0;
      while(LPC_I2C->STAT != 0x28u)
      {
         if(lock == I2C_WHILE_TIMEOUT)
         {
            lock = 0u;
            Stop_I2C();
            return 0u;
         }
         lock++;
      };
      if(wlen > 0u)
      {
         wlen--;
      }
      (void)*wptr++;
   }
   
   Stop_I2C();
   __enable_irq();
   return 1;
}

/*****************************************************************************
** Function name :  
** Descriptions  :  
** parameters    :   None
** Returned value:  
*****************************************************************************/
unsigned char Read_Si_I2C(unsigned char *rptr,
                          unsigned char rlen)
{
   volatile unsigned char chip_address = SI_SLAVE_ADDRESS;

   __disable_irq();
   Start_I2C();
   Send_I2C(chip_address | 0x01u);
   if(LPC_I2C->STAT != 0x40u)
   {
      Stop_I2C();
      return 0u;
   }
   if(rlen > 1u)
   {
      LPC_I2C->CONCLR = I2C_FLAG_SI;
      LPC_I2C->CONSET |= I2C_FLAG_AA;
      for(;;)
      {
         while(!(LPC_I2C->CONSET & I2C_FLAG_SI));
         *rptr++ = (unsigned char)LPC_I2C->DAT;
         rlen--;
         if(!rlen)
         {
           break;
         }
         else if(rlen == 1u)
         {
          
            LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
         }
         else
         {
            LPC_I2C->CONCLR = I2C_FLAG_SI;
            LPC_I2C->CONSET |= I2C_FLAG_AA;
         }
      }
   }
   if(rlen == 1u)
   {
      LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));  //-- End Data from slave;
      *rptr = (unsigned char) LPC_I2C->DAT;
   }
   Stop_I2C();
   __enable_irq();
   return 1u;
}



