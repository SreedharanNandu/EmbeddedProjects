
#ifndef APP_BTN_H
#define APP_BTN_H

#include "type.h"
#include "i2c.h"


#define MAX_ROT_BTN        4u
#define MAX_PUSH_BTN       2u
#define MAX_BTN            (MAX_ROT_BTN+MAX_PUSH_BTN)
#define MAX_SAMPLES        2u

#define NO_PRESS_TYPE     0u
#define SHORT_PRESS_TYPE  1u
#define LONG_PRESS_TYPE   2u
#define STUCK_TYPE        3u

#define PIN_MAKE_THR          3u
#define PIN_MAKE_SHORT_THR    15u
#define PIN_MAKE_LONG_THR     50u
#define PIN_MAKE_STUCK_THR    100u

#define INFO        0u
#define BAND        1u
#define TUNE_UP     2u
#define TUNE_DWN    3u
#define VOL_UP      4u
#define VOL_DWN     5u



#define Read_Info_Pin()     GPIOGetValue(PORT0,8)
#define Read_Band_Pin()     GPIOGetValue(PORT1,2)

typedef enum
{
   BTN_INIT,
   BTN_RUNNING,
   BTN_SHUT_STATE,
   BTN_OFF_STATE
}btn_T;

extern btn_T btn_state;
extern u8 key_pressed[MAX_BTN];
extern u16 key_pressed_type[MAX_PUSH_BTN];
extern void App_Btn_Process(void);
extern void Init_Btn(void);



#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      02-04-2025
 *---------------------------------------------------------------------------*/

