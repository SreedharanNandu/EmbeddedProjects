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
unsigned char volume_mode;
volatile si4703_read_reg_T si4703_read_reg;
volatile radio_T radio_state;
volatile unsigned char Is_Manual_Tune;
static volatile unsigned char Is_Vol_Changed;
volatile unsigned char preset_mode;
unsigned short int fm_freq,prev_fm_freq;
unsigned char preset_index;
volatile unsigned char si_vol = DEF_SI_VOL;
unsigned char fm_stereo;
unsigned short int K_Radio_Data_Read[MAX_CH+1u] = {FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,0u};
unsigned short int K_Radio_Data_Write[MAX_CH+1u] = {FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,FM_MIN_FREQ,0u};
unsigned short int reset_Vol_Timer;

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
   Power_Radio();
   Is_Manual_Tune = 0;
   power_state = ON_STATE;
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

    ReadI2C1(SI_SLAVE_ADDRESS,(unsigned char*)&data_rd[0],32u);

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


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void Power_Radio(void)
{
   Read_si4703();
   if(si4703_read_reg.chipid != 0)//chip detected
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
void Radio_Enc_Operation(unsigned char dir)
{
   if((ON_STATE == power_state ))
   {
      if(dir == RIGHT_TURN)/*right turn*/
      { 
         if(volume_mode)
         {
            if(si_vol < SI_VOL_MAX)
            {
              si_vol++;
            }             
            Is_Vol_Changed = 1;
            reset_Vol_Timer = RESET_VOL_TIMER_TH;
            UART_Printf(SEND_VOL);
         }
         else
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
                     Start_LED(LED_B,BLINK_ON,1,(unsigned long)50,(unsigned long)50,TURN_ON,1);
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
      }
      else if(dir == LEFT_TURN)/*left*/
      {
         if(volume_mode)
         {
            if(si_vol > SI_VOL_MUTE)
            {
              si_vol--;
            }             
            Is_Vol_Changed = 1;
            reset_Vol_Timer = RESET_VOL_TIMER_TH;
            UART_Printf(SEND_VOL);
         }
         else
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
                     Start_LED(LED_B,BLINK_ON,1,(unsigned long)50,(unsigned long)50,TURN_ON,1);
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
void Radio_Enc_Button(unsigned char duration)
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
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)50,(unsigned long)50,TURN_ON,1);
      }
      else if(duration == LONG_PRESS)
      {
         K_Radio_Data_Write[preset_index] = fm_freq;
         K_Radio_Data_Read[preset_index] = K_Radio_Data_Write[preset_index];
         ee_state = UPDATE_EE;
         preset_mode = 1u;
         UART_Printf(SEND_FREQ);
         UART_Printf(SEND_SAVED);
         UART_Printf(SEND_CH);
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)500,(unsigned long)10,TURN_ON,1);
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
void Radio_Touch_Button(unsigned char duration)
{
   if(duration == SHORT_PRESS)
   {
      if(power_state == ON_STATE)
      {
         volume_mode = !volume_mode;
         if(volume_mode)
         {
            reset_Vol_Timer = RESET_VOL_TIMER_TH;
            UART_Printf(SEND_VOL_SEL_ON);
         }
         else
         {
            UART_Printf(SEND_VOL_SEL_OFF);
         }
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)250,(unsigned long)250,TURN_ON,1);
      }
   }
   else if(duration == LONG_PRESS)
   {
      volume_mode = 0;
      preset_mode = 0;
      if(power_state == ON_STATE)
      {
         power_state = OFF_STATE;
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)500,(unsigned long)50,TURN_ON,1);
      }
      else if(power_state == OFF_STATE)
      {
         power_state = ON_STATE;
         Start_LED(LED_B,BLINK_ON,1,(unsigned long)100,(unsigned long)50,TURN_ON,1);
      }
      else
      {
      }
   }
   else
   {
   
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
   if(K_Radio_Data_Read[MAX_CH] > (MAX_CH-1u))
   {
      K_Radio_Data_Read[MAX_CH] = 0u;
      K_Radio_Data_Write[MAX_CH] = 0u;
      preset_index = 0u;
   }
   else
   {
      preset_index = (unsigned char)K_Radio_Data_Read[MAX_CH];
      K_Radio_Data_Write[MAX_CH] = (unsigned short int)preset_index;
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
   static unsigned char first_time_power_up;
   
   switch(radio_state)
   {
      case RADIO_RUNNING:
           if(power_state == OFF_STATE)
           {
              radio_state = RADIO_OFF;
              Set_Vol(SI_VOL_MUTE);
              Set_GPIO2(0);//Blue Led Power OFF
              UART_Printf(SEND_PWR_OFF);
           }
           else if(power_state == ON_STATE)
           {
              if(volume_mode)
              {
                 if(reset_Vol_Timer > 0u)
                 {
                    reset_Vol_Timer--;
                 }
                 if(!reset_Vol_Timer)
                 {
                    volume_mode = 0u;
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
                 Is_Manual_Tune = 0;
                 fm_stereo = 0;
              }
              else
              {
                 Scan_Cnt++;
                 if(Scan_Cnt == 10u)
                 {
                    Scan_Cnt = 0u;
                    Read_si4703();
                    if(Get_Fm_Tune_Status() & 0x0100U)
                    {
                      fm_stereo = 1;
                    }
                    else
                    {
                      fm_stereo = 0;
                    }
                    if(prev_fm_stereo != fm_stereo)
                    {
                       disp_st_timer = 4;
                       prev_fm_stereo = fm_stereo;
                    }
                    if(disp_st_timer != 0u)
                    {
                       disp_st_timer--;
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
              si_vol = SI_VOL_SET;
              if(!first_time_power_up)
              {
                 first_time_power_up = 1;
                 fm_freq = K_Radio_Data_Read[preset_index];
              }
              Set_Fm_Tune_Freq(fm_freq);
              preset_mode = 0u;               
              radio_state = RADIO_RUNNING;
              Is_Vol_Changed = 1;
              Set_GPIO2(1);//Blue Led Power ON
              UART_Printf(SEND_FREQ);
           }
           break;   
      default:
      case RADIO_OFF:
           if(power_state == ON_STATE)
           {
              radio_state = RADIO_INIT;
              UART_Printf(SEND_PWR_ON);
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
                 volume_mode = 0u;
                 power_state = OFF_STATE;
                 Start_LED(LED_B,BLINK_ON,1,(unsigned long)500,(unsigned long)50,TURN_ON,1);
              }
              break;
      case '[':	//power ON
              if(power_state == OFF_STATE)
              {
                 volume_mode = 0u;
                 power_state = ON_STATE;
                 Start_LED(LED_B,BLINK_ON,1,(unsigned long)100,(unsigned long)50,TURN_ON,1);
              }
              break;
      case '<'://volume down
              if(power_state == ON_STATE)
              {
                 volume_mode = 1u;
                 Radio_Enc_Operation(LEFT_TURN);
              }
              break;
      case '>'://volume up
              if(power_state == ON_STATE)
              {
                 volume_mode = 1u;
                 Radio_Enc_Operation(RIGHT_TURN);
              }
              break;
      case '+':	//right rotate in radio mode
              if(power_state == ON_STATE)
              {
                 volume_mode = 0u;
                 Radio_Enc_Operation(RIGHT_TURN);
              }
              break;
      case '-'://left rotate in radio mode
              if(power_state == ON_STATE)
              {
                 volume_mode = 0u;
                 Radio_Enc_Operation(LEFT_TURN);
              }
              break;
      case 'S'://store preset
              if(power_state == ON_STATE)
              {
                 volume_mode = 0u;
                 Radio_Enc_Button(LONG_PRESS);
              }
              break;
      case 'M'://mode change
              if(power_state == ON_STATE)
              {
                 volume_mode = 0u;
                 Radio_Enc_Button(SHORT_PRESS);
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
  
}






