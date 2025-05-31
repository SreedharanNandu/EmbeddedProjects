/******************************************************************************
** filename - App_Si.h
*******************************************************************************
** Project Name      : 
** File Name         : 
** Description       :  
** Date              : 
** Version           : 
** Author            : 
******************************************************************************/

#ifndef APP_SI_H
#define APP_SI_H

/*****************************************************************************
*                         System/Project Includes                            *
******************************************************************************/



/*****************************************************************************
*                             Macro Definition                               *
******************************************************************************/
#define RADIO_ENABLED                1U
#define SET_FM_TUNE_RSSI_THRESHOLD   5536u
#define OFF_STATE                    0u
#define ON_STATE                     1u
#define LEFT_TURN                    0u 
#define RIGHT_TURN                   1u

// RDS specific stuff
#define RDS_PS (0)
#define RDS_RT (2)

#if TEST
 #define SI_VOL_INIT    0x05 /*else keep at 6 for Si IC check*/
 #define SI_VOL_MUTE    0x00
#else
 #define SI_VOL_MAX     0x0Fu
 #define SI_VOL_MIN     0x00u
 #define SI_VOL_MUTE    SI_VOL_MIN
 #define SI_VOL_SET     0x05u
#endif

#define RESET_VOL_TIMER_TH    100u


#define SI_IC_ADDRESS (0x10u)
#define SI_SLAVE_ADDRESS (SI_IC_ADDRESS<<1)

#define R 0x00u
#define W 0x01u

#define SEEK_UP            1u
#define SEEK_DWN           0u

#define FM_MAX_FREQ    10800u
#define FM_MIN_FREQ    8750u
#define FM_DEF_FREQ    9350u

#define DEF_SI_VOL     1u
#define SI_MIN_VOL     0u
#define SI_MAX_VOL     15u
#define MAX_CH         10u
#define MAX_CH_SIZE    20u

#define PRINTBUFSIZE   100u


#define Set_GPIO1_Val(x)  ((x) ?  3u : 2u)
#define Set_GPIO2_Val(x)  ((x) ? 12u : 8u)


#define si4703_Rst(x)   GPIOSetValue(PORT0,2 ,x);
#define si4703_Rsen(x)  GPIOSetValue(PORT0,1 ,x);

#define Get_Sound_Status() 1//temporary as of now till gpio is available

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


#define UART_Sprintf(x,y) sprintf(printBuffer,x,y); \
                          UART_Printf(printBuffer); 


typedef enum
{
   INDEX_DEVICE_ID                         ,
   INDEX_CHIPID                            ,
   INDEX_POWERCFG                          ,
   INDEX_CHANNEL                           ,
   INDEX_SYSCONFIG1                        ,
   INDEX_SYSCONFIG2                        ,
   INDEX_SYSCONFIG3                        ,
   INDEX_TEST1                             ,
   INDEX_TEST2                             ,
   INDEX_BOOTCONFIG                        ,
   INDEX_STATUSRSSI                        ,
   INDEX_READCHAN                          ,
   INDEX_RDSA                              ,
   INDEX_RDSB                              ,
   INDEX_RDSC                              ,
   INDEX_RDSD
}si4703_T;

typedef enum
{
   INDEX_FM_DETECTOR_SNR               ,
   INDEX_BLEND_MONO_RSSI               ,
   INDEX_BLEND_STEREO_RSSI             ,
   INDEX_CALCODE                       ,
   INDEX_SNRDB                         
}si4703_Prop_Index_T;

typedef enum
{
   DEVICE_ID      ,
   CHIPID         ,
   POWERCFG       ,
   CHANNEL        ,
   SYSCONFIG1     ,
   SYSCONFIG2     ,
   SYSCONFIG3     ,
   TEST1          ,
   TEST2          ,
   BOOTCONFIG     ,
   STATUSRSSI     ,
   READCHAN       ,
   RDSA           ,
   RDSB           ,
   RDSC           ,
   RDSD           ,
}si4703_Cmd_T;


typedef enum
{
   FM_DETECTOR_SNR                 = 0x0200,
   BLEND_MONO_RSSI                 = 0x0300,
   BLEND_STEREO_RSSI               = 0x0301,
   CALCODE                         = 0x0700,
   SNRDB                           = 0x0C00
              
}si4703_Prop_T;


