
#ifndef BTN_H
#define BTN_H

#include "pic12f1822.h"
#include <stdio.h>


#define NO_PRESS_TYPE     0u
#define SHORT_PRESS_TYPE  1u
#define LONG_PRESS_TYPE   2u
#define STUCK_TYPE        3u

#define PIN_MAKE_THR          3u
#define PIN_MAKE_SHORT_THR    15u
#define PIN_MAKE_LONG_THR     30u
#define PIN_MAKE_STUCK_THR    100u

#define INFO_BTN        0u

#define Read_Info_Pin()    RA2




extern volatile unsigned char key_pressed;
extern volatile unsigned char key_pressed_type;


extern void App_Btn_Process(void);
extern void Init_Btn(void);



#endif

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      02-04-2025
 *---------------------------------------------------------------------------*/

