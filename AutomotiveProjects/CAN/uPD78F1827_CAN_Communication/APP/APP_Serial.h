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

/******************************************************************************
*                             Function Declarations
*******************************************************************************/
extern void APP_Task_1s(void);
extern void APP_Task_100ms(void);
extern void APP_Task_250ms(void);
extern void APP_Task_500ms(void);
extern void APP_Task_10ms(void);
extern void to_ascii(char* dest, unsigned char *text,unsigned char num);

#endif