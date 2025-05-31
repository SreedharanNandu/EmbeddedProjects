/*****************************************************************************
 *   timer16.h:  Header file for NXP LPC1xxx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER16_H 
#define __TIMER16_H

#define TIME_INTERVAL   (SystemAHBFrequency/100 - 1)/*10msec timer*/
/* depending on the SystemFrequency and SystemAHBFrequency setting, 
if SystemFrequency = 48Mhz, SystemAHBFrequency = SystemAHBFrequency, 
*/
extern volatile uint32_t timer16_0_counter;
extern volatile uint32_t timer16_1_counter;
extern volatile unsigned char timer16_flag;

void Delay_Ms(uint32_t delayInMs);
void TIMER16_0_IRQHandler(void);
void TIMER16_1_IRQHandler(void);
void Enable_Timer16(uint8_t timer_num);
void Disable_Timer16(uint8_t timer_num);
void Reset_Timer16(uint8_t timer_num);
void Init_Timer16(uint8_t timer_num, uint32_t timerInterval);

#endif /* end __TIMER16_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
