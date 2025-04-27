#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdio.h>
#include "LPC11xx.h"                        /* LPC11xx definitions */
#include "uart.h"
#include "gpio.h"
#include "i2c.h"
#include "type.h"
#include "timer16.h"


//#define TEST_RADIO 1
//#define TEST_EE 1

#define TMR_PERIODIC_RATE_MS 1u
#define PLL_MAX 0x21D2u
#define PLL_MIN 0x1B6Au

#define PRINTBUFSIZE 100u



#define Audio_Mute(x)  /*Nothing*/
#define Stereo_Led(x)  GPIOSetValue(2,1,(x!=0)?0:1)
#define Power_Led(x)   GPIOSetValue(2,0,(x!=0)?0:1)
#define Mode_Led(x)    GPIOSetValue(2,2,(x!=0)?0:1)


#define UART_Sprintf(x,y) sprintf(printBuffer,x,y); \
                          UART_Printf(printBuffer); 

typedef union
{
   unsigned char data_byte1;
   struct
   {
      unsigned char pll:6;
      unsigned char sm:1;
      unsigned char mute:1;
   }bits;
}write_byte1_t;

typedef union
{
   unsigned char data_byte2;
   struct
   {
      unsigned char pll:8;
   }bits;


}write_byte2_t;
typedef union
{
   unsigned char data_byte3;
   struct
   {
      unsigned char swp1:1;
      unsigned char ml:1;
      unsigned char mr:1;
      unsigned char ms:1;
      unsigned char hlsi:1;
      unsigned char ssl:2;
      unsigned char sud:1;
   }bits;

}write_byte3_t;
typedef union
{
   unsigned char data_byte4;
   struct
   {
      unsigned char si:1;
      unsigned char snc:1;
      unsigned char hcc:1;
      unsigned char smute:1;
      unsigned char xtal:1;
      unsigned char bl:1;
      unsigned char stby:1;
      unsigned char swp2:1;
   }bits;


}write_byte4_t;
typedef union
{
   unsigned char data_byte5;
   struct
   {
      unsigned char reserved:6;
      unsigned char dtc:1;
      unsigned char pllref:1;
   }bits;
}write_byte5_t;


typedef struct
{
   write_byte1_t data_byte1;
   write_byte2_t data_byte2;
   write_byte3_t data_byte3;
   write_byte4_t data_byte4;
   write_byte5_t data_byte5;
}radio_write_t;

/*##################### READ ########################*/
typedef union
{
   unsigned char byte1;
   struct
   {
      unsigned char pll:6;
      unsigned char blf:1;
      unsigned char rf:1;
   }bits;
}read_byte1_t;

typedef union
{
   unsigned char byte2;
   struct
   {
      unsigned char pll:8;
   }bits;
}read_byte2_t;


typedef union
{
   unsigned char byte3;
   struct
   {
      unsigned char pll_if:7;
      unsigned char stereo:1;
   }bits;

}read_byte3_t;
typedef union
{
   unsigned char byte4;
   struct
   {
      unsigned char reserved:1;
      unsigned char ci:3;
      unsigned char lev:4;
   }bits;


}read_byte4_t;
typedef union
{
   unsigned char rsrvd;
   struct
   {
      unsigned char reserved:8;
   }bits;
}read_byte5_t;



typedef struct
{
   read_byte1_t data_byte1;
   read_byte2_t data_byte2;
   read_byte3_t data_byte3;
   read_byte4_t data_byte4;
   read_byte5_t data_byte5;
}radio_read_t;

extern radio_read_t Radio_Read;
extern radio_write_t Radio_Write;

extern volatile unsigned char K_Radio_Data_Read[21];
extern volatile unsigned char K_Radio_Data_Write[21];

extern volatile unsigned int pll;
extern volatile unsigned char key_pressed,mode_pressed;

extern volatile char  UARTData[UART_BUFSIZE];
extern volatile unsigned char UARTFlag;

extern volatile unsigned char index,preset;

typedef enum
{
   NONE,
   UP,
   DOWN
}seek_t;

extern volatile seek_t seek;
typedef  enum
{
   IDLE,
   INIT,
   UPDATE
}state_t;

typedef enum
{
   TMR_IDLE,
   TMR_ACTIVE,
   TMR_EXPIRED
          
}tmr_status_t;

typedef enum
{  
   TMR1,
   TMR2,
   TMR3,
   TMR4,
   TMR5,
   TMR6,
   TMR_MAX_CNT
}tmr_id_t;  

typedef struct
{
   tmr_status_t tmr_status;
   unsigned long tmr_count;

}tmr_data_t;

extern state_t ee_state;
extern state_t radio_state;
extern state_t sensor_state;
extern state_t serial_state;

extern volatile unsigned char disp_freq_once;


extern char printBuffer[PRINTBUFSIZE];

extern void Process_Uart(void); 
extern void Process_Buttons(void);
extern void Process_Eeprom(void);
extern void Process_Sensor(void);
extern void Process_Radio(void);
extern void Process_Timer(void);
extern void Process_Commands(void);
extern void UART_Printf(char *ptr);


extern void EE_Read(unsigned int address,
                            unsigned char *data,
                            unsigned char length);
extern void EE_Write(unsigned int address,
                             unsigned char *data,
                             unsigned char length);
 
extern void Start_Timer(tmr_id_t id , unsigned long time);
extern tmr_status_t  Get_Timer_Status(tmr_id_t id);
extern void  Set_Timer_Status(tmr_id_t  id , tmr_status_t  status);
extern void Set_Timer_Count(tmr_id_t id ,unsigned long count);
extern void Reset_All_Timers(void);
extern void Stop_Timer(tmr_id_t id);
extern void Write_Radio(unsigned char *data,unsigned char length);
extern void Read_Radio(unsigned char *data,unsigned char length);
extern void Init_Encoder(void);
extern unsigned int Convert_PLL_To_Freq(void);
extern void Execute_Commands(unsigned char val);
extern void Process_Encoder( void );
extern void Init_Radio(void);
extern void Init_Led(void); 

#endif  /* __TYPE_H__ */
