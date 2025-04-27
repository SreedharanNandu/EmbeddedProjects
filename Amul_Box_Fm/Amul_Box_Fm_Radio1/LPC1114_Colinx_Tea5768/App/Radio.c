/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "uart.h"
#include "type.h"
#include "i2c.h"
#include "system.h"
#include <string.h>
#include "timer16.h"

volatile seek_t seek;
state_t radio_state;
volatile unsigned char preset,index;
volatile unsigned int pll;
volatile unsigned char K_Radio_Data_Read[21];
volatile unsigned char K_Radio_Data_Write[21];//10 channels of 2 bytes each and 1 byte index number
volatile unsigned char disp_freq_once;
volatile unsigned char standby,mute;

radio_read_t Radio_Read;
radio_write_t Radio_Write;

void Check_Radio(void);
static void Standby_Radio(unsigned char stby);
static void Mute_Radio(unsigned char mute);


/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void Init_Radio(void)
{
   standby = 0;

   pll = PLL_MIN;

   Radio_Write.data_byte1.bits.mute = 0;
   Radio_Write.data_byte1.bits.sm = 0;
   Radio_Write.data_byte1.bits.pll = 0x1b;

   Radio_Write.data_byte2.bits.pll = 0x6a;

   Radio_Write.data_byte3.bits.sud = 0;
   Radio_Write.data_byte3.bits.ssl = 0;
   Radio_Write.data_byte3.bits.hlsi = 1;
   Radio_Write.data_byte3.bits.ms = 0;
   Radio_Write.data_byte3.bits.mr = 0;
   Radio_Write.data_byte3.bits.ml = 0;
   Radio_Write.data_byte3.bits.swp1 = 1;

   Radio_Write.data_byte4.bits.swp2 = 1;
   Radio_Write.data_byte4.bits.stby = 0;
   Radio_Write.data_byte4.bits.bl = 0;
   Radio_Write.data_byte4.bits.xtal = 0;
   Radio_Write.data_byte4.bits.smute = 1;
   Radio_Write.data_byte4.bits.hcc = 1;
   Radio_Write.data_byte4.bits.snc = 1;
   Radio_Write.data_byte4.bits.si = 0;

   Radio_Write.data_byte5.bits.pllref = 0;
   Radio_Write.data_byte5.bits.dtc = 0;

   preset = 1;
   EE_Read(0u,(unsigned char*)&K_Radio_Data_Read[0],20);
   index = (unsigned char)K_Radio_Data_Read[20];
   if(index >= 20)//since index increments by 2
   {
     index = 0;
   }
   Audio_Mute(ON);
   pll = K_Radio_Data_Read[index];
   pll = (pll<<8)| K_Radio_Data_Read[index+1];
   Radio_Write.data_byte1.bits.pll = pll>>8;
   Radio_Write.data_byte2.bits.pll = (unsigned char)pll;
   Write_Radio((unsigned char*)&Radio_Write,5);
   Delay_Ms(100);
   Audio_Mute(OFF);

}



void Process_Radio(void)
{
   #if TEST_RADIO
   static unsigned char flip=0;
   unsigned char lev1,lev2;
   #endif
   switch(radio_state)
   {
    case INIT:
             #if TEST_RADIO
             flip = !flip;
             Radio_Write.data_byte3.bits.swp1=flip;
             Radio_Write.data_byte4.bits.swp2=!flip;
             Write_Radio((unsigned char*)&Radio_Write,5);
             #endif
             radio_state = UPDATE;
             break;
    case UPDATE:
             memset((void*)&Radio_Read,0,5);
             Read_Radio((unsigned char*)&Radio_Read,5);
             radio_state = IDLE;
             Start_Timer(TMR4,100u);
             break;
    case IDLE:
             if(Get_Timer_Status(TMR4) == TMR_EXPIRED)
             {
                Check_Radio();
                radio_state = INIT;
             }
             break;
    default:
             break;

   }
}
void Check_Radio(void)
{
   unsigned int freq=0;
    
   if(disp_freq_once)
   {
      disp_freq_once = 0;
      memset((void*)&Radio_Read,0,5);
      Read_Radio((unsigned char*)&Radio_Read,5);
      freq = Convert_PLL_To_Freq();
      UART_Sprintf("\n\rFreq : %dMhz", freq);

      #ifdef DISP_SIGNAL_INFO
      UART_Sprintf("\n\rIF:0x%X", Radio_Read.data_byte3.bits.pll_if);
      UART_Sprintf("\n\rLEVEL:0x%X", Radio_Read.data_byte4.bits.lev);
      #endif

      if((Radio_Read.data_byte3.bits.pll_if <0x3f ) && 
            (Radio_Read.data_byte3.bits.pll_if > 0x31) &&
            (Radio_Read.data_byte4.bits.lev >=7))
      {
         UART_Printf("\n\rTUNED°");
      } 
      if(Radio_Read.data_byte3.bits.stereo)
      {
         Stereo_Led(TRUE);
         UART_Printf("\n\rSTEREO°");
      }
			else
			{
         Stereo_Led(FALSE);
			}
   }
}
void Write_Radio(unsigned char *data,unsigned char length)
{

   I2CStart();

   //-- Set ADDRESS
   Send_I2C(0xC0);
   while(LPC_I2C->STAT != 0x18);
   


   while(length)
   {
      Send_I2C(*data);
      while(LPC_I2C->STAT != 0x28);
      length-- ;
      *data++;
   }

 
   I2CStop();
}

