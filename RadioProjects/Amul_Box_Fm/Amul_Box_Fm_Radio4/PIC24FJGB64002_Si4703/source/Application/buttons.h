#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <stdio.h>
#include "hardware.h"

#define SHORT_PRESS   1u
#define LONG_PRESS    2u

#define BREAK_TIMEOUT           (1000u/10u) /*10msec sample*/
#define BREAK_TIMEOUT1          (2000u/10u) /*10msec sample*/

#define TouchBtn_Pin()   _RB3
#define RotBtn_Pin()     _RA4

extern void Scan_Touch_Button(void);
extern void Scan_Buttons(void);
extern void Scan_Rotary_Button(void);



#endif  /* __APP_BUTTONS_H__ */
