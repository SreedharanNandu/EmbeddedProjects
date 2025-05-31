#ifndef __APP_ROTARY_H__
#define __APP_ROTARY_H__

#include "p30f3010.h"
#include <stdio.h>
#include "ports.h"

#define LEFT_TURN  1u
#define RIGHT_TURN 2u

#define EncA_Pin()   RB4             
#define EncB_Pin()   RB0             

extern volatile unsigned char Encoder_Count;


extern void Init_Encoder(void);
extern void Fast_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder(void);


#endif  /* __APP_ROTARY_H__ */
