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


uint8_t ee_state=INIT_EE;
volatile unsigned char tempRead1[MAX_EE_SIZE];
volatile unsigned char tempRead2[MAX_EE_SIZE];
volatile unsigned char tempWrite[MAX_EE_SIZE];
volatile uint16_t calcCrc=0;
volatile unsigned char crc1Good=0 ,crc2Good=0;
volatile unsigned short int storedCrc1=0,storedCrc2=0;
volatile unsigned short int calcCrc1=0,calcCrc2=0;




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
             ee_state = IDLE_EE;
             Update_EE_To_RAM();
             break;
    case UPDATE_EE:
             if(power_state == ON_STATE)
             {
                ee_state = IDLE_EE;
                Update_RAM_To_EE();
             }
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
void EE_Read(unsigned char *data,
             unsigned char length)
{
   unsigned char address =0u;
   unsigned int tempData = 0u;
   unsigned char cnt=0u;

   if((length > 0u) && ((length%2)!= 0u))
   {
      length = length + 1u;
   }
   for(cnt=0;cnt<length;)
   {
      tempData = DataEERead(address);
      data[cnt] = (unsigned char)(tempData >> 8);
      data[cnt+1] = (unsigned char)tempData;
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

   if((length > 0u) && ((length%2)!= 0u))
   {
      length = length + 1u;
   }
   for(cnt=0;cnt<length;)
   {
      tempData = (data[cnt]<<8u)|data[cnt+1];
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
   unsigned char i=0;	
   //[0.....19],[20,21],[22.23]
   calcCrc1 = CRC16_CCITT((uint8_t*)&tempRead1[0],(MAX_CH_SIZE+2u));
   storedCrc1 = (tempRead1[MAX_EE_SIZE-1u]<<8) | tempRead1[MAX_EE_SIZE-2u];
   
   //[24.....43],[44,45],[46.47]
   calcCrc2 = CRC16_CCITT((uint8_t*)&tempRead2[0],(MAX_CH_SIZE+2u));
   storedCrc2 = (tempRead2[MAX_EE_SIZE-1u]<<8) | tempRead2[MAX_EE_SIZE-2u];

   if(calcCrc1 == storedCrc1)
   {
      crc1Good = 1;
   } 
   if(calcCrc2 == storedCrc2)
   {
      crc2Good = 1;
   }    

   if((crc1Good == 1) && 
      (crc2Good == 1))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE+2u);
   }
   else if((crc1Good == 1) && 
           (crc2Good != 1))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE+2u);
   }
   else if((crc1Good != 1) && 
           (crc2Good == 1))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead2[0],MAX_CH_SIZE+2u);
   }
   else if((crc1Good != 1) && 
           (crc2Good != 1))
   {
      for(i=0;i<MAX_CH;i++)
      {
         K_Radio_Data_Read[i] = FM_MIN_FREQ;
      }
   }
   else
   {
   }
   App_Si_EE_Check_Pending = 1;
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Validate_EE_Write_Data(void)
{
   memcpy((unsigned char*)&tempWrite[0],(unsigned char*)&K_Radio_Data_Write[0],MAX_CH_SIZE+2u);
   calcCrc = CRC16_CCITT((unsigned char*)&K_Radio_Data_Write[0],MAX_CH_SIZE+2u);
   tempWrite[MAX_EE_SIZE-2u] = (unsigned char)calcCrc;
   tempWrite[MAX_EE_SIZE-1u] = (unsigned char)(calcCrc>>8);
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
