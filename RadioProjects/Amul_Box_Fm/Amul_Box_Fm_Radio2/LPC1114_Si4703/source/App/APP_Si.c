/*----------------------------------------------------------------------------
 * Name:   App_Si.c
 *----------------------------------------------------------------------------*/

/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/


/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/

#include "LPC11xx.h"
#include "APP_Si.h"   
#include "APP_Buttons.h"   
#include "App_I2c_Intf.h"
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "timer16.h"
#include "App_Eeprom.h"
#include "App_Led.h"
#include "uart.h"
#include "Cal_Const.h"

/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/
// allowed chars in RDS
#define FIRST_ALLOWED_CHAR (0x20)
#define LAST_ALLOWED_CHAR (0x7f)


/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/

unsigned char App_Si_EE_Check_Pending;
unsigned char power_state;
rds_data_T rds_data;
unsigned char rds_mode;
volatile si4703_read_reg_T si4703_read_reg;
volatile radio_T radio_state;
volatile unsigned char Is_Manual_Tune;
static volatile unsigned char Is_Vol_Changed;
volatile unsigned char preset_mode;
unsigned short int fm_freq,prev_fm_freq;
unsigned char preset_index;
unsigned char si_vol;
unsigned char fm_stereo;
unsigned char rdsText[65];
unsigned char rdsName[9];
unsigned short int K_Radio_Data_Read[MAX_CH];
unsigned short int K_Radio_Data_Write[MAX_CH];
unsigned short int K_Radio_Index_Read[1];
unsigned short int K_Radio_Index_Write[1];
unsigned short int K_Radio_Vol_Read[1];
unsigned short int K_Radio_Vol_Write[1];
unsigned short int reset_Vol_Timer;
unsigned short int reset_Index_Timer;
char printBuffer[PRINTBUFSIZE];
unsigned char first_time_power_up;

/*****************************************************************************
*                            Function declarations                          *
******************************************************************************/
static void Write_si4703(void);
void Power_Radio(void);
void Get_Rev(void);
void Set_Fm_Tune_Freq(unsigned short int freq);
void Set_Vol(unsigned char vol_level);
void Set_Dig_Out_Fmt(void);
void Get_Int_Status(void);
void Get_Fm_Rsq_Status(void);
void Get_Fm_RDS_Status(void);
void Seek_Thresholds(unsigned char SNR, unsigned char RSSI);
void ptystr(char pty);
void Refresh_Display(int next_stn);
void Read_All_Si4703_Registers(void);
unsigned char RDS_Process(unsigned char* ptr, unsigned char*ptPtr);


