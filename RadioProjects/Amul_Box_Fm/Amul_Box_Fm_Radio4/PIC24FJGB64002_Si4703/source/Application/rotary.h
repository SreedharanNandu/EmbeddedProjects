#ifndef __APP_ROTARY_H__
#define __APP_ROTARY_H__

#include <stdio.h>
#include "hardware.h"

#define LEFT_TURN  1u
#define RIGHT_TURN 2u

#define EncA_Pin()   _RB5             
#define EncB_Pin()   _RB4             


extern void Init_Encoder(void);
extern void Fast_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder(void);


#endif  /* __APP_ROTARY_H__ */
