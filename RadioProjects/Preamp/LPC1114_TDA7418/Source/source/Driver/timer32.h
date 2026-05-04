/*****************************************************************************
 *   timer32.h:  Header file for NXP LPC1xxx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.20  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER32_H 
#define __TIMER32_H

#define PRESCALE_US (47) //Used by PWM
#define PRESCALE_MS (48000) //Used by 16Bit-TMR0 for generating delay
#define PWM_PERIOD 31u //in micro-seconds ~ 31.250Khz

void delay32Ms(uint8_t timer_num, uint32_t delayInMs);
void TIMER32_0_IRQHandler(void);
void TIMER32_1_IRQHandler(void);
void enable_timer32(uint8_t timer_num);
void disable_timer32(uint8_t timer_num);
void reset_timer32(uint8_t timer_num);
void Init_Timer32(uint8_t timer_num, uint32_t timerInterval);
void init32BTMR0_PWM(void);

#endif /* end __TIMER32_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
