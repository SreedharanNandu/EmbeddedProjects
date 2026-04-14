/*****************************************************************************
 *   type.h:  
******************************************************************************/
#ifndef __TYPE_H__
#define __TYPE_H__

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#define LOG_OPTION  1u

#define MAX_EXIT_LOOP_TH   512u

#define DISP_RELOAD 80u

#define IDLE_EE   0u
#define INIT_EE   1u
#define UPDATE_EE 2u

#define BLINK_MODE_CHANGE  1u
#define STORE              2u
#define PWNONOFF           3u
#define ROTCHANGE          4u

#define UART_BUFSIZE      3u

#define MAX_EE_SIZE ((MAX_CH_SIZE) + 2u + 2u) /* 10 channels (2byte each)  + 2byte index + 2byte CRC */

#define PAGE_EE(x)   (x*128u) /* page is 128 bytes */

#define RADIO_ROT_ENC_INACTIVE_RESP_TIME_MS    (100u)
#define RADIO_ENC_IDLE_ENCODER_STATE  0        ///* Idle encoder state*/
#define RADIO_ENC_MAXIMUM_ENCODER_STATE  (2 )       ///* Maximum 2 edge per detent encoder state*/
#define RADIO_ENC_MINIMUM_ENCODER_STATE  (-2 )      ///* Minimum 2 edge per detent encoder state*/
#define RADIO_ENC_ZERO_ENCODER_COUNT     (0x00)    ///* Zero*/ 
#define RADIO_ENC_MAXIMUM_ENCODER_COUNT  (0x0E)    
#define RADIO_ENC_MINIMUM_ENCODER_COUNT  (0x01)    ///* Minimum encoder value is the same for Radio*/ 
#define RADIO_ENC_ZERO_SAMPLES   (0u)
#define RADIO_ENC_TIMER_EXPIRED  (0u)
#define RADIO_ENC_DEBOUNCE_PERIOD_NORMAL  (1u)
#define RADIO_ENC_EDGE_DELAY         (20u)
#define RADIO_ENC_EDGE_FASTER        (20u)  /* example 2 seconds / period (100ms)  = 20 */
#define RADIO_ENC_EDGE_DELAY_EXPIRED (0u)
#define RADIO_ENC_USE_ENCODER_B_TO_COUNT (0)/* Use only encoder Gray code phase A to count the detents*/

#define SHORT_PRESS   1u
#define LONG_PRESS    2u

#define BREAK_TIMEOUT           (1000u/10u) /*10msec sample*/
#define BTN_BREAK_TIMEOUT       (5000u/10u) /*10msec sample*/


#define Service_Watchdog() (WDTCTL = WDTPW + WDTSSEL + WDTCNTCL)
/* Interval mode: WDTTMSEL enables ISR instead of auto-reset */
//#define Service_Watchdog() (WDTCTL = WDTPW + WDTTMSEL + WDTSSEL + WDTCNTCL)
#define Stop_Watchdog()    (WDTCTL = WDTPW + WDTHOLD)
#define Start_Watchdog()   Service_Watchdog()


#define ON (1u)
#define OFF (0u)

#define EE_I2C_ADDRESS        0x50u

#define UART_SIZE             3u

// CRC16 Polynomial (CRC-16-CCITT)
#define CRC16_POLYNOMIAL           0x8005u
#define CRC16_INITIAL_VALUE        0xFFFFu


#define si4703_Rst(x)      ((x==1)?(P2OUT |= BIT4):(P2OUT &= ~BIT4))
#define si4703_Rsen(x)      ((x==1)?(P2OUT |= BIT5):(P2OUT &= ~BIT5)) 

#define Set_GPIO1_Val(x)  ((x) ?  3u : 2u)
#define Set_GPIO2_Val(x)  ((x) ? 12u : 8u)

#define SAMPLE_PER             10u

#define FIFO_SIZE              10u


#define Led(x)                  EnQ(&BlinkQ,x)



