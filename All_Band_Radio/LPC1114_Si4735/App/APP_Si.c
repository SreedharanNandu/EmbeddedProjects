/*filename :- App_Si.c*/
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include "LPC11xx.h"
#include "App_Si_Intf.h"
#include "i2c.h"
#include "App_Si.h"
#include "App_Btn.h"
#include "lcd.h"
#include "type.h"
#include <string.h>
#include "timer16.h"
#include "APP_Si_Fm_Table.h"
#include "APP_Si_Am_Table.h"
#include "EEPROM.h"
/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/

/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
u8 show_lcd_rds;
u8 rds_update;
u8 KR_Cmd[6];
u8 KR_Resp[13];
u8 Rssi;
u8 Snr;
u8 Bw;
u8 am_ch_filter_idx;
u8 sw_ch_filter_idx;
u8 sw_mode_cnt;
si_state_T si_state;

const FM_Prop_Table_T K_FM_Prop_Table[] = 
{
  // #if FM_UNUSED_CFG
   {0x1100,0x0001, /*DEEMPHASIS                       */},//moved to asia
   {0x1102,0x0000, /*CHANNEL_FILTER                   */},//Automatic Select
   {0x1105,0x0000, /*BLEND_STEREO_THRESHOLD           */},//forced stereo (not in Si4735)
   {0x1106,0x0000, /*BLEND_MONO_THRESHOLD             */},//forced stereo (not in Si4735)
   {0x1107,0x0000, /*ANTENNA_INPUT                    */},//FMI pin (not in Si4735)
   {0x1108,0x0014, /*MAX_TUNE_ERROR                   */},//default
   {0x1200,0x0000, /*RSQ_INT_SOURCE                   */},//default
   {0x1201,0x007F, /*RSQ_SNR_HI_THRESHOLD             */},//default
   {0x1202,0x0000, /*RSQ_SNR_LO_THRESHOLD             */},//default
   {0x1203,0x0000, /*RSQ_RSSI_HI_THRESHOLD            */},//changed to 0
   {0x1204,0x0000, /*RSQ_RSSI_LO_THRESHOLD            */},//default
   {0x1205,0x0000, /*RSQ_MULTIPATH_HI_THRESHOLD       */},//set to 0 default is 7F
   {0x1206,0x0000, /*RSQ_MULTIPATH_LO_THRESHOLD       */},//default
   {0x1207,0x0081, /*RSQ_BLEND_THRESHOLD              */},//default
   {0x1300,0x0000, /*SOFT_MUTE_RATE                   */},//not in Si4735
   {0x1301,0x0000, /*SOFT_MUTE_SLOPE                  */},//default is 2
   {0x1302,0x0010, /*SOFT_MUTE_MAX_ATTENUATION        */},//default
   {0x1303,0x0004, /*SOFT_MUTE_SNR_THRESHOLD          */},//default
   {0x1304,0x0000, /*SOFT_MUTE_RELEASE_RATE           */},//default is 0x2000
   {0x1305,0x0004, /*SOFT_MUTE_ATTACK_RATE            */},//default is 0x2000
   {0x1400,0x222E, /*SEEK_BAND_BOTTOM                 */},//default
   {0x1401,0x2A26, /*SEEK_BAND_TOP                    */},//default
   {0x1402,0x000A, /*SEEK_FREQ_SPACING                */},//default
   {0x1403,0x0003, /*SEEK_TUNE_SNR_THRESHOLD          */},//default
   {0x1404,0x0014, /*SEEK_TUNE_RSSI_TRESHOLD          */},//default
   //#endif
   {0x1500,0x0037, /*RDS_INT_SOURCE                   */},//default
   {0x1501,0x0001, /*RDS_INT_FIFO_COUNT               */},//changed to 1
   {0x1502,0xAA01, /*RDS_CONFIG                       */},//changed to AA01
   {0x1503,0x1111, /*RDS_CONFIDENCE                   */},//default
   {0x1800,0x0000, /*BLEND_RSSI_STEREO_THRESHOLD      */},//default is 0
   {0x1801,0x0000, /*BLEND_RSSI_MONO_THRESHOLD        */},//default is 0x001E
   {0x1802,0x0000, /*BLEND_RSSI_ATTACK_RATE           */},//default is 0x0FA0
   {0x1803,0x0000, /*BLEND_RSSI_RELEASE_RATE          */},//default is 0x0190
   {0x1804,0x0000, /*BLEND_SNR_STEREO_THRESHOLD       */},//default is 0x001B
   {0x1805,0x0000, /*BLEND_SNR_MONO_THRESHOLD         */},//default is 0x000E
   {0x1806,0x0000, /*BLEND_SNR_ATTACK_RATE            */},//default is 0x0FA0
   {0x1807,0x0000, /*BLEND_SNR_RELEASE_RATE           */},//default is 0x0190
   {0x1808,0x0064, /*BLEND_MULTIPATH_STEREO_THRESHOLD */},//default is 0x0014
   {0x1809,0x0064, /*BLEND_MULTIPATH_MONO_THRESHOLD   */},//default is 0x003C
   {0x180A,0x0000, /*BLEND_MULTIPATH_ATTACK_RATE      */},//default is 0x0FA0
   {0x180B,0x0000, /*BLEND_MULTIPATH_RELEASE_RATE     */},//default is 0x0028
   {0x1A00,0x0000, /*HICUT_SNR_HIGH_THRESHOLD         */},//default is 0x0018
   {0x1A01,0x0000, /*HICUT_SNR_LOW_THRESHOLD          */},//default is 0x000F
   {0x1A02,0x0000, /*HICUT_ATTACK_RATE                */},//default is 0x4E20
   {0x1A03,0x0000, /*HICUT_RELEASE_RATE               */},//default is 0x0014
   {0x1A04,0x0000, /*HICUT_MULTIPATH_TRIGGER_THRESHOLD*/},//default is 0x0014
   {0x1A05,0x0000, /*HICUT_MULTIPATH_END_THRESHOLD    */},//default is 0x003C
   {0x1A06,0x0000, /*HICUT_CUTOFF_FREQUENCY           */},//default
   {0,0}      
};




