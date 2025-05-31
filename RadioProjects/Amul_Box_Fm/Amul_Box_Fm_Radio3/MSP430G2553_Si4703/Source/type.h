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
#define BREAK_TIMEOUT1          (1000u/10u) /*10msec sample*/





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

#define FIFO_SIZE 10


#define Led(x)                  EnQ(&BlinkQ,x)



#define RADIO_ENABLED                1U
#define SET_FM_TUNE_RSSI_THRESHOLD   5536u
#define OFF_STATE                    0u
#define ON_STATE                     1u
#define LEFT_TURN                    0u 
#define RIGHT_TURN                   1u


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


#define SHORT_PRESS      1u
#define LONG_PRESS       2u

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
   FLT_EE_NONE
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
extern unsigned char volume_mode;
extern unsigned char App_Si_EE_Check_Pending;
extern unsigned short int K_Radio_Data_Read[MAX_CH+1u];
extern unsigned short int K_Radio_Data_Write[MAX_CH+1u];
extern unsigned short int reset_Vol_Timer;
extern unsigned char power_state;
extern unsigned char monitor_mode;

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

extern const BlinkRequest_T blink_ModeChange;
extern const BlinkRequest_T blink_PwnOnOff;
extern const BlinkRequest_T blink_Store;
extern const BlinkRequest_T blink_RotChange;

extern FIFOQueue_T BlinkQ;

extern volatile unsigned int _1msCounter;
extern volatile unsigned int _10msCounter;
extern volatile unsigned int _100msCounter;
extern volatile unsigned int _1sCounter;

extern unsigned char UARTData[UART_SIZE];
extern unsigned char UARTFlag;


extern unsigned char ee_state;

extern const char Ch_Text[11][5]; 
extern const char Vol_Text[17][7]; 
extern const char Saved_Text[5];
extern const char Preset_Text[11];
extern const char Manual_Text[11];
extern const char Freq_Text[208][8];
extern const char PwrON_Text[8]; 
extern const char PwrOFF_Text[9]; 
extern const char Mute_Text[5]; 
extern const char VolSelON_Text[10]; 
extern const char VolSelOFF_Text[11]; 

extern unsigned char disp_timer;
extern unsigned char prev_fm_stereo;
extern unsigned char Scan_Cnt;
extern unsigned char first_time_power_up;


typedef enum
{
   SEND_VOL=0,
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

void InitTimerA1(void) ;
void Delay_Ms(unsigned int ms) ;

void LED_On(unsigned char id);
void LED_Off(unsigned char id); 

void Init_Uart(unsigned int baud);
void Send_Uart(unsigned char *ptr , unsigned int size);

unsigned char EESeqRead(unsigned int startAddress , unsigned char *dPtr  , unsigned int Size);
unsigned char EESeqWrite(unsigned int startAddress , unsigned char *dPtr , unsigned int size);
void Validate_EE_Read_Data(void);
void Validate_EE_Write_Data(void);
unsigned int CRC16_CCITT(unsigned char *data, unsigned int length); 
void Change_Mode_EE(unsigned char state);
void Process_Eeprom(void);


void InitI2C(unsigned char eeprom_i2c_address);
void I2CWriteInit(void);
void I2CReadInit(void);

void ReadEE(void);
void WriteEE(void);

unsigned char Read_si4703(void);
unsigned char Write_si4703(void);
unsigned char Power_Radio(void);
void Set_Fm_Tune_Freq(unsigned short int freq);
void Set_Vol(unsigned char vol_level);
void UART_Printf(Send_Uart_Type_T id);
void Execute_Commands(unsigned char val);
void Set_GPIO1(unsigned char state);
void Set_GPIO2(unsigned char state);

void Init_Si(void);
void Init_Radio(void);
unsigned char Write_Si_I2C(unsigned char *dataPtr,unsigned char size);
unsigned char Read_Si_I2C(unsigned char *dataPtr,unsigned char size);

void Process_Commands(void);
void Process_App_Si(void);

void Led_Task(void);
void Init_Fifo(FIFOQueue_T* q) ;
void EnQ(FIFOQueue_T* q, BlinkRequest_T req); 
BlinkRequest_T DeQ(FIFOQueue_T* q); 
void Led_Process(FIFOQueue_T* q);
void Led_Init(void); 

extern void Scan_Buttons(void);
extern void Scan_Rotary_Button(void);
extern void Scan_Touch_Button(void);

extern void Init_Encoder(void);
extern void Fast_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder(void);

void Radio_Enc_Button(unsigned char duration);
void Radio_Touch_Button(unsigned char duration);
void Radio_Enc_Operation(unsigned char dir);

unsigned char exitLoop(unsigned char id);
void Fault(Fault_Type_T id ,unsigned char status);

void PowerUp_Check(void);
void Software_Reset(void) ;


#endif  /* __TYPE_H__ */
