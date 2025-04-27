/*filename :- lcd.c*/
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/

#include "lcd.h"
#include "sh1106.h"
#include "App_Si.h"
#include "timer16.h"
#include <string.h>
#include <stdio.h>
#include "APP_Si_Fm_Table.h"
/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/

#define DISP_IDLE_TH 1u


/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
volatile lcd_T  lcd_state;
u32 disp_radio_counter;
screen_mode_T screen_mode;


extern const uint8_t For_OU_map[];

/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void Init_LCD(void)
{
   screen_mode = NO_SCREEN;
   sleep_check &= (~LCD_READY);
   lcd_state = LCD_INIT;
   SH1106_Init();
   SH1106_DrawBitmap(2,0,For_OU_map,128,64,1);
   SH1106_UpdateScreen();
   Delay_Ms(3000);
}
 
/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void Shut_LCD(void)
{
   lcd_state = LCD_INIT;
}



/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void LCD_Disp_Vol(void)
{
   if(screen_mode != VOL_SCREEN)
   {
      SH1106_Clear();
      screen_mode = VOL_SCREEN;
   }
   SH1106_GotoXY (50,10); 
   SH1106_Puts("VOL", &Font_11x18, (SH1106_COLOR_t)1);
   SH1106_GotoXY (54,30); 
   SH1106_Puts((char*)&vol_text[si_vol][0], &Font_11x18, (SH1106_COLOR_t)1);
   SH1106_UpdateScreen(); 
}
/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void LCD_Disp_Radio_Freq(u16 freq)
{
   unsigned char freq_offset=0;

   if(screen_mode != FREQ_SCREEN)
   {
      SH1106_Clear();
      screen_mode = FREQ_SCREEN;
   }
   if(radio_mode == FM_MODE)
   {
      freq_offset = (freq - FM_MIN_FREQ)/10u;
      SH1106_GotoXY (5,10); 
      SH1106_Puts("FM", &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_GotoXY (100,10); 
      if(fm_stereo)
      {
         SH1106_Puts("ST", &Font_11x18, (SH1106_COLOR_t)1);
      }
      else
      {
         SH1106_Puts("  ", &Font_11x18, (SH1106_COLOR_t)1);
      }
      SH1106_GotoXY (15,30); 
      SH1106_Puts((char*)&fm_text[freq_offset][0], &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_Puts(" Mhz", &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_GotoXY (5,50); 
      SH1106_Puts("         ", &Font_7x10, (SH1106_COLOR_t)1);
      SH1106_UpdateScreen(); 
   }
   else if(radio_mode == AM_MODE)
   {
      freq_offset = (freq - AM_MIN_FREQ)/9u;
      SH1106_GotoXY (5,10); 
      SH1106_Puts("AM", &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_GotoXY (20,30); 
      SH1106_Puts((char*)&am_text[freq_offset][0], &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_Puts(" Khz", &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_UpdateScreen(); 
   }
   else if(radio_mode == SW_MODE)
   {
      freq_offset = (freq - K_Sw_Min_Freq[sw_mode_cnt])/5u;
      sprintf((char *)&sw_text[0],"%d",sw_freq[sw_mode_cnt]);
      SH1106_GotoXY (5,10); 
      SH1106_Puts("SW   ", &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_Puts((char *)&sw_ch_text[sw_mode_cnt][0],&Font_11x18, (SH1106_COLOR_t)1);
      SH1106_Puts("m",&Font_11x18, (SH1106_COLOR_t)1);
      SH1106_GotoXY (20,30); 
      SH1106_Puts((char*)&sw_text[0], &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_Puts(" Khz", &Font_11x18, (SH1106_COLOR_t)1);
      SH1106_UpdateScreen(); 
   }
   else
   {
      //do nothing
   }
}


/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void LCD_Disp(const char *txt)
{
   if(screen_mode != INTRO_SCREEN)
   {
      SH1106_Clear();
      screen_mode = INTRO_SCREEN;
   }
   SH1106_GotoXY (12,10); 
   SH1106_Puts((char*)txt, &Font_7x10, (SH1106_COLOR_t)1);
   SH1106_UpdateScreen(); 
   Delay_Ms(1500);
   SH1106_Clear();
}  



/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void App_LCD_Periodic(void)
{
   static u16 disp_idle_counter;
   static si_mode_T prev_radio_mode;
   
   switch(lcd_state)
   {
      case LCD_INIT:
           if(Power_Mode == PWR_ON)
           {
              LCD_Disp((const char*)&on_text[0]);
              lcd_state = LCD_RUNNING; 
              LCD_Disp((const char*)&hi_text[0]);
              disp_radio_counter = DISP_RAD_TIMEOUT_LCD_REFRESH;
           }
           break;
   
      case LCD_RUNNING:
           if(update_vol)
           {
              LCD_Disp_Vol();
              //Delay_Ms(DISP_RAD_LCD_VOL_DLY);
              update_vol = 0;
              disp_radio_counter = DISP_RAD_TIMEOUT_VOL;
           }
           if(disp_radio_counter)
           {
              disp_radio_counter--;
              if(disp_radio_counter == 2u)
              {
                  rds_update=0;
              }
              else if(disp_radio_counter == 1u)
              {
                 if(radio_mode == FM_MODE)
                 {
                    if(prev_radio_mode != radio_mode)
                    {
                       SH1106_Clear();
                    }
                    LCD_Disp_Radio_Freq(fm_freq);
                    rds_update=0;
                    prev_radio_mode = radio_mode;
                 }
                 else if(radio_mode == AM_MODE)
                 {
                    if(prev_radio_mode != radio_mode)
                    {
                       SH1106_Clear();
                    }
                    LCD_Disp_Radio_Freq(am_freq);
                    LCD_Disp_Am_Sw_Info(Rssi,Snr,Bw);
                    prev_radio_mode = radio_mode;
                 }
                 else if(radio_mode == SW_MODE)
                 {
                    if(prev_radio_mode != radio_mode)
                    {
                       SH1106_Clear();
                    }
                    LCD_Disp_Radio_Freq(sw_freq[sw_mode_cnt]);
                    LCD_Disp_Am_Sw_Info(Rssi,Snr,Bw);
                    prev_radio_mode = radio_mode;
                 }
                 disp_radio_counter = 0;
                 disp_idle_counter=DISP_IDLE_TH;
              }
           }
           else
           {
              if(disp_idle_counter)
              {
                 disp_idle_counter--;
                 if(disp_idle_counter == 0u)
                 {
                    Set_UnMute();
                 }
              }
           }
           
           if((show_lcd_rds) && (rds_update) && (disp_radio_counter == 0u))
           {
              rds_update = 0;
              LCD_Disp_Rds(FALSE);
           }
           if(Power_Mode == PWR_OFF)
           {
              lcd_state = LCD_OFF_STATE;
           }
           
           break;

      case LCD_OFF_STATE:
           LCD_Disp((const char*)&off_text[0]);
           lcd_state = LCD_SHUT_STATE;
           disp_radio_counter = DISP_RAD_TIMEOUT_DEF;
           break;

      case LCD_SHUT_STATE:
           if(Power_Mode == PWR_OFF)
           {
              sleep_check |= (LCD_READY);
              Shut_LCD();
              lcd_state = LCD_INIT;
           }
           break;
      default:
           break;
   
   }
}


 
 
 
 
 
/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void LCD_Disp_Rds(u8 clear)
{
   SH1106_GotoXY (5,50); 
   if(clear)
   {
      memset(rdsdata,' ',8);
      SH1106_Puts((char*)rdsdata, &Font_7x10, (SH1106_COLOR_t)1);
      //SH1106_Puts(rdstext, &Font_7x10, (SH1106_COLOR_t)1);
   }
   else
   {
      SH1106_Puts((char*)rdsdata, &Font_7x10, (SH1106_COLOR_t)1);
      //SH1106_Puts(rdstext, &Font_7x10, (SH1106_COLOR_t)1);
   }
   SH1106_UpdateScreen(); 

}
/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void LCD_Disp_Clr_Rds(u8 state)
{
   SH1106_GotoXY (5,50); 
   SH1106_Puts("RDS", &Font_7x10, (SH1106_COLOR_t)1);
   SH1106_Puts(" ", &Font_7x10, (SH1106_COLOR_t)1);
   if(state)
   {
      SH1106_Puts("ON   ", &Font_7x10, (SH1106_COLOR_t)1);
   }
   else
   {
      SH1106_Puts("OFF  ", &Font_7x10, (SH1106_COLOR_t)1);
   }
   SH1106_UpdateScreen(); 

}    

/*****************************************************************************
** Function name :  
** Descriptions  :  
** Parameters    :  
** Returned value:  
*****************************************************************************/
void LCD_Disp_Am_Sw_Info(u8 rssi, u8 snr,u8 bw)
{

   /*
   volatile u8 snr_data[5] = {0,0,0,0,0};
   volatile u8 rssi_data[5] = {0,0,0,0,0};
	 */

   const u8 bw_data[7][4] = 
   {
      {"6.0"},
      {"4.0"},
      {"3.0"},
      {"2.0"},
      {"1.0"},
      {"1.8"},
      {"2.5"}    
   };

   /*
   sprintf((char *)&rssi_data[0],"%d",rssi);
   sprintf((char *)&snr_data[0],"%d",snr);
   */
   SH1106_GotoXY (5,50); 
   /*  Disabling Rssi and Snr due to unwanted noise heards in audio due to polling of I2C
   SH1106_Puts("P=", &Font_7x10, (SH1106_COLOR_t)1);
   SH1106_Puts((char *)&rssi_data[0], &Font_7x10, (SH1106_COLOR_t)1);
   SH1106_Puts("Snr=", &Font_7x10, (SH1106_COLOR_t)1);
   SH1106_Puts((char *)&snr_data[0], &Font_7x10, (SH1106_COLOR_t)1);
   */
   SH1106_Puts("Bw=", &Font_7x10, (SH1106_COLOR_t)1);
   SH1106_Puts((char *)&bw_data[bw][0],&Font_7x10, (SH1106_COLOR_t)1);
   SH1106_UpdateScreen(); 
}    

 