const AM_Prop_Table_T K_AM_Prop_Table[] = 
{
   {0x3100,0x0000,/*AM_DEEMPHASIS                       */},//default
   {0x3102,0x0004,/*AM_CHANNEL_FILTER                   */},
   {0x3103,0x7800,/*AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN*/},
   {0x3400,0x020A,/*AM_SEEK_BAND_BOTTOM                 */},//default
   {0x3401,0x06AE,/*AM_SEEK_BAND_TOP                    */},
   {0x3402,0x0009,/*AM_SEEK_FREQ_SPACING                */},
   {0,0}
};

const SW_Prop_Table_T K_SW_Prop_Table[] = 
{
   {0x3100,0x0000,/*AM_DEEMPHASIS                       */},//default
   {0x3102,0x0014,/*AM_CHANNEL_FILTER                   */},//default
   {0x3103,0x7800,/*AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN*/},//default
   {0x3400,SW_MIN_FREQ,/*AM_SEEK_BAND_BOTTOM            */},//default
   {0x3401,SW_MAX_FREQ,/*AM_SEEK_BAND_TOP               */},//default
   {0x3402,0x0005,/*AM_SEEK_FREQ_SPACING                */},//default
   {0,0}
};


const u16 am_sw_ch_filter[] = 
{
   0x0100,0x0101,0x0102,0x0103,0x0104,0x0105,0x0106
};

const u16 K_Sw_Min_Freq[MAX_SW_BANDS] = {2300u,3200u,3900u,4750u,5900u,7100u,9400u,11600u,13500u,15100u,17480u,18900u,21450u};
const u16 K_Sw_Max_Freq[MAX_SW_BANDS] = {2495u,3400u,4000u,5060u,6200u,7450u,9900u,12100u,13870u,15830u,17900u,19020u,21850u};


/*****************************************************************************
*                             Var Definitions                             
******************************************************************************/

u8 skup;
u8 skdwn;

u16 fm_freq,am_freq,sw_freq[MAX_SW_BANDS],
    prev_fm_freq,prev_am_freq,prev_sw_freq[MAX_SW_BANDS];

u8 fm_stereo,prev_fm_stereo;
si_mode_T radio_mode;
u8 update_vol;
u8 si_vol,si_vol_prev;






/*****************************************************************************
*                             Func Definitions                             
******************************************************************************/

