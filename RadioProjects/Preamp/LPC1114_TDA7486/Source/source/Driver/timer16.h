/*****************************************************************************
 *   timer16.h: 
******************************************************************************/
#ifndef __TIMER16_H 
#define __TIMER16_H



#define TIME_INTERVAL   (SystemAHBFrequency/1000u - 1u)/*10msec timer*/


#define TMR16_THR_1MS     1u
#define TMR16_THR_10MS    10u
#define TMR16_THR_100MS   100u
#define TMR16_THR_1S      1000u

/* depending on the SystemFrequency and SystemAHBFrequency setting, 
if SystemFrequency = 48Mhz, SystemAHBFrequency = SystemAHBFrequency, 
*/
extern volatile uint32_t timer16_0_counter;
extern volatile uint32_t timer16_1_counter;

extern volatile unsigned char timer16_1ms_flag;
extern volatile unsigned char timer16_10ms_flag;
extern volatile unsigned char timer16_100ms_flag;
extern volatile unsigned char timer16_1s_flag;

extern volatile unsigned int timer16_10ms_cnt;
extern volatile unsigned int timer16_100ms_cnt; 
extern volatile unsigned int timer16_1s_cnt;

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
