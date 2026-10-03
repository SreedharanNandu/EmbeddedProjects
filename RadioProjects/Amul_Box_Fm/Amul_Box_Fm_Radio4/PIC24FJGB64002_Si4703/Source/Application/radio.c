/*----------------------------------------------------------------------------
 * Name:   radio.c
 *----------------------------------------------------------------------------*/

/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/


/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/

#include "radio.h"   
#include "buttons.h"   
#include "i2c.h"
#include <stdio.h>
#include <string.h>
#include "io.h"
#include "timer.h"
#include "led.h"
#include "uart.h"
#include "rotary.h"
#include "types.h"
#include "hardware.h"
#include "memory.h"
#include "const.h"
#include "generic.h"
#include "process.h"
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
volatile si4703_read_reg_T si4703_read_reg;
volatile radio_T radio_state;
volatile unsigned char Is_Manual_Tune;
volatile unsigned char Is_Vol_Changed;
volatile unsigned char preset_mode;
unsigned short int fm_freq,prev_fm_freq;
unsigned char preset_index;
volatile unsigned char si_vol;
unsigned char fm_stereo;
unsigned short int K_Radio_Data_Read[MAX_CH];
unsigned short int K_Radio_Data_Write[MAX_CH];
unsigned short int reset_Vol_Timer;
unsigned short int reset_Index_Timer;
unsigned short int K_Radio_Index_Read[1];
unsigned short int K_Radio_Index_Write[1]; 
unsigned short int K_Radio_Vol_Read[1];
unsigned short int K_Radio_Vol_Write[1]; 
unsigned char first_time_power_up;
unsigned char prev_power_state;
unsigned char Scan_Cnt;


/*****************************************************************************
*                            Function declarations                          *
******************************************************************************/
static void Write_si4703(void);
void Power_Radio(unsigned char state);
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
static void Reset_CN_Wake(void);
static void Setup_CN_Wake(void);

