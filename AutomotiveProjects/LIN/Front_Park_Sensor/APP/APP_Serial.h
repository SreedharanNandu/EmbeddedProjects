/******************************************************************************
** #include's
*******************************************************************************
** Project Name      : 
** File Name         : APP_Serial.h
** Description       : 
** Date              : 
** Version           : 
** Author            : 
******************************************************************************/

#ifndef APP_SERIAL_H
#define APP_SERIAL_H

/*****************************************************************************
System/Project Includes                            *
******************************************************************************/
#include "system_def.h"

/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/

// Buzzer features
typedef enum 
{
    BUZZER_ZONE0=0,//continuous beep
    BUZZER_ZONE1,
    BUZZER_ZONE2,
    BUZZER_ZONE3,
    BUZZER_ZONE4,//no beep
    BUZZER_ZONE_MAX
} buzzerZone_t;


typedef enum
{

   SENSOR_L = 0,
   SENSOR_R,
   SENSOR_MAX
}sensor_t;

typedef struct 
{
    u16 on_time_10ms;   // in 10ms units
    u16 off_time_10ms;  // in 10ms units

} buzzerPattern_t;

extern u8 Distance_Received[7];
extern volatile u8 Lin_Comm_Count;
extern volatile u8 Lin_Comm_Count0;
extern u8 Lin_Next_Transmit_Msg_Timer0;


/******************************************************************************
*                             Function Declarations
*******************************************************************************/
extern void Init_APP(void);
extern void APP_Task_100ms(void);
extern void APP_Task_10ms(void);
extern void APP_Task_5ms(void);
extern void App_Dist_Task(void);
extern void App_Chime_Task(void);
extern void App_Read_Sw(void);




#endif