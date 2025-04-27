/*filename :- lcd.h*/
#ifndef LCD_H
#define LCD_H

/*****************************************************************************
System/Project Includes                            *
******************************************************************************/

#include "System.h" 
#include "gpio.h"


#define LCD_Width()  128u
#define LCD_Height()  64u

#define DISP_RAD_TIMEOUT_DEF                      250u//(500u)
#define DISP_RAD_TIMEOUT_VOL                      250u//10u
#define DISP_RAD_TIMEOUT_LCD_REFRESH             (2u)
#define DISP_RAD_TIMEOUT_BAND_CHANGE_LCD_REFRESH (2u)
#define DISP_RAD_TIMEOUT_ST_REFRESH              (5u)
#define DISP_RAD_LCD_VOL_DLY                      50u

#define __enable_irq()               { __ASM volatile ("cpsie i"); }
#define __disable_irq()              { __ASM volatile ("cpsid i"); }



#define FM_MIN_FREQ 8750u	   
#define FM_MAX_FREQ 10850u

#define AM_MIN_FREQ 522u
#define AM_MAX_FREQ 1710u

#define SW_MIN_FREQ 2300u
#define SW_MAX_FREQ 21850u

// allowed chars in RDS
#define FIRST_ALLOWED_CHAR (0x20)
#define LAST_ALLOWED_CHAR (0x7f)

extern u32 disp_radio_counter;



typedef enum
{
   LCD_INIT,
   LCD_RUNNING,
   LCD_SHUT_STATE,
   LCD_OFF_STATE
}lcd_T;


extern volatile lcd_T lcd_state;

typedef enum
{
   NO_SCREEN=0,
   VOL_SCREEN,
   FREQ_SCREEN,
   INTRO_SCREEN

}screen_mode_T;



extern screen_mode_T screen_mode;


extern const unsigned char fm_text[][7];
extern const unsigned char am_text[][7];
extern unsigned char sw_text[7];
extern const unsigned char hi_text[];
extern const unsigned char on_text[];
extern const unsigned char off_text[];
extern const unsigned char ch_text[10];
extern const unsigned char vol_text[64][3];
extern const unsigned char sw_ch_text[13][4]; 
 
/************************************
         Fuction Declaration
*************************************/
extern void Init_LCD(void);
extern void Shut_LCD(void);
extern void App_LCD_Periodic(void);
extern void LCD_Disp_Radio_Freq(u16 freq);
extern void LCD_Disp_Radio_St(u8 st);
extern void LCD_Disp_Radio_Sw_Band(void);
extern void LCD_Disp_Rds(u8 clear);
extern void LCD_Disp_Vol(void);
extern void LCD_Disp_Clr_Rds(u8 state);
extern void LCD_Disp_Am_Sw_Info(u8 rssi, u8 snr , u8 bw);



#endif