extern void SwitchSystemClock(void);

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
   
   //SDI (SDA) must be LOW as per data sheet 
   _RB9 = 0; InsertDelay(100);
   Delay_Ms(200u);
   //si in reset state
   si4703_Rst(0); InsertDelay(100);
   //make RSEN High
   si4703_Rsen(1);InsertDelay(100);
   Delay_Ms(100u);
   //out of reset    
   si4703_Rst(1);InsertDelay(100);
   Delay_Ms(100u);
   //Reconfigure to I2C SDA pin
   _RB9 = 1;InsertDelay(100);
   Init_I2C();

   Delay_Ms(300u);//Reconfigure delay for I2C
   Power_Radio(1u);
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
  
   data_wr[0] = si4703_read_reg.powercfg>>8;
   data_wr[1] = (unsigned char)si4703_read_reg.powercfg;
   data_wr[2] = si4703_read_reg.channel>>8;
   data_wr[3] = (unsigned char)si4703_read_reg.channel;
   data_wr[4] = si4703_read_reg.sysconfig1>>8;
   data_wr[5] = (unsigned char)si4703_read_reg.sysconfig1;
   data_wr[6] = si4703_read_reg.sysconfig2>>8;
   data_wr[7] = (unsigned char)si4703_read_reg.sysconfig2;
   data_wr[8] = si4703_read_reg.sysconfig3>>8;
   data_wr[9] = (unsigned char)si4703_read_reg.sysconfig3;
   data_wr[10] = si4703_read_reg.test1>>8;
   data_wr[11] = (unsigned char)si4703_read_reg.test1;
   SendI2C1(SI_SLAVE_ADDRESS,(unsigned char*)&data_wr[0],12u);
   
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

    if(ReadI2C1(SI_SLAVE_ADDRESS,(unsigned char*)&data_rd[0],32u))
    {
        si4703_read_reg.statusrssi     = (data_rd[0]<<8)|data_rd[1]; 
        si4703_read_reg.readchan       = (data_rd[2]<<8)|data_rd[3]; 
        si4703_read_reg.rdsa           = (data_rd[4]<<8)|data_rd[5]; 
        si4703_read_reg.rdsb           = (data_rd[6]<<8)|data_rd[7]; 
        si4703_read_reg.rdsc           = (data_rd[8]<<8)|data_rd[9]; 
        si4703_read_reg.rdsd           = (data_rd[10]<<8)|data_rd[11]; 
        si4703_read_reg.device_id      = (data_rd[12]<<8)|data_rd[13]; 
        si4703_read_reg.chipid         = (data_rd[14]<<8)|data_rd[15];
        si4703_read_reg.powercfg       = (data_rd[16]<<8)|data_rd[17];
        si4703_read_reg.channel        = (data_rd[18]<<8)|data_rd[19];
        si4703_read_reg.sysconfig1     = (data_rd[20]<<8)|data_rd[21];
        si4703_read_reg.sysconfig2     = (data_rd[22]<<8)|data_rd[23];
        si4703_read_reg.sysconfig3     = (data_rd[24]<<8)|data_rd[25];
        si4703_read_reg.test1          = (data_rd[26]<<8)|data_rd[27];
        si4703_read_reg.test2          = (data_rd[28]<<8)|data_rd[29];
        si4703_read_reg.bootconfig     = (data_rd[30]<<8)|data_rd[31];
    }
    else
    {
       Amp_Mute(ON);
       radio_state = RADIO_RESET;
    }
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Power_Radio(unsigned char state)
{
   
   Read_si4703();
   if(si4703_read_reg.chipid != 0)//chip detected
   {
      if(state == 1u)
      {
         Delay_Ms(200u);//delay for XTAL stabilization
         si4703_read_reg.test1 = 0x8100u;
         Write_si4703();
         Delay_Ms(300u);//delay for XTAL stabilization
         si4703_read_reg.powercfg = 0x4001u;
         Write_si4703();
         si4703_read_reg.sysconfig1 = 0x1080u; 
         Write_si4703();
         si4703_read_reg.sysconfig2 &= ~(3<<4) ; 
         si4703_read_reg.sysconfig2 &= 0xFFF0u ; 
         si4703_read_reg.sysconfig2 |= 0x0015u ; //make very volume MUTE
         Write_si4703();
      }
      else 
      {
         si4703_read_reg.powercfg = 0x4041u;
         Write_si4703();
      }
   }
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
   si4703_read_reg.powercfg &= ~(1<<10);
   if(seek == SEEK_DWN) 
   {
      si4703_read_reg.powercfg &= ~(1<<9);
   }
   else
   { 
      si4703_read_reg.powercfg |= 1<<9; //Set the bit to seek up
   }
   si4703_read_reg.powercfg |= (1<<8); //Start seek
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
   if(si4703_read_reg.chipid != 0)
   {
      si4703_read_reg.channel &= 0xFE00u; //Clear out the channel bits
      si4703_read_reg.channel |= scale; //Mask in the new channel
      si4703_read_reg.channel |= 0x8000u; //Set the TUNE bit to start
      Write_si4703();      
      cnt  = SET_FM_TUNE_RSSI_THRESHOLD;
      while(cnt)
      {
        cnt -- ;
        Read_si4703();      
        if((si4703_read_reg.statusrssi & (1<<14)) != 0u)//Tune complete
        {
           break;
        }
      }
      Read_si4703();
      si4703_read_reg.channel  &= ~(1<<15); 
      Write_si4703();      
      cnt  = SET_FM_TUNE_RSSI_THRESHOLD;
      while(cnt)
      {
        cnt--;
        Read_si4703();      
        if((si4703_read_reg.statusrssi & (1<<14)) == 0u)
        {
           break;
        }
      }
   }
}



