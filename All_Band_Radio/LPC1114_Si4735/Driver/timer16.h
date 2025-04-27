/*****************************************************************************
 *   timer16.h:  
******************************************************************************/
#ifndef TIMER16_H 
#define TIMER16_H

#include "type.h"


#define TIME_INTERVAL   (SystemAHBFrequency/1000 - 1)/*1msec timer*/

/* depending on the SystemFrequency and SystemAHBFrequency setting, 
if SystemFrequency = 48Mhz, SystemAHBFrequency = SystemAHBFrequency, 
*/

extern volatile u32 timer16_1_counter;
extern volatile u8 timer16_1ms_cnt;
extern volatile u8 timer16_100ms_counter;
extern volatile u16 timer16_10ms_counter;
extern volatile u16 timer16_1s_counter;

void Delay_Ms(uint32_t delayInMs);
void TIMER16_0_IRQHandler(void);
void TIMER16_1_IRQHandler(void);
void Enable_Timer16(uint8_t timer_num);
void Init_Timer16(uint8_t timer_num, uint32_t timerInterval);

#endif /* end __TIMER16_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
