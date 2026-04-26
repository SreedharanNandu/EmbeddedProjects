#include <stdio.h>
#include "dee.h"
#include "i2c.h"
#include "radio.h"
#include <string.h>
#include "types.h"
#include "memory.h"


// CRC16 Polynomial (CRC-16-CCITT)
#define CRC16_POLYNOMIAL           0x8005u
#define CRC16_INITIAL_VALUE        0xFFFFu


volatile uint8_t ee_state;
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
             Change_Mode_EE(IDLE_EE);
             Update_EE_To_RAM();
             break;
    case UPDATE_EE:
             Change_Mode_EE(IDLE_EE);
             Update_RAM_To_EE();
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
void Update_EE_To_RAM(void)
{
   EE_Read((unsigned char*)&tempRead1[0],MAX_EE_SIZE);//copy1
   EE_Read((unsigned char*)&tempRead2[0],MAX_EE_SIZE);//copy2
   Validate_EE_Read_Data();//check if not corrupted
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Update_RAM_To_EE(void)
{
   Validate_EE_Write_Data();//check if not corrupted
   EE_Write((unsigned char*)&tempWrite[0],MAX_EE_SIZE);//copy1
   EE_Write((unsigned char*)&tempWrite[0],MAX_EE_SIZE);//copy2
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void EE_Read(unsigned char *data,
             unsigned char length)
{
   unsigned char address =0u;
   unsigned int tempData = 0u;
   unsigned char cnt=0u;

   if((length > 0u) && ((length%2u)!= 0u))
   {
      length = length + 1u;
   }
   for(cnt=0;cnt<length;)
   {
      tempData = DataEERead(address);
      data[cnt] = (unsigned char)(tempData >> 8u);
      data[cnt+1u] = (unsigned char)tempData;
      cnt = cnt + 2u;
      address = address + 1u ;
   }
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void EE_Write(unsigned char *data,
              unsigned char length)
{
   unsigned char cnt=0u;
   unsigned char address =0u;
   unsigned int tempData=0u;

   if((length > 0u) && ((length%2u)!= 0u))
   {
      length = length + 1u;
   }
   for(cnt=0u;cnt<length;)
   {
      tempData = (data[cnt]<<8u)|data[cnt+1u];
      cnt = cnt + 2u ;
      DataEEWrite(tempData,address);
      address = address + 1u ;
   }
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Validate_EE_Read_Data(void)
{
   unsigned char i=0u;	
   //[0.....19],[20,21],[22.23]
   calcCrc1 = CRC16_CCITT((uint8_t*)&tempRead1[0],(MAX_EE_SIZE-2u));
   storedCrc1 = (tempRead1[MAX_EE_SIZE-1u]<<8u) | tempRead1[MAX_EE_SIZE-2u];
   
   //[24.....43],[44,45],[46.47]
   calcCrc2 = CRC16_CCITT((uint8_t*)&tempRead2[0],(MAX_EE_SIZE-2u));
   storedCrc2 = (tempRead2[MAX_EE_SIZE-1u]<<8u) | tempRead2[MAX_EE_SIZE-2u];

   if(calcCrc1 == storedCrc1)
   {
      crc1Good = 1u;
   } 
   if(calcCrc2 == storedCrc2)
   {
      crc2Good = 1u;
   }    

   if((crc1Good == 1u) && 
      (crc2Good == 1u))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE);
      memcpy((unsigned char*)&K_Radio_Index_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE],MAX_INDEX_SIZE);
      memcpy((unsigned char*)&K_Radio_Vol_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE+MAX_INDEX_SIZE],MAX_VOL_SIZE);
   }
   else if((crc1Good == 1u) && 
           (crc2Good != 1u))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE);
      memcpy((unsigned char*)&K_Radio_Index_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE],MAX_INDEX_SIZE);
      memcpy((unsigned char*)&K_Radio_Vol_Read[0],(unsigned char*)&tempRead1[MAX_CH_SIZE+MAX_INDEX_SIZE],MAX_VOL_SIZE);
   }
   else if((crc1Good != 1u) && 
           (crc2Good == 1u))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead2[0],MAX_CH_SIZE);
      memcpy((unsigned char*)&K_Radio_Index_Read[0],(unsigned char*)&tempRead2[MAX_CH_SIZE],MAX_INDEX_SIZE);
      memcpy((unsigned char*)&K_Radio_Vol_Read[0],(unsigned char*)&tempRead2[MAX_CH_SIZE+MAX_INDEX_SIZE],MAX_VOL_SIZE);
   }
   else if((crc1Good != 1u) && 
           (crc2Good != 1u))
   {
      for(i=0u;i<MAX_CH;i++)
      {
         K_Radio_Data_Read[i] = K_Radio_Data_Write[i] = FM_MIN_FREQ;
      }
      K_Radio_Index_Read[0] = K_Radio_Index_Write[0] = 0u;
      K_Radio_Vol_Read[0] = K_Radio_Vol_Write[0] = 0u;
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
