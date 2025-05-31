
#ifndef TIMER_H
#define TIMER_H

#include "io_map.h"
#include "types.h"
#include "int.h"

#define TIMER_INTERRUPT

#define TICKS_FOR_1MS     (4000u-1u)  //for 1msec (4000/(8Mhz/2) = 1.024msec)
#define TMR1_VALUE        TICKS_FOR_1MS 


void Delay_Ms(unsigned long timer_val);
void Init_Timer1(void);
void Turn_On_Timer1(void);
void Turn_Off_Timer1(void);
void Init_Timer2(void);
void Turn_On_Timer2(void);
void Turn_Off_Timer2(void);
void InsertDelay(unsigned int x);

#endif 

