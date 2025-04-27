/*filename:- system.h*/
#ifndef SYSTEM_H
#define SYSTEM_H 
#include "p30f3010.h"

#define FALSE  0
#define TRUE   1


#define PWR_OFF     0u
#define PWR_ON      1u

typedef enum
{
   BT_ADD,
   MODE_ADD,
   NO_ADD


}eeprom_mem_address_T;
typedef enum
{
   BT_MODE,
   NO_MODE


}radio_modes_T;
typedef enum
{
   NEXT,
   PREV,
   SYS_ON,
   SYS_OFF,
   PLAY,
   PAUSE,
   STOP,
   NO_FUNC
}various_func_T;

typedef enum
{
   OFF=0,
   ON=1
}Bool_T;

typedef enum
{
   TASK_5MS=0,
   TASK_10MS,
   TASK_50MS,
   TASK_100MS,
   TASK_1S
}Task_T;



extern volatile unsigned char Power_Mode;
extern volatile unsigned char Task_Activate;
extern volatile Task_T Task_Now;
extern void Delay_Ms(unsigned long timer_val);
extern void UARTSend(unsigned char *data,unsigned char len);
extern void Shut_PowerAmp(void);
extern void Start_PowerAmp(void);


#endif
