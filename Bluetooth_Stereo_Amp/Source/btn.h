
#ifndef BTN_H
#define BTN_H

#include "p30f3010.h"
#include <stdio.h>
#include "ports.h"

#define MAX_PUSH_BTN       2u
#define MAX_SAMPLES        2u

#define NO_PRESS_TYPE     0u
#define SHORT_PRESS_TYPE  1u
#define LONG_PRESS_TYPE   2u
#define STUCK_TYPE        3u

#define PIN_MAKE_THR          3u
#define PIN_MAKE_SHORT_THR    15u
#define PIN_MAKE_LONG_THR     30u
#define PIN_MAKE_STUCK_THR    100u

#define INFO_BTN        0u
#define PWR_BTN         1u



#define Read_Info_Pin()    RB5
#define Read_Pwr_Pin()     RB1

typedef enum
{
   BTN_INIT=0,
   BTN_RUNNING
}btn_T;

extern btn_T btn_state;
extern unsigned char key_pressed[MAX_PUSH_BTN];
extern unsigned short int key_pressed_type[MAX_PUSH_BTN];
extern void App_Btn_Process(void);
extern void Init_Btn(void);



#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      02-04-2025
 *---------------------------------------------------------------------------*/

