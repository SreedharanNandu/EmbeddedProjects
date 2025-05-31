/*----------------------------------------------------------------------------
 * Name:   EEPROM.c
 *----------------------------------------------------------------------------*/
#include "LPC11xx.h"                        /* LPC11xx definitions */
#include "i2c.h"
#include "EEPROM.h"
#include "System.h"


unsigned int nvm_tmr;


/*****************************************************************************
** Function name:  Write_I2C1
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
unsigned char Write_I2C_EEPROM(unsigned char chip_address,
                               unsigned int address ,
                               unsigned char *ptr,
                               unsigned char len)
{
   unsigned long lock;
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

 
   Start_I2C();

   //-- Set ADDRESS
   Send_I2C(chip_address & 0xFE);
   lock=0;
   while(LPC_I2C->STAT != 0x18)
   {
     if(lock == WHILE_TIMEOUT)
     {
       lock = 0;
       /*break;*/
     }
     lock++;
   };
   


   //-- Set High Byte
   Send_I2C(hi_byte);
   lock=0;while(LPC_I2C->STAT != 0x28){};
   
   #ifndef EEPROM_24C02
   //-- Set Low Byte
   Send_I2C(low_byte);
   lock=0;while(LPC_I2C->STAT != 0x28){};
   #endif   

   while(len)
   {
      Send_I2C(*ptr);
      lock=0;while(LPC_I2C->STAT != 0x28){};
      len--;
      *ptr++;
   }
 
   Stop_I2C();



   return 1;
}

/*****************************************************************************
** Function name:  Read_I2C
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
unsigned char Read_I2C_EEPROM( unsigned char chip_address,
                               unsigned int address ,
                               unsigned char *ptr,
                               unsigned char len)
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


   
   Start_I2C();

   //-- Set ADDRESS
   Send_I2C(chip_address & 0xFE);
   if(LPC_I2C->STAT != 0x18)
   {
      Stop_I2C();
      return 0;
   }

   //-- Set High Byte
   Send_I2C(hi_byte);
   if(LPC_I2C->STAT != 0x28)
   {
      Stop_I2C();
      return 0;
   }

   #ifndef EEPROM_24C02
   //-- Set Low Byte
   Send_I2C(low_byte);
   if(LPC_I2C->STAT != 0x28)
   {
      Stop_I2C();
      return 0;
   }
   #endif

   Start_I2C();
   Send_I2C(chip_address | 0x01);
   if(LPC_I2C->STAT != 0x40)
   {
      Stop_I2C();
      return 0;
   }

   if(len > 1)
   {
      LPC_I2C->CONCLR = I2C_FLAG_SI;
      LPC_I2C->CONSET |= I2C_FLAG_AA;
      for(;;)
      {
         while(!(LPC_I2C->CONSET & I2C_FLAG_SI)){};  //-- End Data from slave;
         *ptr++ = (unsigned char)LPC_I2C->DAT;
         len--;
         if(!len)
         {
            break;
         }
         else if(len == 1)
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
   else if(len == 1)
   {
      LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI)){};  //-- End Data from slave;
      *ptr = (unsigned char) LPC_I2C->DAT;
   }
 
   Stop_I2C();
   return 1;
}

/*****************************************************************************
** Function name:  Read_EEPROM
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
void Read_EEPROM(unsigned long address,
                 unsigned char *ptr,
                 unsigned int len)
{
   unsigned char chip_address;
   /*
       0x00000 - 0x1FFF      0 - 256 32bytes of 256pages
   */
   chip_address = CHIP_ADDRESS;

   //commenting out due to eeprom non implemented 
   Read_I2C_EEPROM(chip_address,(unsigned int)address,ptr,len);

}
/*****************************************************************************
** Function name:  Write_EEPROM
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
unsigned char Write_EEPROM(unsigned long address,
                           unsigned char *ptr,
                           unsigned int len)
{
   uint8_t current_addr = address;
   uint8_t remaining_bytes = len;
   uint8_t bytes_to_write = 0;
   uint8_t status = 1;
    
   while(remaining_bytes > 0) 
   {
      // Calculate the number of bytes to write (max 8 bytes per page)
      bytes_to_write = (remaining_bytes > 8) ? 8 : remaining_bytes;
        
      // Write a page (up to 8 bytes)
      Write_I2C_EEPROM(CHIP_ADDRESS, current_addr, ptr, bytes_to_write);
      Delay_Ms(50u);
        
      // Update the memory address and remaining bytes
      current_addr += bytes_to_write;
      ptr += bytes_to_write;
      remaining_bytes -= bytes_to_write;
   }
   return status;


}
/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      2-10-2025
 *---------------------------------------------------------------------------*/


