
#ifndef PWM_H
#define PWM_H

//#include "types.h"
#include "io_map.h"
#include "timer.h"
#include "hardware.h"


#define Buzzer_On()  (T3CONbits.TON = 1)
#define Buzzer_Off() (T3CONbits.TON = 0)

extern void Init_PWM1(void);
extern void Set_PWM_Duty(uint16_t);



#endif 
//EOF

