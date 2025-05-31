/*****************************************************************************
filename:- eeprom.c
******************************************************************************/
#include<msp430.h>
#include<msp430g2553.h>
#include "type.h"





unsigned char ee_state=INIT_EE;
volatile unsigned char tempRead1[MAX_EE_SIZE];
volatile unsigned char tempRead2[MAX_EE_SIZE];
volatile unsigned char tempWrite[MAX_EE_SIZE];
volatile unsigned int calcCrc=0;
volatile unsigned char crc1Good=FALSE ,crc2Good=FALSE;
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
             ReadEE();
             Validate_EE_Read_Data();//check if not corrupted
             ee_state = IDLE_EE;
             break;
    case UPDATE_EE:
             if(power_state == ON_STATE)
             {
                Validate_EE_Write_Data();//check if not corrupted
                WriteEE();
                ee_state = IDLE_EE;
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
void Validate_EE_Read_Data(void)
{
   unsigned char i=0;
   //[0.....19],[20,21],[22.23]
   calcCrc1 = CRC16_CCITT((unsigned char*)&tempRead1[0],(MAX_CH_SIZE+2u));
   storedCrc1 = (tempRead1[MAX_EE_SIZE-1u]<<8) | tempRead1[MAX_EE_SIZE-2u];
   
   //[24.....43],[44,45],[46.47]
   calcCrc2 = CRC16_CCITT((unsigned char*)&tempRead2[0],(MAX_CH_SIZE+2u));
   storedCrc2 = (tempRead2[MAX_EE_SIZE-1u]<<8) | tempRead2[MAX_EE_SIZE-2u];

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
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE+2u);
   }
   else if((crc1Good == TRUE) && 
           (crc2Good != TRUE))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead1[0],MAX_CH_SIZE+2u);
   }
   else if((crc1Good != TRUE) && 
           (crc2Good == TRUE))
   {
      memcpy((unsigned char*)&K_Radio_Data_Read[0],(unsigned char*)&tempRead2[0],MAX_CH_SIZE+2u);
   }
   else if((crc1Good != TRUE) && 
           (crc2Good != TRUE))
   {
      for(i=MAX_CH;i>0;i--)
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
unsigned int CRC16_CCITT(unsigned char *data, unsigned int length) 
{
    unsigned int crc = CRC16_INITIAL_VALUE;  // Initialize CRC to 0xFFFF
    unsigned int i, j;

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

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void ReadEE(void)
{
   unsigned char status = 0;
   InitI2C(EE_I2C_ADDRESS);
   status = EESeqRead(PAGE_EE(0u),(unsigned char*)&tempRead1[0],MAX_EE_SIZE);
   Fault(FLT_EE_PAGE1_INIT,status);
   InitI2C(EE_I2C_ADDRESS);
   status = EESeqRead(PAGE_EE(1u),(unsigned char*)&tempRead2[0],MAX_EE_SIZE);
   Fault(FLT_EE_PAGE2_INIT,status);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void WriteEE(void)
{
   unsigned char status = 0;
   InitI2C(EE_I2C_ADDRESS);
   status = EESeqWrite(PAGE_EE(0u),(unsigned char*)&tempWrite[0],MAX_EE_SIZE);
   Fault(FLT_EE_PAGE1_INIT,status);
   InitI2C(EE_I2C_ADDRESS);
   status = EESeqWrite(PAGE_EE(1u),(unsigned char*)&tempWrite[0],MAX_EE_SIZE);
   Fault(FLT_EE_PAGE1_INIT,status);
   Delay_Ms(100u);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char EESeqRead(unsigned int startAddress , unsigned char *dPtr , unsigned int size)
{
   volatile unsigned char status=1;
   volatile unsigned int addressWord=0u;
   volatile unsigned int counterSize=0u;
   volatile unsigned char *memoryData;

   addressWord = startAddress;
   memoryData = dPtr;

   while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
   {
      if(exitLoop(14))
      {
         status = 0;
         return status;
      }
   }
   I2CWriteInit();
   UCB0CTL1 |= UCTXSTT;                      // start condition generation
   while(UCB0CTL1 & UCTXSTT)
   {
      if(exitLoop(15))
      {
         status = 0;
         return status;
      }
      if(!(UCNACKIFG & UCB0STAT))            // Break out if ACK received
      {
         break;
      }         
   }
   UCB0TXBUF = (unsigned char)(addressWord>>8);// Load TX buffer
   if(UCB0STAT & UCNACKIFG)       // nack received this should not happen if address is correct
   {
      UCB0STAT &= ~UCNACKIFG;
      UCB0CTL1 |= UCTXSTP;        //stop...
      while(UCB0CTL1 & UCTXSTP)
      {
         if(exitLoop(16))
         {
            status = 0;
            break;
         }
      }
      return status;                    //... and exit
   }
   while(!(UCB0TXIFG & IFG2))
   {
      if(exitLoop(17))
      {
         status = 0;
         break;
      }
   }

   UCB0TXBUF = (unsigned char)(addressWord);  // Load TX buffer
   if(UCB0STAT & UCNACKIFG)       // nack received this should not happen if address is correct
   {
      UCB0STAT &= ~UCNACKIFG;
      UCB0CTL1 |= UCTXSTP;        //stop...
      while(UCB0CTL1 & UCTXSTP)
      {
         if(exitLoop(18))
         {
            status = 0;
            break;
         }
      }
      return status;                    //... and exit
   }
   while(!(UCB0TXIFG & IFG2))
   {
      if(exitLoop(34))
      {
         status = 0;
         break;
      }
   }
   // Read Data byte
   I2CReadInit();
   UCB0CTL1 |= UCTXSTT;                     // I2C start condition
   while(UCB0CTL1 & UCTXSTT)                // Start condition sent?
   {
      if(exitLoop(19))
      {
         status = 0;
         return status;
      }
      if(!(UCNACKIFG & UCB0STAT))           // Break out if ACK received
      {
         break;
      }
   }
   if(UCB0STAT & UCNACKIFG)                 // nack received this should not happen if address is correct
   {
      UCB0STAT &= ~UCNACKIFG;
      UCB0CTL1 |= UCTXSTP;                  //stop...
      while(UCB0CTL1 & UCTXSTP)
      {
         if(exitLoop(20))
         {
            status = 0;
            return status;
         }
      }
      while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
      {
         if(exitLoop(21))
         {
            status = 0;
            break;
         }
      }
      return status;                              //... and exit
   }

    while(1)
    {
      if(UCB0RXIFG & IFG2)
      {
         if(counterSize >= size)
         {
            IFG2 &= ~UCB0RXIFG;
            //one more  byte to read send stop
            UCB0CTL1 |= UCTXSTP; //I2C stop condition
            while(UCB0CTL1 & UCTXSTP)
            {
               if(exitLoop(22))
               {
                  status = 0;
                  break;
               }
            }
            while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
            {
               if(exitLoop(23))
               {
                  status = 0;
                  break;
               }
            }
            break;            
         }
         else 
         {
            *memoryData= UCB0RXBUF;
            *memoryData++;
            counterSize++;
         }         
      }         
   }
   return status;
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char EESeqWrite(unsigned int startAddress , unsigned char *dPtr , unsigned int size)
{
   volatile unsigned char status=1;
   volatile unsigned int addressWord=0u;
   volatile unsigned int counterSize=0u;
   volatile unsigned char *memoryData;

   addressWord = startAddress;
   memoryData = dPtr;
   
   while(UCB0STAT & UCBUSY)                // wait until I2C module has
   {
      if(exitLoop(24))
      {
         status = 0;
         return status;
      }
   }
   I2CWriteInit();
   UCB0CTL1 |= UCTXSTT;                      // start condition generation
   while(UCB0CTL1 & UCTXSTT)
   {
      if(exitLoop(25))
      {
         status = 0;
         return status;
      }
      if(!(UCNACKIFG & UCB0STAT))            // Break out if ACK received
      {
         break;
      }         
   }
   UCB0TXBUF = (unsigned char)(addressWord>>8);// Load TX buffer
   if(UCB0STAT & UCNACKIFG)       // nack received this should not happen if address is correct
   {
      UCB0STAT &= ~UCNACKIFG;
      UCB0CTL1 |= UCTXSTP;        //stop...
      while(UCB0CTL1 & UCTXSTP)
      {
         if(exitLoop(26))
         {
            status = 0;
            break;
         }
      }
      return status;                    //... and exit
   }
   while(!(UCB0TXIFG & IFG2))
   {
      if(exitLoop(27))
      {
         status = 0;
         return status;
      }
   }
   UCB0TXBUF = (unsigned char)(addressWord);  // Load TX buffer
   if(UCB0STAT & UCNACKIFG)       // nack received this should not happen if address is correct
   {
      UCB0STAT &= ~UCNACKIFG;
      UCB0CTL1 |= UCTXSTP;        //stop...
      while(UCB0CTL1 & UCTXSTP)
      {
         if(exitLoop(28))
         {
            status = 0;
            break;
         }
      }
      return status;                    //... and exit
   }
   while(!(UCB0TXIFG & IFG2))
   {
      if(exitLoop(29))
      {
         status = 0;
         return status;
      }
   }
   for(counterSize=0;counterSize<size;counterSize++)   
   {
      UCB0TXBUF = *memoryData;  // Load TX buffer
      while(!(UCB0TXIFG & IFG2))
      {
         if(exitLoop(30))
         {
            status = 0;
            return status;
         }
      }
      *memoryData++;     
      if(UCB0STAT & UCNACKIFG)       // nack received this should not happen if address is correct
      {
         UCB0STAT &= ~UCNACKIFG;
         UCB0CTL1 |= UCTXSTP;        //stop...
         while(UCB0CTL1 & UCTXSTP)
         {
            if(exitLoop(31))
            {
               status = 0;
               return status;
            }
         }
         while(UCB0STAT & UCBUSY)                // wait until I2C module is idle
         {
           if(exitLoop(32))
           {
              status = 0;
              break;
           }
         }
         return status;
      }
   }
   UCB0CTL1 |= UCTXSTP;        //stop...while(UCB0CTL1 & UCTXSTP);
   while(UCB0STAT & UCBUSY)
   {
     if(exitLoop(33))
      {
         status = 0;
         break;
      }
   } // wait until I2C module is idle
   return status;
}