/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Init_Si(void)
{
  /*
   To  use  this  busmode
   selection  method,  the  GPIO3  and  SDIO  pins  must  be
   sampled low by the device on the rising edge of RST.
   
   The user may either drive the GPIO3 pin low externally,
   or leave the pin floating. If the pin is not driven by the
   user,  it  will be pulled low  by an  internal  1 M?  resistor
   which  is  active  only  while  RST  is  low.  The  user  must
   drive the SEN and SDIO pins externally to the proper
   state.
   
   To  select  2-wire  operation,  the  SEN  pin  must  be
   sampled high by the device on the rising edge of RST.   */
   
   LPC_IOCON->PIO0_5 = 0u;
   GPIOSetValue(PORT0,5u ,0u); //SDI (SDA) must be LOW as per data sheet 
   Delay_Ms(200u);
   //si in reset state
   si4703_Rst(0u);
   //make RSEN High
   si4703_Rsen(1u);
   Delay_Ms(100u);
   //out of reset    
   si4703_Rst(1u);
   Delay_Ms(100u);
   LPC_IOCON->PIO0_5 = 1u;//Reconfigure to I2C SDA pin
   Init_I2C();
   Delay_Ms(300u);//Reconfigure delay for I2C
   Power_Radio();
   Is_Manual_Tune = 0u;
   power_state = ON_STATE;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void PreInit_Radio(void)
{
   unsigned char i;
   for(i=0u;i<MAX_CH;i++)
   {
      K_Radio_Data_Read[i] = FM_MIN_FREQ;
      K_Radio_Data_Write[i] = FM_MIN_FREQ;
   }
   K_Radio_Index_Read[0] = 0u;
   K_Radio_Index_Write[0] = 0u;
   K_Radio_Vol_Read[0] = 0u;
   K_Radio_Vol_Write[0] = 0u;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Init_Radio(void)
{
   radio_state = RADIO_INIT;
   Init_Si();
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
static void Write_si4703(void)
{
   unsigned char data_wr[12];
  
   data_wr[0] = si4703_read_reg.powercfg>>8u;
   data_wr[1] = (unsigned char)si4703_read_reg.powercfg;
   data_wr[2] = si4703_read_reg.channel>>8u;
   data_wr[3] = (unsigned char)si4703_read_reg.channel;
   data_wr[4] = si4703_read_reg.sysconfig1>>8u;
   data_wr[5] = (unsigned char)si4703_read_reg.sysconfig1;
   data_wr[6] = si4703_read_reg.sysconfig2>>8u;
   data_wr[7] = (unsigned char)si4703_read_reg.sysconfig2;
   data_wr[8] = si4703_read_reg.sysconfig3>>8u;
   data_wr[9] = (unsigned char)si4703_read_reg.sysconfig3;
   data_wr[10] = si4703_read_reg.test1>>8u;
   data_wr[11] = (unsigned char)si4703_read_reg.test1;
   Write_Si_I2C((unsigned char*)&data_wr[0],12u);
   
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Read_si4703(void)
{
    volatile unsigned char data_rd[32];

    Read_Si_I2C((unsigned char*)&data_rd[0],32u);
    si4703_read_reg.statusrssi     = (data_rd[0]<<8u)|data_rd[1]; 
    si4703_read_reg.readchan       = (data_rd[2]<<8u)|data_rd[3]; 
    si4703_read_reg.rdsa           = (data_rd[4]<<8u)|data_rd[5]; 
    si4703_read_reg.rdsb           = (data_rd[6]<<8u)|data_rd[7]; 
    si4703_read_reg.rdsc           = (data_rd[8]<<8u)|data_rd[9]; 
    si4703_read_reg.rdsd           = (data_rd[10]<<8u)|data_rd[11]; 
    si4703_read_reg.device_id      = (data_rd[12]<<8u)|data_rd[13]; 
    si4703_read_reg.chipid         = (data_rd[14]<<8u)|data_rd[15];
    si4703_read_reg.powercfg       = (data_rd[16]<<8u)|data_rd[17];
    si4703_read_reg.channel        = (data_rd[18]<<8u)|data_rd[19];
    si4703_read_reg.sysconfig1     = (data_rd[20]<<8u)|data_rd[21];
    si4703_read_reg.sysconfig2     = (data_rd[22]<<8u)|data_rd[23];
    si4703_read_reg.sysconfig3     = (data_rd[24]<<8u)|data_rd[25];
    si4703_read_reg.test1          = (data_rd[26]<<8u)|data_rd[27];
    si4703_read_reg.test2          = (data_rd[28]<<8u)|data_rd[29];
    si4703_read_reg.bootconfig     = (data_rd[30]<<8u)|data_rd[31];
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Power_Radio(void)
{
   Read_si4703();
   Delay_Ms(200u);//delay for XTAL stabilization
   si4703_read_reg.test1 = 0x8100u;
   Write_si4703();
   Delay_Ms(300u);//delay for XTAL stabilization
   si4703_read_reg.powercfg = 0x4001u;
   Write_si4703();
   si4703_read_reg.sysconfig1 = 0x1080u; 
   Write_si4703();
   si4703_read_reg.sysconfig2 &= ~(3u<<4u) ; 
   si4703_read_reg.sysconfig2 &= 0xFFF0u ; 
   si4703_read_reg.sysconfig2 |= 0x0010u ; //make very volume MUTE
   Write_si4703();

}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_Vol(unsigned char vol_level)
{
   si4703_read_reg.sysconfig2 &= 0xFFF0u ; //make mute
   si4703_read_reg.sysconfig2 |= (unsigned short int)(vol_level) ; //make very low volume
   Write_si4703();
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Seek_Fm_Start(unsigned char seek)
{
   Read_si4703();
   si4703_read_reg.powercfg &= ~(1u<<10u);
   if(seek == SEEK_DWN) 
   {
      si4703_read_reg.powercfg &= ~(1u<<9u);
   }
   else
   { 
      si4703_read_reg.powercfg |= 1u<<9u; //Set the bit to seek up
   }
   si4703_read_reg.powercfg |= (1u<<8u); //Start seek
   Write_si4703(); //Seeking will now start
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_Fm_Tune_Freq(unsigned short int freq)
{
   unsigned char scale = 0u;
   unsigned short int cnt;
   if((freq > FM_MIN_FREQ) & (freq <= FM_MAX_FREQ))
   {
      scale = (freq - FM_MIN_FREQ)/10u;
   }
   Read_si4703();      
   si4703_read_reg.channel &= 0xFE00u; //Clear out the channel bits
   si4703_read_reg.channel |= scale; //Mask in the new channel
   si4703_read_reg.channel |= 0x8000u; //Set the TUNE bit to start
   Write_si4703();      
   cnt  = SET_FM_TUNE_RSSI_THRESHOLD;
   while(cnt)
   {
     cnt -- ;
     Read_si4703();      
     if((si4703_read_reg.statusrssi & (1u<<14u)) != 0u)//Tune complete
     {
        break;
     }
   }
   Read_si4703();
   si4703_read_reg.channel  &= ~(1u<<15u); 
   Write_si4703();      
   cnt  = SET_FM_TUNE_RSSI_THRESHOLD;
   while(cnt)
   {
     cnt--;
     Read_si4703();      
     if((si4703_read_reg.statusrssi & (1u<<14u)) == 0u)
     {
        break;
     }
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned short int Get_Fm_Tune_Status(void)
{
   return(si4703_read_reg.statusrssi);
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Tune_Operation(unsigned char dir)
{
   if((ON_STATE == power_state ))
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         if(preset_mode)
         {
            if(preset_index < (MAX_CH-1U))
            {
               preset_index++;
               if(K_Radio_Data_Read[preset_index] < FM_MAX_FREQ)
               {
                  fm_freq = K_Radio_Data_Read[preset_index];
                  Is_Manual_Tune = 1u;
                  UART_Sprintf("\n\rChannel No:-%d", preset_index);
                  UART_Sprintf("\n\rFrequency:-%d", fm_freq);
                  reset_Index_Timer = RESET_INDEX_TIMER_TH;
               }
            }
         }
         else
         {
            if(fm_freq < FM_MAX_FREQ)
            {
               fm_freq = fm_freq + 10u;
               Is_Manual_Tune = 1u;
               UART_Sprintf("\n\rFrequency:-%d", fm_freq);
            }
         }
      }
      else if(dir == LEFT_TURN)/*left*/
      {
         if(preset_mode)
         {
            if(preset_index > 0u)
            {
               preset_index--;
               if(K_Radio_Data_Read[preset_index] > FM_MIN_FREQ)
               {
                  fm_freq = K_Radio_Data_Read[preset_index];
                  Is_Manual_Tune = 1u;
                  UART_Sprintf("\n\rChannel No:-%d", preset_index);
                  UART_Sprintf("\n\rFrequency:-%d", fm_freq);
                  reset_Index_Timer = RESET_INDEX_TIMER_TH;
               }
            }
         }
         else
         {
            if(fm_freq > FM_MIN_FREQ)
            {
               fm_freq = fm_freq - 10u;
               Is_Manual_Tune = 1u;
               UART_Sprintf("\n\rFrequency:-%d", fm_freq);
            }
         }
      }
      else
      {
         /*do nothing*/
      }
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Volume_Operation(unsigned char dir)
{
   if((ON_STATE == power_state ))
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         if(si_vol < SI_VOL_MAX)
         {
           if(si_vol == SI_VOL_MUTE)
           {
              Amp_Mute(OFF);
           }
           si_vol++;
         }             
         Is_Vol_Changed = 1u;
         reset_Vol_Timer = RESET_VOL_TIMER_TH;
         UART_Sprintf("\nVol:-%d", si_vol);
      }
      else if(dir == LEFT_TURN)/*left*/
      {
         if(si_vol > SI_VOL_MUTE)
         {
           si_vol--;
           if(si_vol == SI_VOL_MUTE)
           {
              Amp_Mute(ON);
           }
         }             
         Is_Vol_Changed = 1u;
         reset_Vol_Timer = RESET_VOL_TIMER_TH;
         UART_Sprintf("\nVol:-%d", si_vol);
      }
      else
      {
         /*do nothing*/
      }
   }
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Tune_Button(unsigned char duration)
{
   if(power_state == ON_STATE)
   {
      if(duration == SHORT_PRESS)
      {
         preset_mode = !preset_mode;
         if(preset_mode)
         {
            UART_Printf("\n\rPreset Mode");
            UART_Sprintf("\n\rChannel No:-%d", preset_index);
            fm_freq = K_Radio_Data_Read[preset_index];
         }
         else
         {
            UART_Printf("\n\rManual Mode");
         }
         UART_Sprintf("\n\rFrequency:-%d", fm_freq);
         Is_Manual_Tune = 1u;
         Led(blink_ModeChange);
      }
      else if(duration == LONG_PRESS)
      {
         K_Radio_Data_Write[preset_index] = fm_freq;
         K_Radio_Data_Read[preset_index] = K_Radio_Data_Write[preset_index];
         K_Radio_Index_Read[0] = preset_index;
         K_Radio_Index_Write[0] = preset_index;
         Change_Mode_EE(UPDATE_EE);
         preset_mode = 1u;
         UART_Sprintf("\n\rFrequency:-%d", fm_freq);
         UART_Sprintf("\n\rChannel %d Saved", preset_index);
         Led(blink_Store);
      }
      else
      {
      
      }
   }
}




/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Pwr_Button(unsigned char duration)
{
   if(duration == SHORT_PRESS)
   {
      if(power_state == ON_STATE)
      {
         power_state = OFF_STATE;
      }
      else if(power_state == OFF_STATE)
      {
         power_state = ON_STATE;
      }
      else
      {
      }
   }
   else if(duration == LONG_PRESS)
   {
      if(power_state == ON_STATE)
      {
         power_state = OFF_STATE;
      }
      else if(power_state == OFF_STATE)
      {
         power_state = ON_STATE;
      }
      else
      {
      }
   
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Get_RDS_Info(void)
{
   if(si4703_read_reg.statusrssi & (1u<<15u))
   {
      if(RDS_Process((unsigned char*)rdsName,(unsigned char*)rdsText)>0u)
      {
         UART_Printf("\nRDS Name:-");
         UART_Printf((char*)rdsName);
         UART_Printf("\nRDS Text:-");
         UART_Printf((char*)rdsText);
         memset(rdsName,0u,9u);
         memset(rdsText,0u,65u);
      }
   }
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
unsigned char RDS_Process(unsigned char* psPtr, unsigned char*ptPtr)
{ 
   volatile unsigned char ret_state = 0u;
   static unsigned char temp_rdsdata[9];
   static unsigned char temp_rdstext[65];
   volatile unsigned char groupid;
   volatile unsigned char index_ps;
   volatile unsigned char index_pt;
   volatile char Ch,Dh;
   volatile char Cl,Dl;
   volatile unsigned short int b;
   volatile unsigned short int c;
   volatile unsigned short int d;      

   temp_rdsdata[8] = 0u;//only 8 characters allowed
   temp_rdstext[64] = 0u;//only 64 charaters allowed

   b = si4703_read_reg.rdsb;
   c = si4703_read_reg.rdsc;
   d = si4703_read_reg.rdsd;

   if(prev_fm_freq != fm_freq)
   {
      memset(temp_rdsdata,0u,9u);
      memset(temp_rdstext,0u,65u);
   }

   groupid = (b & 0xF000u) >> 12u;
   switch(groupid) 
   {
      case RDS_PS: 
                  index_ps = (b & 0x3u)*2u;//i.e 0,2,4,6 totall b=[3,2,1,0] i.e 4 * 2 i.e Dh and Dl so total 8 characters
                  Dh = (d & 0xFF00u) >> 8u;
                  Dl = d;
                  temp_rdsdata[index_ps] = Dh;
                  temp_rdsdata[index_ps+1u] = Dl;
                  memcpy((char*)psPtr, (char*)&temp_rdsdata[0],9u);
                  ret_state  = 1u;
                  break;
      case RDS_RT: 
                  index_pt = (b & 0xFu)*4u;//per address code has 4 charaters ,so totally 16*4 = 64 charater
                  Ch = (c & 0xFF00u) >> 8u;
                  Cl = c;
                  Dh = (d & 0xFF00u) >> 8u;
                  Dl = d;
                  temp_rdstext[index_pt] = Ch;
                  temp_rdstext[index_pt+1u] = Cl;
                  temp_rdstext[index_pt+2u] = Dh;
                  temp_rdstext[index_pt+3u] = Dl;
                  memcpy((char*)ptPtr, (char*)&temp_rdstext[0],65u);
                  ret_state  = 2u;
                  break;
          default:
                  break;
   }
   return (ret_state);
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_GPIO1(unsigned char state)
{
   Read_si4703();
   si4703_read_reg.sysconfig1 &= 0xFFFCu;
   si4703_read_reg.sysconfig1 |= Set_GPIO1_Val(!state);
   Write_si4703();
   
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Set_GPIO2(unsigned char state)
{
   si4703_read_reg.sysconfig1 &= 0xFFF3u;
   si4703_read_reg.sysconfig1 |= Set_GPIO2_Val(!state);
   Write_si4703();
   
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Validate_FM_Data(void)
{
   unsigned char i = 0u;
   for(i=0u;i<MAX_CH;i++)
   {
      if((K_Radio_Data_Read[i] > FM_MAX_FREQ) || (K_Radio_Data_Read[i] < FM_MIN_FREQ))
      {
         K_Radio_Data_Read[i] = FM_MIN_FREQ;
         K_Radio_Data_Write[i] = FM_MIN_FREQ;
      }
      else
      {
         K_Radio_Data_Write[i] = K_Radio_Data_Read[i];
      }
   }    
   if(K_Radio_Index_Read[0] > (MAX_CH-1u))
   {
      K_Radio_Index_Read[0] = (unsigned short int)0u;
      K_Radio_Index_Write[0] = (unsigned short int)0u;
      preset_index = 0u;
   }
   else
   {
      preset_index = (unsigned char)K_Radio_Index_Read[0];
      K_Radio_Index_Write[0] = (unsigned short int)preset_index;
   }

   if(K_Radio_Vol_Read[0] > SI_VOL_MAX)
   {
      K_Radio_Vol_Read[0] = (unsigned short int)0u;
      K_Radio_Vol_Write[0] = (unsigned short int)0u;
   }
   else
   {
      K_Radio_Vol_Write[0] = K_Radio_Vol_Read[0];
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Process_App_Si(void)
{
   static unsigned char disp_st_timer;
   static unsigned char prev_power_state;
   static unsigned char prev_fm_stereo;
   static unsigned char Scan_Cnt;
   static unsigned char stc_wait_to_clear;
   
   switch(radio_state)
   {
      case RADIO_RUNNING:
           if(power_state == OFF_STATE)
           {
              radio_state = RADIO_OFF;
              Set_Vol(SI_VOL_MUTE);
              Set_GPIO2(FALSE);//Blue Led Power OFF
              UART_Printf("\n\rPower OFF");
              Amp_Mute(ON);
           }
           else if(power_state == ON_STATE)
           {
              if(reset_Vol_Timer > 0u)
              {
                 reset_Vol_Timer--;
                 if(reset_Vol_Timer == 0u)
                 {
                    K_Radio_Vol_Read[0] = si_vol;
                    K_Radio_Vol_Write[0] = si_vol;
                    Change_Mode_EE(UPDATE_EE);
                 }
              }
              if(reset_Index_Timer > 0u)
              {
                 reset_Index_Timer--;
                 if(reset_Index_Timer == 0u)
                 {
                    K_Radio_Index_Read[0] = preset_index;
                    K_Radio_Index_Write[0] = preset_index;
                    Change_Mode_EE(UPDATE_EE);
                 }
              }
              if(Is_Vol_Changed)
              {
                 if((si_vol >= SI_VOL_MUTE) && 
                    (si_vol <= SI_VOL_MAX))
                 {
                    Set_Vol(si_vol);
                 }
                 Is_Vol_Changed = 0u;
              }
              
              if(Is_Manual_Tune)
              {
                 Set_Fm_Tune_Freq(fm_freq);
                 Is_Manual_Tune = 0u;
                 fm_stereo = 0u;
              }
              else
              {
                 if(rds_mode)
                 {
                    Scan_Cnt++;
                    if(Scan_Cnt == 10u)
                    {
                       Scan_Cnt = 0u;
                       Read_si4703();
                       Get_RDS_Info();
                       if(Get_Fm_Tune_Status() & 0x0100U)
                       {
                         fm_stereo = 1u;
                       }
                       else
                       {
                         fm_stereo = 0u;
                       }
                       if(prev_fm_stereo != fm_stereo)
                       {
                          disp_st_timer = 4u;
                          prev_fm_stereo = fm_stereo;
                       }
                       if(disp_st_timer != 0u)
                       {
                          disp_st_timer--;
                       }
                    }
                 }
              }
              if(prev_fm_freq != fm_freq)
              {
                 prev_fm_freq = fm_freq;
              }
           }
           break;   
      case RADIO_INIT:
           if(power_state == ON_STATE)
           {
              if(App_Si_EE_Check_Pending)
              {
                 App_Si_EE_Check_Pending = 0u;
                 Validate_FM_Data();
              }
              if(!first_time_power_up)
              {
                 first_time_power_up = 1u;
                 fm_freq = K_Radio_Data_Read[preset_index];
                 si_vol = K_Radio_Vol_Read[0];
              }
              Set_Fm_Tune_Freq(fm_freq);
              preset_mode = 0u;               
              radio_state = RADIO_RUNNING;
              Is_Vol_Changed = 1u;
              rds_mode = 0u;
              Set_GPIO2(TRUE);//Blue Led Power ON
              UART_Printf("\n\rRadio Initialized");
              UART_Sprintf("\n\rFrequency:-%d", fm_freq);
              Set_Vol(si_vol);
              if(si_vol == 0u)
              { 
                 Amp_Mute(ON);
              }
              else
              {
                 Amp_Mute(OFF);
              }
           }
           break;   
      default:
      case RADIO_OFF:
           if(power_state == ON_STATE)
           {
              radio_state = RADIO_INIT;
              UART_Printf("\n\rPower ON");
           }
           else
           {
              Enter_Sleep();
           }
           
           break;   
   }
   prev_power_state = power_state;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Process_Commands(void)
{
   unsigned char val=0u;
   if(UARTFlag == 1u)
   {
      UARTFlag = 0u;
      NVIC_DisableIRQ(UART_IRQn);
      if((UARTData[0] == 0x55u) && (UARTData[2] == 0xAAu))
      {
         val = UARTData[1];
      }
      Execute_Commands(val);
      NVIC_EnableIRQ(UART_IRQn);
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Execute_Commands(unsigned char val)
{
   unsigned int freq;
   switch(val)
   {
      case '^'://mute
              if(power_state == ON_STATE)
              {
                 Set_Vol(SI_VOL_MUTE);
                 UART_Printf("\n\rMute ON");
              }
              break;
              break;
      case ']'://power OFF
              if(power_state == ON_STATE)
              {
                 power_state = OFF_STATE;
              }
              break;
      case '[':	//power ON
              if(power_state == OFF_STATE)
              {
                 power_state = ON_STATE;
              }
              break;
      case '<'://volume down
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Volume_Operation(LEFT_TURN);
              }
              break;
      case '>'://volume up
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Volume_Operation(RIGHT_TURN);
              }
              break;
      case '+':	//right rotate in radio mode
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Tune_Operation(RIGHT_TURN);
              }
              break;
      case '-'://left rotate in radio mode
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Tune_Operation(LEFT_TURN);
              }
              break;
      case 'S'://store preset
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Tune_Button(LONG_PRESS);
              }
              break;
      case 'M'://mode change
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Tune_Button(SHORT_PRESS);
              }
              break;
      case 'R'://rds 
              if(power_state == ON_STATE)
              {
                 rds_mode = !rds_mode;
                 if(rds_mode == 1u)
                 {
                    UART_Printf("\n\rRDS ON");
                 }
                 else
                 {
                    UART_Printf("\n\rRDS OFF");
                 }
              }
              break;
       default :
              break;
   }    
}
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void UART_Printf(char *ptr)
{
   UARTSend((uint8_t *)ptr, strlen(ptr));
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Amp_Mute(unsigned char state)
{
   if(TRUE == state)
   {
      GPIOSetValue(PORT1,2 ,0);
   }
   else
   {
      GPIOSetValue(PORT1,2 ,1);
   }
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Enter_Sleep(void)
{
    /* Mute and save state */
    Amp_Mute(ON);
    Set_Vol(SI_VOL_MUTE);
    Change_Mode_EE(UPDATE_EE);   /* flush shadow RAM ? EEPROM */
    Process_Eeprom();

    /* Power down Si4703 - pull RST low */
    si4703_Rst(0);              /* macro in APP_Si.h: GPIOSetValue(PORT0,2,0) */

    /* 3. Stop Timer16_0 (the 1ms heartbeat) */
    NVIC_DisableIRQ(TIMER_16_0_IRQn);
    Disable_Timer16(0);

    /* Configure wake-up GPIO interrupt */
    Configure_WakeUp_GPIO();

    /* Enter Sleep mode — NOT Deep Sleep.
       SLEEPDEEP bit must be CLEAR for Sleep mode.
       UART IRQ will also wake the CPU.              */
    SCB->SCR &= ~(1u << 2u);   /* clear SLEEPDEEP — Sleep mode */
    __WFI();


    /* ---- CPU resumes here after button press ---- */
    /* Wake-up: re-init peripherals */
    System_WakeUp();
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void System_WakeUp(void)
{
    /* Disable wake-up interrupts now that we're awake */
    GPIOIntDisable(PORT0, 8);
    GPIOIntDisable(PORT0, 9);
    NVIC_DisableIRQ(EINT0_IRQn);

    /* Disable UART start logic */
    LPC_SYSCON->STARTERP1 &= ~(1u << 6u);
    
    Init_Buttons();

    /* Restart 1ms timer */
    NVIC_EnableIRQ(TIMER_16_0_IRQn);
    Enable_Timer16(0);

    Amp_Mute(ON);

    
    /* Power up Si4703 and restore last station/volume */
    Init_Radio();               /* your existing function in APP_Si.c */

}