/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Vol_Operation(unsigned char dir)
{
   if(ON_STATE == power_state)
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         if(si_vol < SI_VOL_MAX)
         {
           if(si_vol == 0u)
           {
              Amp_Mute(OFF);
           }
           si_vol++;
         }             
         Is_Vol_Changed = 1;
         reset_Vol_Timer = RESET_VOL_TIMER_TH;
         UART_Printf(SEND_VOL);
      }
      else if(dir == LEFT_TURN)/*left*/
      {
         if(si_vol > SI_VOL_MUTE)
         {
           si_vol--;
           if(si_vol == 0u)
           {
              Amp_Mute(ON);
           }
         }             
         Is_Vol_Changed = 1;
         reset_Vol_Timer = RESET_VOL_TIMER_TH;
         UART_Printf(SEND_VOL);
      }
   }
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Tune_Operation(unsigned char dir)
{
   if(ON_STATE == power_state )
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         if(preset_mode)
         {
            if(preset_index < (MAX_CH-1U))
            {
               preset_index++;
               if(K_Radio_Data_Read[preset_index] <= FM_MAX_FREQ)
               {
                  fm_freq = K_Radio_Data_Read[preset_index];
                  Is_Manual_Tune = 1;
                  UART_Printf(SEND_CH);
                  UART_Printf(SEND_FREQ);
                  reset_Index_Timer = RESET_INDEX_TIMER_TH;
               }
            }
         }
         else
         {
            if(fm_freq < FM_MAX_FREQ)
            {
               fm_freq = fm_freq + 10;
               Is_Manual_Tune = 1;
               UART_Printf(SEND_FREQ);
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
               if(K_Radio_Data_Read[preset_index] >= FM_MIN_FREQ)
               {
                  fm_freq = K_Radio_Data_Read[preset_index];
                  Is_Manual_Tune = 1;
                  UART_Printf(SEND_CH);
                  UART_Printf(SEND_FREQ);
                  reset_Index_Timer = RESET_INDEX_TIMER_TH;
               }
            }
         }
         else
         {
            if(fm_freq > FM_MIN_FREQ)
            {
               fm_freq = fm_freq - 10;
               Is_Manual_Tune = 1;
               UART_Printf(SEND_FREQ);
            }
         }
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
            UART_Printf(SEND_PRESET);
            UART_Printf(SEND_CH);
            fm_freq = K_Radio_Data_Read[preset_index];
         }
         else
         {
            UART_Printf(SEND_MANUAL);
         }
         UART_Printf(SEND_FREQ);
         Is_Manual_Tune = 1;
      }
      else if(duration == LONG_PRESS)
      {
         K_Radio_Data_Write[preset_index] = fm_freq;
         K_Radio_Data_Read[preset_index] = K_Radio_Data_Write[preset_index];
         Change_Mode_EE(UPDATE_EE);
         preset_mode = 1u;
         UART_Printf(SEND_FREQ);
         UART_Printf(SEND_SAVED);
         UART_Printf(SEND_CH);
      }
   }
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Radio_Enc_Vol_Button(unsigned char duration)
{
   if(duration == SHORT_PRESS)
   {
      if(power_state == ON_STATE)
      {
         UART_Printf(SEND_VOL_SEL_ON);
      }
   }
   else if(duration == LONG_PRESS)
   {
      if(power_state == ON_STATE)
      {
         power_state = OFF_STATE;
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)500,(unsigned long)50,TURN_ON);
      }
      else if(power_state == OFF_STATE)
      {
         power_state = ON_STATE;
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)100,(unsigned long)50,TURN_ON);
      }
   }
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
   unsigned char i = 0;
   for(i=0;i<MAX_CH;i++)
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
   if(K_Radio_Index_Read[0] > MAX_CH)
   {
      K_Radio_Index_Read[0] = 0u;
      K_Radio_Index_Write[0] = 0u;
      preset_index = 0u;
   }
   else
   {
      preset_index = (unsigned char)K_Radio_Index_Read[0];
      K_Radio_Index_Write[0] = (unsigned short int)preset_index;
   }
   if(K_Radio_Vol_Read[0] > SI_MAX_VOL)
   {
      K_Radio_Vol_Read[0] = 0u;
      K_Radio_Vol_Write[0] = 0u;
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
   
   switch(radio_state)
   {
      case RADIO_RUNNING:
           if(power_state == OFF_STATE)
           {
              radio_state = RADIO_OFF;
              Amp_Mute(1u);
              Set_Vol(SI_VOL_MUTE);
              Set_GPIO2(0u);//Blue Led Power OFF
              UART_Printf(SEND_PWR_OFF);
           }
           else if(power_state == ON_STATE)
           {
              if(reset_Vol_Timer > 0u)
              {
                 reset_Vol_Timer--;
                 if(reset_Vol_Timer == 0u)
                 {
                    K_Radio_Vol_Write[0] = K_Radio_Vol_Read[0] = si_vol;
                    Change_Mode_EE(UPDATE_EE);
                 }
              }
              if(Is_Vol_Changed)
              {

                 Set_Vol(si_vol);
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
                 Scan_Cnt++;
                 if(Scan_Cnt == 10u)
                 {
                    Scan_Cnt = 0u;
                    Read_si4703();
                    if(si4703_read_reg.powercfg != 0x4001u)
                    {
                       Amp_Mute(ON);
                       radio_state = RADIO_RESET;
                    }
                    if(si4703_read_reg.statusrssi & 0x0100U)
                    {
                      fm_stereo = 1u;
                    }
                    else
                    {
                      fm_stereo = 0u;
                    }
                    if(reset_Index_Timer != 0u)
                    {
                       reset_Index_Timer--;
                       if(reset_Index_Timer == 0u)
                       {
                          K_Radio_Index_Write[0] = K_Radio_Index_Read[0] = preset_index;
                          Change_Mode_EE(UPDATE_EE);
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
              Amp_Mute(ON);
              if(App_Si_EE_Check_Pending)
              {
                 App_Si_EE_Check_Pending = 0u;
                 Validate_FM_Data();
              }
              if(!first_time_power_up)
              {
                 first_time_power_up = 1;
                 fm_freq = K_Radio_Data_Read[preset_index];
                 si_vol = K_Radio_Vol_Read[0];
              }
              Set_Fm_Tune_Freq(fm_freq);
              preset_mode = 0u;               
              radio_state = RADIO_RUNNING;
              Is_Vol_Changed = 1;
              Set_GPIO2(1);//Blue Led Power ON
              UART_Printf(SEND_FREQ);
              Amp_Mute(OFF);
           }
           break;   
      case RADIO_RESET:
           if(power_state == ON_STATE)
           {
              DeInit_I2C();
              Amp_Mute(ON);
              Init_Si();  // resets and re-powers Si4703
              radio_state = RADIO_INIT;
           }
           break;   
      default:
      case RADIO_OFF:
           if(power_state == ON_STATE)
           {
              radio_state = RADIO_INIT;
              UART_Printf(SEND_PWR_ON);
           }
           else
           {
              Enter_Deep_Sleep();
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
   if(rx_flag == 1u)
   {
      rx_flag = 0u;
      if((UARTData[0] == 0x55u) && (UARTData[2] == 0xAAu))
      {
         val = UARTData[1];
      }
      Execute_Commands(val);
      UARTData[0] = 0u;UARTData[1] = 0u;UARTData[2] = 0u;
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
   switch(val)
   {
      case '^'://mute
              if(power_state == ON_STATE)
              {
                 Set_Vol(SI_VOL_MUTE);
                 UART_Printf(SEND_MUTE);
              }
              break;
              break;
      case ']'://power OFF
              if(power_state == ON_STATE)
              {
                 
                 power_state = OFF_STATE;
                 Start_LED(LED_B,BLINK_ON,1,(unsigned long)500,(unsigned long)50,TURN_ON);
              }
              break;
      case '[':	//power ON
              if(power_state == OFF_STATE)
              {
                 
                 power_state = ON_STATE;
                 Start_LED(LED_B,BLINK_ON,1,(unsigned long)100,(unsigned long)50,TURN_ON);
              }
              break;
      case '<'://volume down
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Vol_Operation(LEFT_TURN);
              }
              break;
      case '>'://volume up
              if(power_state == ON_STATE)
              {
                 Radio_Enc_Vol_Operation(RIGHT_TURN);
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
void UART_Printf(Send_Uart_Type_T id)
{
   #if 0
   switch(id)
   {
      case SEND_WC:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Welcome_Text[0],12);
           break;
      case SEND_VOL:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Vol_Text[si_vol][7],7);
           break;
      case SEND_CH:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Ch_Text[preset_index][5],5);
           break;
      case SEND_SAVED:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Saved_Text[0],5);
           break;
      case SEND_PRESET:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Preset_Text[0],11);
           break;
      case SEND_MANUAL:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Manual_Text[0],11);
           break;
      case SEND_FREQ:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Freq_Text[(fm_freq-FM_MIN_FREQ)/10][8],8);
           break;
      case SEND_PWR_ON:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&PwrON_Text[0],8);
           break;
      case SEND_PWR_OFF:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&PwrOFF_Text[0],9);
           break;
      case SEND_MUTE:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Mute_Text[0],5);
           break;
      case SEND_VOL_SEL_ON:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&VolSelON_Text[0],10);
           break;
      case SEND_VOL_SEL_OFF:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&VolSelOFF_Text[0],11);
           break;
      case SEND_POR_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Por_Rst_Text[0],9);
           break;
      case SEND_MCLR_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Mclr_Rst_Text[0],9);
           break;
      case SEND_SOFT_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Soft_Rst_Text[0],9);
           break;
      case SEND_WDT_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Wdt_Rst_Text[0],9);
           break;
      case SEND_BOR_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Bor_Rst_Text[0],9);
           break;
      case SEND_CM_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Cm_Rst_Text[0],9);
           break;
      case SEND_TRAP_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Trap_Rst_Text[0],9);
           break;
      case SEND_ILLOP_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Illop_Rst_Text[0],9);
           break;
      case SEND_INV_RST:
           Send_UART_String("\r\n",2);
           Send_UART_String((const char*)&Inv_Rst_Text[0],9);
           break;
      default:
           break;
   }
   #endif
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Amp_Mute(unsigned char state)
{
   if(state == TRUE)
   {
      _RB13 = 0u;
   }
   else
   {
      _RB13 = 1u;
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Enter_Deep_Sleep(void)
{
    //Turn off LEDs
    LED_Off(LED_B);
    
    //Flush EEPROM writes if pending
    Process_Eeprom();            // existing call — ensure data is committed
    Change_Mode_EE(UPDATE_EE);

    //Mute amp and power down Si4703
    Amp_Mute(1u);
    Power_Radio(0u);
    
    //Stop Timer 1 so its ISR doesn't fire during sleep prep
    Turn_Off_Timer1();

    DeInit_I2C();

    //Drive SCL/SDA low to avoid I2C bus leakage during sleep
    _RB8 = 0u;
    _RB9 = 0u;

    //Arm CN wake source on RB3 (VOL_BTN / power button)
    Setup_CN_Wake();

    //Enter Sleep — CPU halts here until CN interrupt fires
    Sleep();        // expands to: asm("PWRSAV #0")


    // ---- CPU resumes here after CN ISR returns ----
    //Disarm CN
    Reset_CN_Wake();


    //Restore I2C lines
    _RB8 = 1u;
    _RB9 = 1u;

    //Re-start system clock (PLL was stopped in sleep)
    //The PIC24 restores from FRC on wake; switch back to PLL:
    SwitchSystemClock();
	Init_HWIO();
    Init_Buttons();

    //Re-init radio and set power state
    power_state = ON_STATE;
    Init_Si();  // resets and re-powers Si4703
    App_Si_EE_Check_Pending = 0u;

    _10msCounter  = 0u;
    _100msCounter = 0u;
    _1sCounter    = 0u;

    LED_On(LED_B);
    //Restart Timer 1
    Turn_On_Timer1();
    Enable_Timer1_Interrupt();
    Enable_UART_Interrupt();
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  : Setup CN7 (RB3 = VOL_BTN) as wake source.
                CN detects both edges; the ISR will re-check pin state if needed
*******************************************************************************/
static void Setup_CN_Wake(void)
{
    unsigned char dummy;
    dummy = PORTB;
    _CN7PUE = 1u;   // Enable pull-up on CN5/RB3 (button pulls low when pressed)
    _CN7IE  = 1u;   // Enable Change Notification on CN7
    _CNIE   = 1u;   // Enable CN interrupt globally
    _CNIP   = 7u;   // CN interrupt priority (must be > 0 to wake from sleep)
    _CNIF   = 0u;   // Clear CN flag
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
static void Reset_CN_Wake(void)
{
    unsigned char dummy;
    dummy = PORTB;
    _CN7IE  = 0u;
    _CNIE   = 0u;
    _CN7PUE = 0u;
    _CNIF   = 0u;
}
