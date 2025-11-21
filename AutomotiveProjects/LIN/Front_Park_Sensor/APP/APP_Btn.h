/******************************************************************************
** #include's
*******************************************************************************
** Project Name      : 
** File Name         : APP_Btn.h
** Description       : 
** Date              : 
** Version           : 
** Author            : 
******************************************************************************/

#ifndef APP_BTN_H
#define APP_BTN_H

/*****************************************************************************
System/Project Includes                            *
******************************************************************************/
#include "system_def.h"

/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/
/* ---------- Types ---------- */
typedef enum {
    BTN_RAW_UNKNOWN = 0,
    BTN_RAW_RELEASED,
    BTN_RAW_PRESSED
} BtnRaw_t;

typedef enum {
    BTN_STATE_UKNOWN = 0,
    BTN_STATE_RELEASED,
    BTN_STATE_PRESSED
} BtnState_t;

typedef enum {
    BTN_EVENT_NONE = 0,
    BTN_EVENT_OFF_TO_ON,
    BTN_EVENT_ON_TO_OFF,
    BTN_EVENT_STUCK_PRESSED,
    BTN_EVENT_STUCK_RELEASED
} BtnEvent_t;



/******************************************************************************
*                             Function Declarations
*******************************************************************************/
extern void App_Btn_Task_10ms(void);
extern BtnState_t Get_Button_State(void);




#endif