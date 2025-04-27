/******************************************************************************
** Methode Electronics India, Bangalore
*******************************************************************************
** Project Name      : T1xx_SWP
** File Name         : APP_SER.h
** Description       : Header file for SER 
** Date              : 30, Nov, 2014
** Version           : 0.1
** Author            : 
******************************************************************************/

#ifndef APP_SER_H
#define APP_SER_H

/*****************************************************************************
System/Project Includes                            *
******************************************************************************/

#include "system.h" 
#include "APP_Si.h"

#define RDS_MINIMAL    1

#define MUTE_DLY                 50u
#define SW_MAX_CNT               13u

#define STATUS_POLL_CNT_AM_TH   (10u/10u)    //10msec task
#define STATUS_POLL_CNT_SW_TH   (10u/10u)   //10msec task
#define STATUS_POLL_CNT_FM_TH   (40u/10u)    //10msec task

#define XTAL_USED TRUE
//#define UART_USED TRUE
#define ALGO_ORIGINAL TRUE
#define SI_FM_DBG TRUE
#define MUTE_ON TRUE

#define MAX_SW_BANDS 13U


//#define HW_MUTE_NEEDED_HERE
#define MAX_AM_CH_FILTER    7u

#define SI_IC_ADDRESS (0x11u)
#define SI_SLAVE_ADDRESS (SI_IC_ADDRESS<<1)

#define CTS_MASK  (0x80u)
#define STC_MASK  (0x01u)
#define RDS_MASK  (0x04u)
#define RSQ_MASK  (0x08u)
#define ERR_MASK  (0x40u)


#define R 0x00u
#define W 0x01u


#define RDS_PS (0)
#define RDS_RT (2)


#define SEEK_UP 1u
#define SEEK_DWN 0u

#define SI_BUSY_DLY 1u

#define VOL_INIT 30u
#define MAX_SI_VOL 62u

#define Get_TuneUp_Button() skup
#define Get_TuneDwn_Button() skdwn

#define Mute_Amp(x) GPIOSetValue(PORT0,11 ,x)

#define testpin(x) GPIOSetValue(PORT0,8 ,x)

#define Si4735_Rst(x) GPIOSetValue(PORT0,7 ,x)
#define Si4735_Sen(x) GPIOSetValue(PORT0,1 ,x)

#define Toggle_Led(x) GPIOSetValue(PORT0,3 ,x)
typedef enum
{
  RADIO_OFF,
  RADIO_INIT,
  RADIO_RUNNING

}radio_T;


extern volatile radio_T radio_state;

typedef enum
{
   RADIO_INIT_STATE,
   RADIO_RUN_STATE,
   RADIO_SHUT_STATE

}si_state_T;

extern si_state_T si_state;

typedef struct
{
   u16 prop;
   u16 init_val;
   u16  *final_val;
}FM_Prop_Table_T;

typedef struct
{
   u16 prop;
   u16 init_val;
}AM_Prop_Table_T;

typedef struct
{
   u16 prop;
   u16 init_val;
}SW_Prop_Table_T;

extern u8 show_lcd_rds;
extern const FM_Prop_Table_T K_FM_Prop_Table[];
extern const AM_Prop_Table_T K_AM_Prop_Table[];
extern const SW_Prop_Table_T K_SW_Prop_Table[];

typedef enum
{
   FM_MODE,
   SW_MODE,
   AM_MODE
}si_mode_T;

typedef enum
{
   OFF_STATE,
   ON_STATE,
   NONE
}power_T;
extern si_mode_T radio_mode;
extern volatile power_T power_state;
extern u16 fm_freq,am_freq,sw_freq[MAX_SW_BANDS];
extern u16 prev_fm_freq,prev_am_freq,prev_sw_freq[MAX_SW_BANDS];
extern u8  fm_stereo,prev_fm_stereo;
extern u8 si_vol,si_vol_prev;
extern u8 update_vol;
extern u8 rdsdata[8];
extern u8 rdstext[64];
extern u8 KR_Cmd[];
extern u8 KR_Resp[];
extern u8 rds_update;
extern u8 Rssi;
extern u8 Snr;
extern u8 Bw;
extern u8 sw_mode_cnt;

extern const u16 K_Sw_Min_Freq[MAX_SW_BANDS];
extern const u16 K_Sw_Max_Freq[MAX_SW_BANDS];

/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/
extern void Set_Prop(u16 prop , u16 val);
extern void Init_Radio(void);
extern void Init_Si(si_mode_T si_mode);
extern void App_Si_Periodic(void);
extern void Set_UnMute(void);
extern void Set_Mute(void);
extern void Process_Fm(void);
extern void Process_Am(void);
extern void Process_Sw(void);
#if 1
extern u8 Process_RDS(void);
#else
extern u8 Process_RDS(u8* psPtr, u8*ptPtr);
#endif

#endif