#define RADIO_ENABLED                1U
#define SET_FM_TUNE_RSSI_THRESHOLD   5536u
#define OFF_STATE                    0u
#define ON_STATE                     1u
#define LEFT_TURN                    0u 
#define RIGHT_TURN                   1u



#define RESET_VOL_TIMER_TH    100u


#define SHORT_PRESS      1u
#define LONG_PRESS       2u

#define SI_IC_ADDRESS (0x10u)
#define SI_SLAVE_ADDRESS (SI_IC_ADDRESS<<1)

#define R 0x00u
#define W 0x01u

#define SEEK_UP            1u
#define SEEK_DWN           0u


#define MAX_CH         10u
#define MAX_CH_SIZE    20u

#define PRINTBUFSIZE   100u

#define NUM_ENCODERS   2u
#define NUM_BUTTONS    2u

#define ENC1           0u
#define ENC2           1u

#define ROT_BTN1       0u
#define ROT_BTN2       1u



typedef enum
{
   PWR_OFF,
   PWR_ON

}power_mode_T;

extern power_mode_T Power_Mode;



#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


typedef enum
{
   FLT_RADIO_INIT=0,
   FLT_RADIO_VOL,
   FLT_RADIO_SET_FM,
   FLT_RADIO_GPIO1,
   FLT_RADIO_GPIO2,
   FLT_RADIO_NONE,
   FLT_EE_PAGE1_INIT,
   FLT_EE_PAGE2_INIT,
   FLT_EE_NONE,
   FLT_TRAP
}Fault_Type_T;


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
  RADIO_RUNNING,
  RADIO_RESET

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



extern unsigned char fm_stereo;
extern unsigned short int fm_freq,prev_fm_freq;
extern volatile unsigned char Is_Manual_Tune;
extern unsigned char App_Si_EE_Check_Pending;
extern unsigned short int K_Radio_Data_Read[MAX_CH+1u];
extern unsigned short int K_Radio_Data_Write[MAX_CH+1u];
extern unsigned short int reset_Vol_Timer;
extern unsigned char power_state;
extern unsigned char monitor_mode;
extern unsigned char preset_index;
extern unsigned char si_vol;


// Structure to hold the blink request
typedef struct 
{
    unsigned char id;            // ID
    unsigned char num_blinks;    // Number of blinks
    unsigned short int on_time;  // On time in milliseconds
    unsigned short int off_time; // Off time in milliseconds
} BlinkRequest_T;

// FIFO queue structure
typedef struct 
{
    BlinkRequest_T queue[FIFO_SIZE];
    unsigned char front;
    unsigned char rear;
    unsigned char size;
} FIFOQueue_T;

typedef enum
{
   NO_RADIO_TURN=0,
   LEFT_RADIO_TURN,
   RIGHT_RADIO_TURN

}Radio_Dir_T;

extern const BlinkRequest_T K_blink_ModeChange;
extern const BlinkRequest_T K_blink_PwnOnOff;
extern const BlinkRequest_T K_blink_Store;
extern const BlinkRequest_T K_blink_RotChange;

extern FIFOQueue_T BlinkQ;

extern volatile unsigned int _1msCounter;
extern volatile unsigned int _10msCounter;
extern volatile unsigned int _100msCounter;
extern volatile unsigned int _1sCounter;

extern unsigned char UARTData[UART_SIZE];
extern unsigned char UARTFlag;

extern const unsigned char K_Power_Led_Delay;
extern const unsigned int K_Uart_Baud;

extern const unsigned int K_Fm_Max_Freq;
extern const unsigned int K_Fm_Min_Freq;

extern const unsigned char K_Default_Si_Vol_Level;
extern const unsigned char K_Min_Si_Vol_Level;
extern const unsigned char K_Max_Si_Vol_Level;

extern unsigned char ee_state;

#if defined(LOG_OPTION)
extern const char K_Ch_Text[11][5]; 
extern const char K_Vol_Text[17][7]; 
extern const char K_Freq_Text[208][8];
#endif
extern const char K_Welcome_Text[23];
extern const char K_Saved_Text[5];
extern const char K_Preset_Text[11];
extern const char K_Manual_Text[11];
extern const char K_PwrON_Text[8]; 
extern const char K_PwrOFF_Text[9]; 
extern const char K_Mute_Text[5]; 
extern const char K_VolSelON_Text[10]; 
extern const char K_VolSelOFF_Text[11]; 

