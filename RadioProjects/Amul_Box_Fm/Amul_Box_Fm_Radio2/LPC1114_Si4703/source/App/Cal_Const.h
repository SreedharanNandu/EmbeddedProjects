#ifndef CAL_CONST_H
#define CAL_CONST_H

#include <stdio.h>
#include "type_sys.h"
#include "App_Led.h"

extern const unsigned long K_Break_Timeout; /*( X 10ms)sec sample*/
extern const BlinkRequest_T blink_ModeChange; // blinks, on and off
extern const BlinkRequest_T blink_Store; // blinks, on and off
extern const BlinkRequest_T blink_PwnOnOff; // blinks, on and off

#endif  /* __CAL_CONST_H__ */
