/******************** (C) COPYRIGHT 2010 Embest Info&Tech Co.,LTD. ************
* File Name          : EEPROM_main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 01/18/2010
* Description        : EEPROM_main program body.
*******************************************************************************
*******************************************************************************
* History:
* 01/18/2010       : V1.0         initial version
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "type.h"
#include "uart.h"
#include "i2c.h"
#include "system.h"

state_t ee_state;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void Process_Eeprom(void)
{
   switch(ee_state)
   {
    case INIT:
             //EE_Read(0x00,(unsigned char*)&K_Radio_Data_Read,42);
             ee_state = IDLE;
             break;
    case UPDATE:
             //EE_Write(0x00,(unsigned char*)&K_Radio_Data_Write,42);
             ee_state = IDLE;
             break;
    case IDLE:
             break;
    default:
             break;

   }
}



void EE_Read(unsigned int address,
             unsigned char *data,
             unsigned char length)
{
   #if EEPROM_24C02
      unsigned char hi_byte;
   #else
      unsigned char hi_byte;
      unsigned char low_byte;
   #endif

   #if EEPROM_24C02
   hi_byte = (unsigned char)(address & 0xFF);
   
   #else
   hi_byte = (unsigned char)(address >> 8);
   low_byte = (unsigned char)(address & 0xFF);
   #endif


   
   I2CStart();

   //-- Set ADDRESS
   Send_I2C(0xA0);
   if(LPC_I2C->STAT != 0x18)
   {
      I2CStop();
      return;
   }

   //-- Set High Byte
   Send_I2C(hi_byte);
   if(LPC_I2C->STAT != 0x28)
   {
      I2CStop();
      return;
   }

   #ifndef EEPROM_24C02
   //-- Set Low Byte
   Send_I2C(low_byte);
   if(LPC_I2C->STAT != 0x28)
   {
      I2CStop();
      return ;
   }
   #endif


   I2CStart();
   Send_I2C(0xA1);
   if(LPC_I2C->STAT != 0x40)
   {
      I2CStop();
      return;
   }

   if(length > 1)
   {
      LPC_I2C->CONCLR = I2C_FLAG_SI;
      LPC_I2C->CONSET |= I2C_FLAG_AA;
      for(;;)
      {
         while(!(LPC_I2C->CONSET & I2C_FLAG_SI));  //-- End Data from slave;
         *data++ = (unsigned char)LPC_I2C->DAT;
         length--;
         if(!length)
         {
            break;
         }
         else if(length == 1)
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
   else if(length == 1)
   {
      LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));  //-- End Data from slave;
      *data = (unsigned char) LPC_I2C->DAT;
   }
 
   I2CStop();
}

void EE_Write(unsigned int address,
              unsigned char *data,
              unsigned char length)
{
   #if EEPROM_24C02
      unsigned char hi_byte;
   #else
      unsigned char hi_byte;
      unsigned char low_byte;
   #endif
   unsigned long i;

   #if EEPROM_24C02
   hi_byte = (unsigned char)(address & 0xFF);
   
   #else
   hi_byte = (unsigned char)(address >> 8);
   low_byte = (unsigned char)(address & 0xFF);
   #endif

 
   I2CStart();

   //-- Set ADDRESS
   Send_I2C(0xA0);
   while(LPC_I2C->STAT != 0x18);
   


   //-- Set High Byte
   Send_I2C(hi_byte);
   while(LPC_I2C->STAT != 0x28)   ;
   
   #ifndef EEPROM_24C02
   //-- Set Low Byte
   Send_I2C(low_byte);
   while(LPC_I2C->STAT != 0x28);
   #endif   

   while(length)
   {
      Send_I2C(*data);
      while(LPC_I2C->STAT != 0x28);
      length--;
      *data++;
   }
 
   I2CStop();

   for(i=0;i < 900000; i++) //-- actually wr = ~110 but timeout =10000
   {
      LPC_I2C->CONSET = I2C_FLAG_STA | I2C_FLAG_I2EN;
      LPC_I2C->CONCLR = I2C_FLAG_SI;  //-- Here - clear only SI (not all LPC_I2C1->I2CONCLR)
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));        //wait the ACK

      LPC_I2C->DAT = 0xA0; // R/WI = 0
      LPC_I2C->CONCLR = I2C_FLAG_SI | I2C_FLAG_STA; //-- Clear START & SI
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));//wait the ACK
      if(LPC_I2C->STAT == 0x18) //-- got ACK after CLA + W
      {
         break;
      }
   }

   I2CStop();
}

/************* (C) COPYRIGHT 2010 Wuhan R&D Center, Embest *****END OF FILE****/