typedef enum
{
   CMD_SIZE_DEVICE_ID              = 3,
   CMD_SIZE_CHIPID                 = 3,
   CMD_SIZE_POWERCFG               = 3,
   CMD_SIZE_CHANNEL                = 3,
   CMD_SIZE_SYSCONFIG1             = 3,
   CMD_SIZE_SYSCONFIG2             = 3,
   CMD_SIZE_SYSCONFIG3             = 3,
   CMD_SIZE_TEST1                  = 3,
   CMD_SIZE_TEST2                  = 3,
   CMD_SIZE_BOOTCONFIG             = 3,
   CMD_SIZE_STATUSRSSI             = 3,
   CMD_SIZE_READCHAN               = 3,
   CMD_SIZE_RDSA                   = 3,
   CMD_SIZE_RDSB                   = 3,
   CMD_SIZE_RDSC                   = 3,
   CMD_SIZE_RDSD                   = 3
}si4703_Cmd_Size_T;

typedef enum
{
   RESP_SIZE_DEVICE_ID              = 2,
   RESP_SIZE_CHIPID                 = 2,
   RESP_SIZE_POWERCFG               = 2,
   RESP_SIZE_CHANNEL                = 2,
   RESP_SIZE_SYSCONFIG1             = 2,
   RESP_SIZE_SYSCONFIG2             = 2,
   RESP_SIZE_SYSCONFIG3             = 2,
   RESP_SIZE_TEST1                  = 2,
   RESP_SIZE_TEST2                  = 2,
   RESP_SIZE_BOOTCONFIG             = 2,
   RESP_SIZE_STATUSRSSI             = 2,
   RESP_SIZE_READCHAN               = 2,
   RESP_SIZE_RDSA                   = 2,
   RESP_SIZE_RDSB                   = 2,
   RESP_SIZE_RDSC                   = 2,
   RESP_SIZE_RDSD                   = 2

}si4703_Cmd_Resp_Size_T;

typedef enum
{
   CMD_FM_DETECTOR_SNR             = 8,
   CMD_BLEND_MONO_RSSI             = 8,
   CMD_BLEND_STEREO_RSSI           = 8,
   CMD_CALCODE                     = 8,
   CMD_SNRDB                       = 8,
              
}si4703_Prop_Size_T;
              
typedef enum
{
   RESP_FM_DETECTOR_SNR            = 2,
   RESP_BLEND_MONO_RSSI            = 2,
   RESP_BLEND_STEREO_RSSI          = 2,
   RESP_CALCODE                    = 2,
   RESP_SNRDB                      = 2,
}si4703_Prop_Resp_Size_T;

typedef enum
{
  RADIO_OFF,
  RADIO_INIT,
  RADIO_RUNNING

}radio_T;
extern volatile radio_T radio_state;

typedef struct
{

  unsigned short int device_id ;
  unsigned short int chipid    ;
  unsigned short int powercfg  ;
  unsigned short int channel   ;
  unsigned short int sysconfig1;
  unsigned short int sysconfig2;
  unsigned short int sysconfig3;
  unsigned short int test1     ;
  unsigned short int test2     ;
  unsigned short int bootconfig;
  unsigned short int statusrssi;
  unsigned short int readchan  ;
  unsigned short int rdsa      ;
  unsigned short int rdsb      ;
  unsigned short int rdsc      ;
  unsigned short int rdsd      ;

}si4703_read_reg_T;
extern volatile si4703_read_reg_T si4703_read_reg;


typedef struct
{
   unsigned char Group_Type_Code;
   unsigned char B_Ver_Code;
   unsigned char Traffic_Prog_Code;
   unsigned char PTY_code;
   unsigned short int BData; 
   unsigned short int CData; 
   unsigned short int DData;
   unsigned short int hashRSD;
   unsigned short int PI_Code;

}rds_data_T;

extern rds_data_T rds_data;
extern unsigned char fm_stereo;
extern unsigned short int fm_freq,prev_fm_freq;
extern volatile unsigned char Is_Manual_Tune;
extern unsigned char volume_mode;
extern unsigned char App_Si_EE_Check_Pending;
extern unsigned short int K_Radio_Data_Read[MAX_CH+1u];
extern unsigned short int K_Radio_Data_Write[MAX_CH+1u];
extern unsigned short int reset_Vol_Timer;
extern unsigned char power_state;
extern char printBuffer[PRINTBUFSIZE];
extern unsigned char rds_mode;
 
/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/
extern void Init_Radio(void);
extern void Init_Si(void);
extern void Process_App_Si(void);
extern unsigned short int Get_Fm_Tune_Status(void);
extern void App_Sound_Detector(void);
extern void Seek_Fm_Start(unsigned char seek);
extern void Set_GPIO1(unsigned char state);
extern void Set_GPIO2(unsigned char state);
extern void Read_si4703(void);
extern void Radio_Enc_Operation(unsigned char dir);
extern void Validate_FM_Data(void);
extern void Radio_Enc_Button(unsigned char duration);
extern void Radio_Touch_Button(unsigned char duration);
extern void Process_Commands(void);
extern void Execute_Commands(unsigned char val);
extern void UART_Printf(char *ptr);
extern void Get_RDS_Info(void);



#endif

