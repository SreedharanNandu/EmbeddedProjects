#ifndef APP_BUTTONS_H
#define APP_BUTTONS_H

#include <stdio.h>

typedef enum
{
  BTN_FALLING_EDGE = 0u,
  BTN_RISING_EDGE,
  BTN_LEVEL_LOW,
  BTN_LEVEL_HIGH   

}Btn_Trigger_T;


typedef enum
{
   BTN_1 = 0u,   /* SW1 - top of ladder    (~3/4 VDD when unpressed) */
   BTN_2,        /* SW2 - 2nd junction     (~1/2 VDD when unpressed) */
   BTN_3,        /* SW3 - 3rd junction     (~1/4 VDD when unpressed) */
   BTN_4,        /* SW4 - bottom junction  (~0V  when unpressed)     */
   MAX_BTNS
}Button_T;

extern volatile unsigned char btn_pin[MAX_BTNS],prev_btn_pin[MAX_BTNS];
extern volatile unsigned long btn_break_timer[MAX_BTNS];
extern volatile unsigned char btn_state[MAX_BTNS];

extern void Init_Buttons(void);
extern void Scan_Buttons(void);

#endif  /* APP_BUTTONS_H */
