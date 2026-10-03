#ifndef __BUTTONS_H__
#define __BUTTONS_H__

#include <stdio.h>
#include "hardware.h"

#define SHORT_PRESS   1u
#define LONG_PRESS    2u

#define BREAK_TIMEOUT           (1000u/10u) /*10msec sample*/
#define BREAK_TIMEOUT1          (2000u/10u) /*10msec sample*/

#define VolRotBtn_Pin()   _RB3
#define TuneRotBtn_Pin()    _RA4

typedef enum
{
   VOL_BTN=0,
   TUNE_BTN,
   MAX_BTNS
}Button_T;


extern void Init_Buttons(void);
extern void Scan_Buttons(void);



#endif  /* __APP_BUTTONS_H__ */
