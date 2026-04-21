#ifndef __APP_BUTTONS_H__
#define __APP_BUTTONS_H__

#include <stdio.h>

#define SHORT_PRESS   1u
#define LONG_PRESS    2u



typedef enum
{
   TUNE_BTN = 0u,
   PWR_BTN,
   MAX_BTNS
}Button_T;

extern volatile unsigned char btn_pin[MAX_BTNS],prev_btn_pin[MAX_BTNS];
extern volatile unsigned long btn_break_timer[MAX_BTNS];
extern volatile unsigned char btn_state[MAX_BTNS];

extern void Init_Buttons(void);
extern void Scan_Buttons(void);


#endif  /* __APP_BUTTONS_H__ */