static void Standby_Radio(unsigned char stby)
{

   if(!stby)
   {
      Radio_Write.data_byte4.bits.stby = 0;
      Write_Radio((unsigned char*)&Radio_Write,5);
   }
   else
   {
      Radio_Write.data_byte4.bits.stby = 1;
      Write_Radio((unsigned char*)&Radio_Write,5);
   }

}

static void Mute_Radio(unsigned char mute)
{

   if(!mute)
   {
      Radio_Write.data_byte1.bits.mute = 0;
      Write_Radio((unsigned char*)&Radio_Write,5);
   }
   else
   {
      Radio_Write.data_byte1.bits.mute = 1;
      Write_Radio((unsigned char*)&Radio_Write,5);
   }

}


void Read_Radio(unsigned char *data,unsigned char length)
{


   I2CStart();

   Send_I2C(0xC0 | 0x01);
   if(LPC_I2C->STAT != 0x40)
   {
      I2CStop();
      return ;
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

void Process_Commands(void)
{
   unsigned char val=0;

   if(UARTFlag == 1)
   {
      UARTFlag = 0;
      NVIC_DisableIRQ(UART_IRQn);
      if((UARTData[0] == 0x55u) && (UARTData[2] == 0xAAu))
      {
         val = UARTData[1];
      }
      Execute_Commands(val);
      NVIC_EnableIRQ(UART_IRQn);
   }
}

void Execute_Commands(unsigned char val)
{
   unsigned int freq;
   switch(val)
   {
      case '{':
              if(!standby)
              {
                 Mute_Radio(TRUE);
              }
              break;
      case '}':
              if(!standby)
              {
                 Mute_Radio(FALSE);
              }
              break;
       case ']':
              if(standby)
              {
                 Standby_Radio(FALSE);
                 standby = FALSE;
              }
              break;
			 case '[':
              if(!standby)
              {
                 Standby_Radio(TRUE);
                 standby = TRUE;
              }
              break;
      case '+':
              if(!standby)
              {
                 if(preset)
                 {
                    index+=2;
                    if(index < 20)
                    {
                    }
                    else
                    {
                       index = 18;
                    }
                    UART_Sprintf("\n\rChannel No:-%d", index/2);
                    EE_Read((index),(unsigned char*)&K_Radio_Data_Read[index],2);
                    pll = K_Radio_Data_Read[index];
                    pll = (pll<<8)| K_Radio_Data_Read[index+1];
                    freq = ((pll*12500) - 225000)/1000;
                 }
                 else
                 {
                    if(pll<PLL_MAX)
                    {
                       pll = pll+8;
                    }
                 }
                 Stereo_Led(FALSE);
                 disp_freq_once = 1;
              }
              break;
      case '-':
              if(!standby)
              {
                 if(preset)
                 {
                    if(index > 0)
                    {
                       index-=2;
                    }
                    UART_Sprintf("\n\rChannel No:-%d", index/2);
                    EE_Read(index,(unsigned char*)&K_Radio_Data_Read[index],2);
                    pll = K_Radio_Data_Read[index];
                    pll = (pll<<8)| K_Radio_Data_Read[index+1];
                    freq = ((pll*12500) - 225000)/1000;
                 }
                 else
                 {
                    if(pll>PLL_MIN)
                    {
                       pll = pll-8;
                    }
                 }
                 Stereo_Led(FALSE);
                 disp_freq_once = 1;
              }
              break;
      case 'S':
              if(!standby)
              {
                 if(!preset)
                 { 
                    preset = 1;
                 }
                 K_Radio_Data_Write[index] = pll>>8;
                 K_Radio_Data_Write[index+1] = (unsigned char)pll;
                 EE_Write(index,(unsigned char*)&K_Radio_Data_Write[index],2);
                 K_Radio_Data_Write[20] = index;
                 EE_Write(20,(unsigned char*)&K_Radio_Data_Write[20],1);
                 freq = Convert_PLL_To_Freq();
                 UART_Sprintf("\n\rChannel %d Saved", index/2);
                 Stereo_Led(FALSE);
              }
              break;
      case 'M':
              if(!standby)
              {
                 preset =! preset;
                 if(preset)
                 {
                    index = 0;
                    UART_Printf("\n\rPreset Mode");
                    UART_Sprintf("\n\rChannel No:-%d", index/2);
                    EE_Read((index),(unsigned char*)&K_Radio_Data_Read[index],2);
                    pll = K_Radio_Data_Read[index];
                    pll = (pll<<8)| K_Radio_Data_Read[index+1];
                    freq = ((pll*12500) - 225000)/1000;
                 }
                 else
                 {
                     UART_Printf("\n\rNormal Mode");
                 }
                 disp_freq_once = 1;
                 Stereo_Led(FALSE);
                 Mode_Led(preset);
              }
              break;
       default :
              break;
   }    

   Audio_Mute(ON);
   Radio_Write.data_byte1.bits.pll = pll>>8;
   Radio_Write.data_byte2.bits.pll = (unsigned char)pll;
   Write_Radio((unsigned char*)&Radio_Write,5);
   Delay_Ms(100);
   Audio_Mute(OFF);

}


unsigned int Convert_PLL_To_Freq(void)
{
   unsigned int pll_read,freq;
   pll_read = (Radio_Read.data_byte1.bits.pll);
   pll_read = (pll_read<<8);
   pll_read = (pll_read | Radio_Read.data_byte2.bits.pll);
   freq = ((pll_read*12500) - 225000)/1000;
   return(freq);

}

