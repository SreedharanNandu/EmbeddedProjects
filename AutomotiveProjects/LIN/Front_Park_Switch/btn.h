
#ifndef BTN_H
#define BTN_H

#include "pic12f1822.h"
#include <stdio.h>

/**********************************************************************************
Global definition
***********************************************************************************/

#define PIN_MAKE_THR          3u
#define PIN_MAKE_SHORT_THR    15u
#define PIN_MAKE_LONG_THR     30u
#define PIN_MAKE_STUCK_THR    100u


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
/**********************************************************************************
Global variables
***********************************************************************************/

/***********************************************************************************
Function Prototypes
***********************************************************************************/
extern void App_Btn_Process(void);



#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *---------------------------------------------------------------------------*/