extern volatile unsigned int WDT_Trip_PC;
extern volatile unsigned int WDT_Reset_Flag;
#define WDT_TRIP_MAGIC   0xBEEFu

typedef enum
{
   SEND_WELCOME=0,
   SEND_VOL,
   SEND_CH,
   SEND_SAVED,
   SEND_PRESET,
   SEND_MANUAL,
   SEND_FREQ,
   SEND_PWR_ON,
   SEND_PWR_OFF,
   SEND_MUTE,
   SEND_VOL_SEL_ON,
   SEND_VOL_SEL_OFF
}Send_Uart_Type_T;

extern void InitTimerA1(void) ;
extern void Delay_Ms(unsigned int ms) ;

extern void LED_On(unsigned char id);
extern void LED_Off(unsigned char id); 

extern void Init_Uart(unsigned int baud);
extern void Send_Uart(unsigned char *ptr , unsigned int size);

extern unsigned char EESeqRead(unsigned int startAddress , unsigned char *dPtr  , unsigned int Size);
extern unsigned char EESeqWrite(unsigned int startAddress , unsigned char *dPtr , unsigned int size);
extern void Validate_EE_Read_Data(void);
extern void Validate_EE_Write_Data(void);
extern unsigned int CRC16_CCITT(unsigned char *data, unsigned int length); 
extern void Change_Mode_EE(unsigned char state);
extern void Process_Eeprom(void);

extern void InitGPIO(void);
extern void ShutGPIO(void);

extern void InitI2C(unsigned char eeprom_i2c_address);
extern void ShutI2C(void);
extern void I2CWriteInit(void);
extern void I2CReadInit(void);

extern void ReadEE(void);
extern void WriteEE(void);

extern unsigned char Read_si4703(void);
extern unsigned char Write_si4703(void);
extern unsigned char Power_Radio(unsigned char power_up);
extern void Set_Fm_Tune_Freq(unsigned short int freq);
extern void Set_Vol(unsigned char vol_level);
extern void Log_Printf(Send_Uart_Type_T id);
extern void Execute_Commands(unsigned char val);
extern void Set_GPIO1(unsigned char state);
extern void Set_GPIO2(unsigned char state);

extern void Enter_Sleep(void);
extern void Init_Si(void);
extern void Init_Radio(void);
extern unsigned char Write_Si_I2C(unsigned char *dataPtr,unsigned char size);
extern unsigned char Read_Si_I2C(unsigned char *dataPtr,unsigned char size);

extern void Process_Commands(void);
extern void Process_App_Si(void);

extern void Led_Task(void);
extern void Init_Fifo(FIFOQueue_T* q) ;
extern void EnQ(FIFOQueue_T* q, BlinkRequest_T req); 
extern BlinkRequest_T DeQ(FIFOQueue_T* q); 
extern void Led_Process(FIFOQueue_T* q);
extern void Led_Init(void); 

extern void Scan_Buttons(void);
extern void Scan_Rotary_Button(void);
extern void Init_Encoder(void);
extern void Fast_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder(void);

extern void Radio_Enc1_Button(unsigned char duration);
extern void Radio_Enc2_Button(unsigned char duration);

extern void Radio_Enc1_Operation(unsigned char dir);
extern void Radio_Enc2_Operation(unsigned char dir);

extern unsigned char exitLoop(unsigned char id);
extern void Fault(Fault_Type_T id ,unsigned char status);

extern void PowerUp_Check(void);
extern void Software_Reset(void) ;

extern char* Get_Vol_Text(unsigned char vol);
extern char* Get_Ch_Text(unsigned char ch);
extern char* Get_Freq_Text(unsigned short int fm_freq);

extern void Amp_Mute(unsigned char state);
extern void Amp_Power(unsigned char state);


#endif  /* __TYPE_H__ */
