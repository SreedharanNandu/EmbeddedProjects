/******************************************************************************
* File Name          : timer16.c
*******************************************************************************/
#include "LPC11xx.h"
#include "timer16.h"
#include "gpio.h"
#include "uart.h"

volatile uint32_t timer16_0_counter = 0;
volatile uint32_t timer16_1_counter = 0;
volatile unsigned char timer16_1ms_flag;
volatile unsigned char timer16_10ms_flag;
volatile unsigned char timer16_100ms_flag;
volatile unsigned char timer16_1s_flag;
volatile unsigned int timer16_10ms_cnt;
volatile unsigned int timer16_100ms_cnt; 
volatile unsigned int timer16_1s_cnt;

/*****************************************************************************
** Function name:      delayMs
**
** Descriptions:      Start the timer delay in mili seconds
**                  until elapsed
**
** parameters:         timer number, Delay value in mili second          
**                   
** Returned value:      None
** 
*****************************************************************************/
void Delay_Ms(uint32_t delayInMs)
{
    /*
    * setup timer #1 for delay
    */
    LPC_TMR16B1->TCR = 0x02;      /* reset timer */
    LPC_TMR16B1->PR  = 0xBB80;   /* set prescaler to zero */
    LPC_TMR16B1->MR0 = delayInMs;//48Mhz/48000 = 1Khz,i.e 1ms ,so 65536*1msec = 65sec
    LPC_TMR16B1->IR  = 0xff;      /* reset all interrrupts */
    LPC_TMR16B1->MCR = 0x04;      /* stop timer on match */
    LPC_TMR16B1->TCR = 0x01;      /* start timer */
    /* wait until delay time has elapsed */
    while (LPC_TMR16B1->TCR & 0x01);
    return;
}
/******************************************************************************
** Function name:      TIMER_0_IRQHandler
**
** Descriptions:      Timer/Counter 0 interrupt handler
**                  executes each 10ms @ 48 Mhz CPU Clock
**
** parameters:         None
** Returned value:      None
** 
******************************************************************************/
void TIMER16_0_IRQHandler(void)
{  
  
   LPC_TMR16B0->IR = 1u;         /* clear interrupt flag */
   
   timer16_0_counter++;
   if(timer16_0_counter >= 1U)//1ms
   {
      timer16_0_counter=0u;
      if(timer16_1ms_flag == 0u)
      {  
         timer16_1ms_flag = 1;
      }
   }
   timer16_10ms_cnt++;
   if(TMR16_THR_10MS <= timer16_10ms_cnt)//10ms
   {
      timer16_10ms_cnt = 0;
      if(timer16_10ms_flag == 0u)
      {  
         timer16_10ms_flag = 1;
      }
   }
   timer16_100ms_cnt++;
   if(TMR16_THR_100MS <= timer16_100ms_cnt)//100ms
   {
      timer16_100ms_cnt = 0;
      if(timer16_100ms_flag == 0u)
      {  
         timer16_100ms_flag = 1;
      }
   }
   timer16_1s_cnt++;
   if(TMR16_THR_1S <= timer16_1s_cnt)//1s
   {
      timer16_1s_cnt = 0;
      if(timer16_1s_flag == 0u)
      {  
         timer16_1s_flag = 1;
      }
   }
   return;
}

/******************************************************************************
** Function name:      TIMER_1_IRQHandler
**
** Descriptions:      Timer/Counter 1 interrupt handler
**                  executes each 10ms @ 48 Mhz CPU Clock
**
** parameters:         None
** Returned value:      None
** 
******************************************************************************/
void TIMER16_1_IRQHandler(void)
{  
   LPC_TMR16B1->IR = 1;         /* clear interrupt flag */
   timer16_1_counter++;
   return;
}

/******************************************************************************
** Function name:      enable_timer
**
** Descriptions:      Enable timer
**
** parameters:         timer number: 0 or 1
** Returned value:      None
** 
******************************************************************************/
void Enable_Timer16(uint8_t timer_num)
{
   if ( timer_num == 0 )
   {
      LPC_TMR16B0->TCR = 1;
   }
   else
   {
      LPC_TMR16B1->TCR = 1;
   }
   return;
}

/******************************************************************************
** Function name:      disable_timer
**
** Descriptions:      Disable timer
**
** parameters:         timer number: 0 or 1
** Returned value:      None
** 
******************************************************************************/
void Disable_Timer16(uint8_t timer_num)
{
   if ( timer_num == 0 )
   {
      LPC_TMR16B0->TCR = 0;
   }
   else
   {
      LPC_TMR16B1->TCR = 0;
   }
   return;
}

/******************************************************************************
** Function name:      reset_timer
**
** Descriptions:      Reset timer
**
** parameters:         timer number: 0 or 1
** Returned value:      None
** 
******************************************************************************/
void Reset_Timer16(uint8_t timer_num)
{
   uint32_t regVal=0;

   if ( timer_num == 0 )
   {
      regVal = LPC_TMR16B0->TCR;
      regVal |= 0x02;
      LPC_TMR16B0->TCR = regVal;
   }
   else
   {
      regVal = LPC_TMR16B1->TCR;
      regVal |= 0x02;
      LPC_TMR16B1->TCR = regVal;
   }
   return;
}

/******************************************************************************
** Function name:      init_timer
**
** Descriptions:      Initialize timer, set timer interval, reset timer,
**                  install timer interrupt handler
**
** parameters:         timer number and timer interval
** Returned value:      None
** 
******************************************************************************/
void Init_Timer16(uint8_t timer_num, uint32_t TimerInterval) 
{
  if ( timer_num == 0 )
  {
     /* Some of the I/O pins need to be clearfully planned if
     you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
     LPC_SYSCON->SYSAHBCLKCTRL |= (1<<7);
     #ifdef __JTAG_DISABLED
     LPC_IOCON->JTAG_TCK_PIO0_10 &= ~0x07;
     LPC_IOCON->JTAG_TCK_PIO0_10 |= 0x03;      /* Timer0_16 MAT2 */
     #endif   

     timer16_0_counter = 0;
     LPC_TMR16B0->PR = 0;
     LPC_TMR16B0->TC = 0;
     LPC_TMR16B0->MR0 = TimerInterval;
     LPC_TMR16B0->MCR = 3;            /* Interrupt and Reset on MR0 */
     /* Enable the TIMER0 Interrupt */
     NVIC_EnableIRQ(TIMER_16_0_IRQn);
  }
  else if ( timer_num == 1 )
  {
     /* Some of the I/O pins need to be clearfully planned if
     you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
     LPC_SYSCON->SYSAHBCLKCTRL |= (1<<8);
  }
  else
  {
     /*do nothing*/
  }
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
