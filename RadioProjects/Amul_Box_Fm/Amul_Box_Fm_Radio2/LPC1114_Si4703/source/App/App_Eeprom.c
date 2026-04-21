#include <stdio.h>
#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "i2c.h"
#include "App_Eeprom.h"
#include "App_Si.h"
#include "timer16.h"
#include <string.h>


// CRC16 Polynomial (CRC-16-CCITT)
#define CRC16_POLYNOMIAL           0x8005u
#define CRC16_INITIAL_VALUE        0xFFFFu


uint8_t ee_state=INIT_EE;
volatile unsigned char tempRead1[MAX_EE_SIZE];
volatile unsigned char tempRead2[MAX_EE_SIZE];
volatile unsigned char tempWrite[MAX_EE_SIZE];
volatile uint16_t calcCrc;
volatile unsigned char crc1Good ,crc2Good;
volatile unsigned short int storedCrc1,storedCrc2;
volatile unsigned short int calcCrc1,calcCrc2;




/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Process_Eeprom(void)
{
   switch(ee_state)
   {
    case INIT_EE:
             EE_Read(PAGE_EE(0u),(unsigned char*)&tempRead1[0],MAX_EE_SIZE);//copy1
             EE_Read(PAGE_EE(1u),(unsigned char*)&tempRead2[0],MAX_EE_SIZE);//copy2
             Validate_EE_Read_Data();//check if not corrupted
             Change_Mode_EE(IDLE_EE);
             break;
    case UPDATE_EE:
             Validate_EE_Write_Data();//check if not corrupted
             EE_Write(PAGE_EE(0u),(unsigned char*)&tempWrite[0],MAX_EE_SIZE);//copy1
             Delay_Ms(100u);//write delay time
             EE_Write(PAGE_EE(1u),(unsigned char*)&tempWrite[0],MAX_EE_SIZE);//copy2
             Delay_Ms(100u);//write delay time
             Change_Mode_EE(IDLE_EE);
             break;
    case IDLE_EE:
    default:
             break;

   }
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Change_Mode_EE(unsigned char state)
{
   ee_state = state;
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void EE_Read(unsigned short int address,
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
   hi_byte = (unsigned char)(address & 0xFFu);
   
   #else
   hi_byte = (unsigned char)(address >> 8u);
   low_byte = (unsigned char)(address & 0xFFu);
   #endif

   Start_I2C();

   //-- Set ADDRESS
   Send_I2C(0xA0u);
   if(LPC_I2C->STAT != 0x18u)
   {
      Stop_I2C();
      return;
   }

   //-- Set High Byte
   Send_I2C(hi_byte);
   if(LPC_I2C->STAT != 0x28u)
   {
      Stop_I2C();
      return;
   }

   #ifndef EEPROM_24C02
   //-- Set Low Byte
   Send_I2C(low_byte);
   if(LPC_I2C->STAT != 0x28u)
   {
      Stop_I2C();
      return ;
   }
   #endif


   Start_I2C();
   Send_I2C(0xA1);
   if(LPC_I2C->STAT != 0x40u)
   {
      Stop_I2C();
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
         else if(length == 1u)
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
   else if(length == 1u)
   {
      LPC_I2C->CONCLR = I2C_FLAG_AA | I2C_FLAG_SI;  //-- After next will NO ASK
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));  //-- End Data from slave;
      *data = (unsigned char) LPC_I2C->DAT;
   }
 
   Stop_I2C();
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void EE_Write(unsigned short int address,
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
   hi_byte = (unsigned char)(address & 0xFFu);
   
   #else
   hi_byte = (unsigned char)(address >> 8u);
   low_byte = (unsigned char)(address & 0xFFu);
   #endif

 
   Start_I2C();

   //-- Set ADDRESS
   Send_I2C(0xA0u);
   while(LPC_I2C->STAT != 0x18u);
   
   //-- Set High Byte
   Send_I2C(hi_byte);
   while(LPC_I2C->STAT != 0x28u)   ;
   
   #ifndef EEPROM_24C02
   //-- Set Low Byte
   Send_I2C(low_byte);
   while(LPC_I2C->STAT != 0x28u);
   #endif   

   while(length)
   {
      Send_I2C(*data);
      while(LPC_I2C->STAT != 0x28u);
      length--;
      (void)*data++;
   }
 
   Stop_I2C();

   for(i=0u;i < 900000u; i++) //-- actually wr = ~110 but timeout =10000
   {
      LPC_I2C->CONSET = I2C_FLAG_STA | I2C_FLAG_I2EN;
      LPC_I2C->CONCLR = I2C_FLAG_SI;  //-- Here - clear only SI (not all LPC_I2C1->I2CONCLR)
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));        //wait the ACK

      LPC_I2C->DAT = 0xA0u; // R/WI = 0
      LPC_I2C->CONCLR = I2C_FLAG_SI | I2C_FLAG_STA; //-- Clear START & SI
      while(!(LPC_I2C->CONSET & I2C_FLAG_SI));//wait the ACK
      if(LPC_I2C->STAT == 0x18u) //-- got ACK after CLA + W
      {
         break;
      }
   }
   Stop_I2C();
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Validate_EE_Read_Data(void)
{
	
   //[0.....19],[20,21],[22.23]
   calcCrc1 = CRC16_CCITT((uint8_t*)&tempRead1[0],(MAX_CH_SIZE+MAX_INDEX_SIZE+MAX_VOL_SIZE));
   storedCrc1 = (tempRead1[MAX_EE_SIZE-1u]<<8u) | tempRead1[MAX_EE_SIZE-2u];
   
   //[24.....43],[44,45],[46.47]
   calcCrc2 = CRC16_CCITT((uint8_t*)&tempRead2[0],(MAX_CH_SIZE+MAX_INDEX_SIZE+MAX_VOL_SIZE));
   storedCrc2 = (tempRead2[MAX_EE_SIZE-1u]<<8u) | tempRead2[MAX_EE_SIZE-2u];

   if(calcCrc1 == storedCrc1)
   {
      crc1Good = TRUE;
   } 
   if(calcCrc2 == storedCrc2)
   {
      crc2Good = TRUE;
   }    

   if((crc1Good == TRUE) && 
      (crc2Good == TRUE))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE);
      memcpy((unsigned char*)&K_Radio_Index_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE],MAX_INDEX_SIZE);
      memcpy((unsigned char*)&K_Radio_Vol_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE+MAX_INDEX_SIZE],MAX_VOL_SIZE);
   }
   else if((crc1Good == TRUE) && 
           (crc2Good != TRUE))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE);
      memcpy((unsigned char*)&K_Radio_Index_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE],MAX_INDEX_SIZE);
      memcpy((unsigned char*)&K_Radio_Vol_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE+MAX_INDEX_SIZE],MAX_VOL_SIZE);
   }
   else if((crc1Good != TRUE) && 
           (crc2Good == TRUE))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead2[0],MAX_CH_SIZE);
      memcpy((unsigned char*)&K_Radio_Index_Read[0],(unsigned char*)&tempRead2[MAX_CH_SIZE],MAX_INDEX_SIZE);
      memcpy((unsigned char*)&K_Radio_Vol_Read[0],(unsigned char*)&tempRead2[MAX_CH_SIZE+MAX_INDEX_SIZE],MAX_VOL_SIZE);
   }
   else if((crc1Good != TRUE) && 
           (crc2Good != TRUE))
   {
      for(unsigned char i=0u;i<MAX_CH;i++)
      {
         K_Radio_Data_Read[i] = FM_MIN_FREQ;
      }
      K_Radio_Index_Read[0] = 0u;
      K_Radio_Vol_Read[0] = 0u;
   }
   else
   {
   }
   App_Si_EE_Check_Pending = 1u;
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Validate_EE_Write_Data(void)
{
   memcpy((unsigned char*)&tempWrite[0],(unsigned char*)&K_Radio_Data_Write[0],MAX_CH_SIZE);
   memcpy((unsigned char*)&tempWrite[MAX_CH_SIZE],(unsigned char*)&K_Radio_Index_Write[0],MAX_INDEX_SIZE);
   memcpy((unsigned char*)&tempWrite[MAX_CH_SIZE+MAX_INDEX_SIZE],(unsigned char*)&K_Radio_Vol_Write[0],MAX_VOL_SIZE);
   calcCrc = CRC16_CCITT((unsigned char*)&tempWrite[0],MAX_EE_SIZE-2u);
   tempWrite[MAX_EE_SIZE-2u] = (unsigned char)calcCrc;
   tempWrite[MAX_EE_SIZE-1u] = (unsigned char)(calcCrc>>8u);
}


/*****************************************************************************
** Function name  : 
** Descriptions   : Function to calculate CRC16 using CRC-16-CCITT (0x8005)
** parameters     : 
** Returned value : 
*****************************************************************************/
uint16_t CRC16_CCITT(uint8_t *data, uint16_t length) 
{
    uint16_t crc = CRC16_INITIAL_VALUE;  // Initialize CRC to 0xFFFF
    uint16_t i, j;

    // Process each byte of data
    for (i = 0u; i < length; i++) 
    {
       crc ^= (data[i] << 8u);  // XOR the byte into the CRC (shifted to the high byte)

       // Process each bit of the byte
       for (j = 0u; j < 8u; j++) 
       {
           if (crc & 0x8000u) 
           {
              // If the leftmost bit is set
              crc = (crc << 1u) ^ CRC16_POLYNOMIAL;  // Shift left and XOR with the polynomial
           } 
           else 
           {
              crc <<= 1u;  // Just shift left
           }
       }
    }
    return crc;  // Return the final CRC value
}