static void Send_Si4735(u8 cmd_arg);
static void Power_Radio(void);
static void Get_Rev(void);
static void Set_Am_Tune_Freq(u16 freq);
static void Set_Fm_Tune_Freq(u16 freq);
static void Get_Fm_RDS_Status(void);
static void Cfg_AM(void);
static void Cfg_FM(void);
static void Cfg_SW(void);
static void Set_Vol(u8 vol_level);
static void Get_Fm_Tune_Status(void);
static void Get_Am_Tune_Status(void);
static void Get_Fm_Rsq_Status(void);
static void Get_Am_Rsq_Status(void);



/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Init_Si(si_mode_T si_mode)
{
   /*
   si4735 needs sen/reset and the i2c pins should not go LO during start up
   timer should be used to generate RCLK if XTAL is not used
   */

   //sen low
   Si4735_Sen(0);
   //si in reset state
   Si4735_Rst(0);
   Delay_Ms(SI_BUSY_DLY);
   //out of reset    
   Si4735_Rst(1);
   Delay_Ms(SI_BUSY_DLY);
   
   
   Power_Radio();
   Delay_Ms(500u);
   
   Get_Rev();
   
   if(si_mode == FM_MODE)
   {
      Cfg_FM();
   }
   else if(si_mode == AM_MODE)
   {
      Cfg_AM();
   }
   else if(si_mode == SW_MODE)
   {
        Cfg_SW();
   }
   Mute_Amp(FALSE);
   Delay_Ms(MUTE_DLY);
   Set_UnMute();
   Delay_Ms(MUTE_DLY);
   
   if(si_mode == FM_MODE)
   {
      Set_Fm_Tune_Freq(fm_freq);
   }
   else if (si_mode == AM_MODE)
   {
      Set_Am_Tune_Freq(am_freq);
   }
   else if (si_mode == SW_MODE)
   {
      Set_Am_Tune_Freq(sw_freq[sw_mode_cnt]);
   }
   
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Shutdown_Si(void)
{
   KR_Cmd[0] = POWER_DOWN;
   Send_Si4735(INDEX_POWER_DOWN);
   Delay_Ms(SI_BUSY_DLY);
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
static void Send_Si4735(u8 cmd_arg)
{
   u8 idx;
   Si4735_FM_Index_T cmd_fm;
   Si4735_AM_Index_T cmd_am;

   static u8 data_wr[7]={0,0,0,0,0,0,0},
             data_rd[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

   static u8 cmd_size,resp_size;

   
   
   if(radio_mode == FM_MODE)
   {
      cmd_fm = (Si4735_FM_Index_T)cmd_arg;
      if(Si4735_FM_Table[cmd_fm].cmd_size)
      {
         memcpy(data_wr,KR_Cmd,Si4735_FM_Table[cmd_fm].cmd_size);
         cmd_size = Si4735_FM_Table[cmd_fm].cmd_size;
      }
      else
      {
         cmd_size = 0u;
      }
      
      
      if(Si4735_FM_Table[cmd_fm].resp_size)
      {
         resp_size = Si4735_FM_Table[cmd_fm].resp_size;
      }
      else
      {
         resp_size = 0u;
      }
      Write_Read_Si4735_I2C(SI_SLAVE_ADDRESS,
                               &data_wr[0],
                               cmd_size,
                               &data_rd[0],
                               resp_size);
      if(resp_size)
      {
         memcpy(KR_Resp,&data_rd[0],Si4735_FM_Table[cmd_fm].resp_size);
      }
      else
      {
         for(idx=0;idx < Si4735_FM_Table[cmd_fm].resp_size;idx++)
         {
            KR_Resp[idx] = 0;
         }   
      }
   }
   else if((radio_mode == AM_MODE)||(radio_mode == SW_MODE))
   {
      cmd_am = (Si4735_AM_Index_T)cmd_arg;
      if(Si4735_AM_Table[cmd_am].cmd_size)
      {
         memcpy(data_wr,KR_Cmd,Si4735_AM_Table[cmd_am].cmd_size);
         cmd_size = Si4735_AM_Table[cmd_am].cmd_size;
      }
      else
      {
         cmd_size = 0u;
      }
      
      
      if(Si4735_AM_Table[cmd_am].resp_size)
      {
         resp_size = Si4735_AM_Table[cmd_am].resp_size;
      }
      else
      {
         resp_size = 0u;
      }
      Write_Read_Si4735_I2C(SI_SLAVE_ADDRESS,
                               &data_wr[0],
                               cmd_size,
                               &data_rd[0],
                               resp_size);
      if(resp_size)
      {
         memcpy(KR_Resp,data_rd,Si4735_AM_Table[cmd_am].resp_size);
      }
      else
      {
         for(idx=0;idx < Si4735_AM_Table[cmd_am].resp_size;idx++)
         {
            KR_Resp[idx] = 0;
         }   
      }
   
   }
   else 
   {
   }
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Power_Radio(void)
{
   if(radio_mode == FM_MODE)
   {
      KR_Cmd[0] = POWER_UP;
      #if defined(XTAL_USED)
        KR_Cmd[1] = (0x90);//cts interrupt en,gpo2 disabled osc enabled
      #else
        KR_Cmd[1] = (0x80);//cts interrupt en,gpo2 disabled osc disabled
      #endif
      KR_Cmd[2] = 0x05;
      Send_Si4735(INDEX_POWER_UP);
   }
   else if((radio_mode == AM_MODE)|| 
           (radio_mode == SW_MODE))
   {
      KR_Cmd[0] = POWER_UP;
      #if defined(XTAL_USED)
      KR_Cmd[1] = (0x91);//cts interrupt en,gpo2 disabled osc enabled
      #else
      KR_Cmd[1] = (0x81);//cts interrupt en,gpo2 disabled osc disabled
      #endif
      KR_Cmd[2] = 0x05;
      Send_Si4735(INDEX_POWER_UP);
   }
   else
   {
      /*do nothing*/
   }
   Delay_Ms(SI_BUSY_DLY);

   KR_Cmd[0] = GPIO_CTL;
   KR_Cmd[1] = 0x00;//gpo2 disabled
   Send_Si4735(INDEX_GPIO_CTL);
   Delay_Ms(10u);

   Set_Prop(GPO_IEN,0x0004);
   #if defined(XTAL_USED)
     Set_Prop(REFCLK_FREQ,0x8000);
   #else
     Set_Prop(REFCLK_FREQ,0x7A12);
   #endif

   Delay_Ms(200u);

   Set_Prop(REFCLK_PRESCALE,0x0001);
   if(radio_mode == FM_MODE)
   {
      //D60 bug fix 0x12 0x00 0xFF 0x00 0x00 0x00
      Set_Prop(0xFF00,0);
   }

}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Get_Rev(void)
{
   if(radio_mode == FM_MODE)
   {
     KR_Cmd[0] = GET_REV;
   }
   else if((radio_mode == AM_MODE)||(radio_mode == SW_MODE))
   {
     KR_Cmd[0] = GET_REV;
   }
   Send_Si4735(INDEX_GET_REV);
   Delay_Ms(SI_BUSY_DLY);
   
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Set_Prop(u16 prop , u16 val)
{
   if(radio_mode == FM_MODE)
   {
      KR_Cmd[0] = SET_PROPERTY;
      KR_Cmd[1] = 0;
      KR_Cmd[2] = (prop>>8);
      KR_Cmd[3] = (u8)prop;
      KR_Cmd[4] = val>>8;
      KR_Cmd[5] = (u8)val;
   }
   else if((radio_mode == AM_MODE)||(radio_mode == SW_MODE))
   {
      KR_Cmd[0] = SET_PROPERTY;
      KR_Cmd[1] = 0;
      KR_Cmd[2] = (prop>>8);
      KR_Cmd[3] = (u8)prop;
      KR_Cmd[4] = val>>8;
      KR_Cmd[5] = (u8)val;
   }
   else
   {
   
   }
   Send_Si4735(INDEX_SET_PROPERTY);
   Delay_Ms(SI_BUSY_DLY);

}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Cfg_FM(void)
{
   u8 idx=0;
   /*
   FM Receiver Mode
   There is a debug feature that remains active in Si4704/05/3x-D60 firmware which can create periodic noise in
   audio. Silicon Labs recommends you disable this feature by sending the following bytes (shown here in
   hexadecimal form):
   0x12 0x00 0xFF 0x00 0x00 0x00
   
   In Si4704/05/3x-D60 devices, the FM_BLEND_RSSI_STEREO_THRESHOLD property is no longer at address
   0x1105. Use address 0x1800 instead.
  
   In Si4704/05/3x-D60 devices, the FM_BLEND_RSSI_MONO_THRESHOLD property is no longer at address
   0x1106. Use address 0x1801 instead.
  
   To disable the SNR-based stereo blend, set both the FM_BLEND_SNR_STEREO_THRESHOLD property
   (0x1804) and the FM_BLEND_SNR_MONO_THRESHOLD property (0x1805) to 0.
  
   To disable the multipath-based stereo blend, set both the FM_BLEND_MULTIPATH_STEREO_THRESHOLD
   property (0x1808) and the FM_BLEND_MULTIPATH_MONO_THRESHOLD property (0x1809) to 100 (0x64).
   */

   while(K_FM_Prop_Table[idx].prop != 0)
   {
      //set the default values
      Set_Prop(K_FM_Prop_Table[idx].prop,K_FM_Prop_Table[idx].init_val);
      idx++;
   }

}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Cfg_AM(void)
{
   u8 idx=0;

   idx = 0;
   while(K_AM_Prop_Table[idx].prop != 0)
   {
      //set the default values
      Set_Prop(K_AM_Prop_Table[idx].prop,K_AM_Prop_Table[idx].init_val);
      idx++;
   }
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Cfg_SW(void)
{
   u8 idx=0;
   while(K_SW_Prop_Table[idx].prop != 0)
   {
      //set the default values
      Set_Prop(K_SW_Prop_Table[idx].prop,K_SW_Prop_Table[idx].init_val);
      idx++;
   }
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Set_Vol(u8 vol_level)
{
   Set_Prop(RX_VOLUME,(u16)vol_level);
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Set_UnMute(void)
{
   #if defined(MUTE_ON)
      Set_Prop(RX_HARD_MUTE,0);
   #endif
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Set_Mute(void)
{
   #if defined(MUTE_ON)
      Set_Prop(RX_HARD_MUTE,3);
   #endif
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Set_Am_Tune_Freq(u16 freq)
{
   KR_Cmd[0] = AM_TUNE_FREQ;
   KR_Cmd[1] = 0;
   KR_Cmd[2] = freq>>8;
   KR_Cmd[3] = (u8)freq;
   KR_Cmd[4] = 0;
   KR_Cmd[5] = (radio_mode == SW_MODE)? 1u : 0u;//tuning capacitor low byte
   Send_Si4735(INDEX_AM_TUNE_FREQ);
   Delay_Ms(SI_BUSY_DLY);
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Set_Fm_Tune_Freq(u16 freq)
{
   KR_Cmd[0] = FM_TUNE_FREQ;
   KR_Cmd[1] = 0;
   KR_Cmd[2] = freq>>8;
   KR_Cmd[3] = (u8)freq;
   KR_Cmd[4] = 0;
   Send_Si4735(INDEX_FM_TUNE_FREQ);
   Delay_Ms(SI_BUSY_DLY);
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Get_Am_Tune_Status(void)
{
   KR_Cmd[0] = AM_TUNE_STATUS;
   KR_Cmd[1] = 0x01;
   Send_Si4735(INDEX_AM_TUNE_STATUS);
   Rssi = KR_Resp[4];
   Snr = KR_Resp[5];
   Delay_Ms(SI_BUSY_DLY);
   
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
static void Get_Fm_Tune_Status(void)
{
   KR_Cmd[0] = FM_TUNE_STATUS;
   KR_Cmd[1] = 0x01;
   Send_Si4735(INDEX_FM_TUNE_STATUS);
   Delay_Ms(SI_BUSY_DLY);
   
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
static void Get_Fm_RDS_Status(void)
{
   KR_Cmd[0] = FM_RDS_STATUS;
   KR_Cmd[1] = 0x01;
   Send_Si4735(INDEX_FM_RDS_STATUS);
   Delay_Ms(SI_BUSY_DLY);
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Get_Fm_Rsq_Status(void)
{
   KR_Cmd[0] = FM_RSQ_STATUS;
   KR_Cmd[1] = 0x01;
   Send_Si4735(INDEX_FM_RSQ_STATUS);
   Delay_Ms(SI_BUSY_DLY);
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Get_Am_Rsq_Status(void)
{
   KR_Cmd[0] = AM_RSQ_STATUS;
   KR_Cmd[1] = 0x00;
   Send_Si4735(INDEX_AM_RSQ_STATUS);
   Delay_Ms(SI_BUSY_DLY);
}



/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void App_Si_Periodic(void)
{

   switch(si_state)
   {
      case RADIO_INIT_STATE:
           if(Power_Mode == PWR_ON)
           {
              sleep_check &= (~SI_READY);
              prev_fm_stereo = fm_stereo;
              si_state = RADIO_RUN_STATE;
              Init_Si(radio_mode);
              Set_Vol(si_vol);
           }
           break;
      case RADIO_RUN_STATE:
           if(key_pressed[INFO])
           {
              if(radio_mode == FM_MODE)
              {
                 if(key_pressed_type[INFO] <= SHORT_PRESS_TYPE)
                 {
                    show_lcd_rds = !show_lcd_rds;
                    LCD_Disp_Clr_Rds(show_lcd_rds);//rds on/off toggle
                 }
                 key_pressed[INFO] = FALSE;
              }
           }
           if(key_pressed[BAND])
           {
              if(radio_mode == FM_MODE)
              {
                 if(key_pressed_type[BAND] <= SHORT_PRESS_TYPE)
                 {
                    Mute_Amp(TRUE);
                    Delay_Ms(MUTE_DLY);
                    Set_Mute();
                    Delay_Ms(MUTE_DLY);
                    radio_mode = AM_MODE;
                    si_state = RADIO_INIT_STATE;
                    disp_radio_counter = DISP_RAD_TIMEOUT_BAND_CHANGE_LCD_REFRESH;
                    Reload_Nvm_Tmr();
                 }
              }
              else if(radio_mode == AM_MODE)
              {
                 if(key_pressed_type[BAND] <= SHORT_PRESS_TYPE)//am mode switch
                 {
                    Mute_Amp(TRUE);
                    Delay_Ms(MUTE_DLY);
                    Set_Mute();
                    Delay_Ms(MUTE_DLY);
                    radio_mode = SW_MODE;
                    si_state = RADIO_INIT_STATE;
                    disp_radio_counter = DISP_RAD_TIMEOUT_BAND_CHANGE_LCD_REFRESH;
                    Reload_Nvm_Tmr();
                 }
              }
              else if(radio_mode == SW_MODE)
              {
                 if(key_pressed_type[BAND] <= SHORT_PRESS_TYPE)//sw mode switch
                 {
                    Mute_Amp(TRUE);
                    Delay_Ms(MUTE_DLY);
                    Set_Mute();
                    Delay_Ms(MUTE_DLY);
                    sw_mode_cnt++;
                    if(sw_mode_cnt >= SW_MAX_CNT)
                    {
                       radio_mode = FM_MODE;
                       sw_mode_cnt = 0;
                    }
                    si_state = RADIO_INIT_STATE;
                    disp_radio_counter = DISP_RAD_TIMEOUT_BAND_CHANGE_LCD_REFRESH;
                    Reload_Nvm_Tmr();
                 }
              }
              key_pressed[BAND] = 0;
           }
           if(key_pressed[VOL_UP])
           {
              si_vol++;
              if(si_vol >=MAX_SI_VOL)
              {
                si_vol = MAX_SI_VOL;
              }
              if(si_vol != si_vol_prev)
              {
                 #if defined(HW_MUTE_NEEDED_HERE)
                 Set_Mute();
                 #endif
                 Set_Vol(si_vol);
                 si_vol_prev = si_vol;
              }
              update_vol = 1;
              Reload_Nvm_Tmr();
              key_pressed[VOL_UP] = 0;
           }
           else if(key_pressed[VOL_DWN])
           {
              if(si_vol > 0u)
              {
                 si_vol--;
              }
              if(si_vol != si_vol_prev)
              {
                 #if defined(HW_MUTE_NEEDED_HERE)
                 Set_Mute();
                 #endif
                 Set_Vol(si_vol);
                 si_vol_prev = si_vol;
              }
              update_vol = 1;
              Reload_Nvm_Tmr();
              key_pressed[VOL_DWN] = 0;
           }
           else//poll status
           {
              if(Get_Nvm_Tmr())
              {
                 Dec_Nvm_Tmr();
                 if(Is_Nvm_Tmr_Expired())
                 {
                    Update_Nvm_Radio(WRITE_DATA);
                 }
              }
              if(radio_mode == FM_MODE)
              {
                 Process_Fm();
              }
              else if(radio_mode == AM_MODE)
              {
                 Process_Am();
              }
              else if(radio_mode == SW_MODE)
              {
                 Process_Sw();
              }
           }
           break;
      case RADIO_SHUT_STATE:
           if(Power_Mode == PWR_OFF)
           {
              Mute_Amp(TRUE);
              Shutdown_Si();
              sleep_check |= (SI_READY);
              si_state = RADIO_INIT_STATE;
           }
           break;
   }
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Process_Fm(void)
{
   static u8 status_poll_cnt;
   if(key_pressed[TUNE_UP])
   {
      if(fm_freq < FM_MAX_FREQ)
      {
         fm_freq = fm_freq + 10;
      }
      else if (fm_freq > FM_MAX_FREQ)
      {
         fm_freq = FM_MIN_FREQ;
      }
      else
      {
      
      }
      if(prev_fm_freq !=  fm_freq)
      {
         rds_update=0;
         #if defined(HW_MUTE_NEEDED_HERE)
         Set_Mute();
         #endif
         Set_Fm_Tune_Freq(fm_freq);
         Get_Fm_Tune_Status();
         prev_fm_freq = fm_freq;
         disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
         LCD_Disp_Rds(TRUE);
         Reload_Nvm_Tmr();
      }   
      key_pressed[TUNE_UP] = 0;
   }
   else if(key_pressed[TUNE_DWN])
   {
      if(fm_freq > FM_MIN_FREQ)
      {
         fm_freq = fm_freq - 10;
      }
      else if (fm_freq < FM_MIN_FREQ)
      {
         fm_freq = FM_MAX_FREQ;
      }
      else
      {
      
      }
      if(prev_fm_freq != fm_freq)
      {
         rds_update=0;
         #if defined(HW_MUTE_NEEDED_HERE)
         Set_Mute();
         #endif
         Set_Fm_Tune_Freq(fm_freq);
         prev_fm_freq = fm_freq;
         disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
         Reload_Nvm_Tmr();
      }   
      key_pressed[TUNE_DWN] = 0;
   }
   else
   {
      status_poll_cnt++;
      if(status_poll_cnt > STATUS_POLL_CNT_FM_TH)
      {
         status_poll_cnt = 0;
         Get_Fm_Rsq_Status();
         fm_stereo = (KR_Resp[3] & 0x80u)?1:0;
         if(prev_fm_stereo != fm_stereo)
         {
            disp_radio_counter = DISP_RAD_TIMEOUT_ST_REFRESH;
            prev_fm_stereo = fm_stereo;
         }
         if(show_lcd_rds)//show rds in display
         {
            Get_Fm_RDS_Status();
            #if RDS_MINIMAL
            if(Process_RDS())
            #else
            if(Process_RDS(rdsdata,rdstext))
            #endif
            {
               rds_update = 1;
            }
         }
      }
   }
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Process_Am(void)
{
   static u8 status_poll_cnt;
   if(key_pressed[TUNE_UP])
   {
      if(am_freq < AM_MAX_FREQ)
      {
         am_freq = am_freq + 9u;
      }
      else if (am_freq > AM_MAX_FREQ)
      {
         am_freq = AM_MIN_FREQ;
      }
      else
      {
      
      }
      if(prev_am_freq != am_freq)
      {
         #if defined(HW_MUTE_NEEDED_HERE)
         Set_Mute();
         #endif
         Set_Am_Tune_Freq(am_freq);
         Get_Am_Tune_Status();
         Get_Am_Rsq_Status();
         prev_am_freq = am_freq;
         disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
         Reload_Nvm_Tmr();
      }   
      key_pressed[TUNE_UP] = 0;
   }
   else if(key_pressed[TUNE_DWN])
   {
      if(am_freq > AM_MIN_FREQ)
      {
         am_freq = am_freq - 9u;
      }
      else if (am_freq < AM_MIN_FREQ)
      {
         am_freq = AM_MAX_FREQ;
      }
      else
      {
      
      }
      if(prev_am_freq != am_freq)
      {
         #if defined(HW_MUTE_NEEDED_HERE)
         Set_Mute();
         #endif
         Set_Am_Tune_Freq(am_freq);
         prev_am_freq = am_freq;
         disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
         Reload_Nvm_Tmr();
      }   
      key_pressed[TUNE_DWN] = 0;
   }
   else if(key_pressed[INFO])
   {
      //change the bandwidth
      am_ch_filter_idx++;
      if(am_ch_filter_idx >= MAX_AM_CH_FILTER)
      {  
         am_ch_filter_idx = 0;
      }
      Bw = am_ch_filter_idx;
      disp_radio_counter = DISP_RAD_TIMEOUT_BAND_CHANGE_LCD_REFRESH;
      Set_Prop(K_AM_Prop_Table[1].prop,am_sw_ch_filter[am_ch_filter_idx]);
      key_pressed[INFO] = FALSE;
   }
   else
   {
      status_poll_cnt++;
      if(status_poll_cnt > STATUS_POLL_CNT_SW_TH)
      {
         status_poll_cnt = 0;
      }
   }

}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Process_Sw(void)
{
   static u8 status_poll_cnt;

   if(key_pressed[TUNE_UP])
   {
      if(sw_freq[sw_mode_cnt] < K_Sw_Max_Freq[sw_mode_cnt])
      {
         sw_freq[sw_mode_cnt] = sw_freq[sw_mode_cnt] + 5;
      }
      else if (sw_freq[sw_mode_cnt] > K_Sw_Max_Freq[sw_mode_cnt])
      {
         sw_freq[sw_mode_cnt] = K_Sw_Min_Freq[sw_mode_cnt];
      }
      else
      {
      
      }
      if(prev_sw_freq[sw_mode_cnt] != sw_freq[sw_mode_cnt])
      {
         #if defined(HW_MUTE_NEEDED_HERE)
         Set_Mute();
         #endif
         Set_Am_Tune_Freq(sw_freq[sw_mode_cnt]);
         Get_Am_Tune_Status();
         Get_Am_Rsq_Status();
         prev_sw_freq[sw_mode_cnt] = sw_freq[sw_mode_cnt];
         disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
         Reload_Nvm_Tmr();
      }   
      key_pressed[TUNE_UP] = 0;
   }
   else if(key_pressed[TUNE_DWN])
   {
      if(sw_freq[sw_mode_cnt] > K_Sw_Min_Freq[sw_mode_cnt])
      {
         sw_freq[sw_mode_cnt] = sw_freq[sw_mode_cnt] - 5;
      }
      else if (sw_freq[sw_mode_cnt] < K_Sw_Min_Freq[sw_mode_cnt])
      {
         sw_freq[sw_mode_cnt] = K_Sw_Max_Freq[sw_mode_cnt];
      }
      else
      {
      
      }
      if(prev_sw_freq[sw_mode_cnt] !=  sw_freq[sw_mode_cnt])
      {
         #if defined(HW_MUTE_NEEDED_HERE)
         Set_Mute();
         #endif
         Set_Am_Tune_Freq(sw_freq[sw_mode_cnt]);
         Get_Am_Tune_Status();
         prev_sw_freq[sw_mode_cnt] = sw_freq[sw_mode_cnt];
         disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
         Reload_Nvm_Tmr();
      }   
      key_pressed[TUNE_DWN] = 0;
   }
   else if(key_pressed[INFO])
   {
      //change the bandwidth
      sw_ch_filter_idx++;
      if(sw_ch_filter_idx >= MAX_AM_CH_FILTER)
      {  
         sw_ch_filter_idx = 0;
      }
      Bw = sw_ch_filter_idx;
      disp_radio_counter = DISP_RAD_TIMEOUT_BAND_CHANGE_LCD_REFRESH;
      Set_Prop(K_SW_Prop_Table[1].prop,am_sw_ch_filter[sw_ch_filter_idx]);
      key_pressed[INFO] = FALSE;
   }
   else
   {
      status_poll_cnt++;
      if(status_poll_cnt > STATUS_POLL_CNT_SW_TH)
      {
         status_poll_cnt = 0;
      }
   }

}









