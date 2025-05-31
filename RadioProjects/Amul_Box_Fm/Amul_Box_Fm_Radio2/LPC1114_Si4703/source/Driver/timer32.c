/******************************************************************************
* File Name          : timer32.c
*******************************************************************************/
#include "LPC11xx.h"
#include "timer32.h"
/*****************************************************************************
** Function name:      delay32Ms
**
** Descriptions:      Start the timer delay in milo seconds
**                  until elapsed
**
** parameters:         timer number, Delay value in milo second          
**                   
** Returned value:      None
** 
*****************************************************************************/
void delay32Ms(uint8_t timer_num, uint32_t delayInMs)
{
  if (timer_num == 0)
  {
     /* setup timer #0 for delay */
     /* reset timer */
     LPC_TMR32B0->TCR = 0x02;      
     /* set prescaler to zero */
     LPC_TMR32B0->PR  = 0x00;      
     LPC_TMR32B0->MR0 = delayInMs * (SystemAHBFrequency / 1000);
     /* reset all interrrupts */
     LPC_TMR32B0->IR  = 0xff;      
     /* stop timer on match */
     LPC_TMR32B0->MCR = 0x04;      
     /* start timer */
     LPC_TMR32B0->TCR = 0x01;      
     /* wait until delay time has elapsed */
     while (LPC_TMR32B0->TCR & 0x01);
  }
  else if (timer_num == 1)
  {
     /* setup timer #1 for delay */
     /* reset timer */
     LPC_TMR32B1->TCR = 0x02;      
     /* set prescaler to zero */
     LPC_TMR32B1->PR  = 0x00;      
     LPC_TMR32B1->MR0 = delayInMs * (SystemAHBFrequency / 1000);
     /* reset all interrrupts */
     LPC_TMR32B1->IR  = 0xff;      
     /* stop timer on match */
     LPC_TMR32B1->MCR = 0x04;      
     /* start timer */
     LPC_TMR32B1->TCR = 0x01;      
     /* wait until delay time has elapsed */
     while (LPC_TMR32B1->TCR & 0x01);
  }
  else
  {
     /*do nothing*/
  }

  return;
}

/******************************************************************************
** Function name:      TIMER32_0_IRQHandler
**
** Descriptions:      Timer/Counter 0 interrupt handler
**                  executes each 10ms @ 48 Mhz CPU Clock
**
** parameters:         None
** Returned value:      None
** 
******************************************************************************/
void TIMER32_0_IRQHandler(void)
{  
   LPC_TMR32B0->IR = 1;         /* clear interrupt flag */
   return;
}

/******************************************************************************
** Function name:      TIMER32_1_IRQHandler
**
** Descriptions:      Timer/Counter 1 interrupt handler
**                  executes each 10ms @ 48 Mhz CPU Clock
**
** parameters:         None
** Returned value:      None
** 
******************************************************************************/
void TIMER32_1_IRQHandler(void)
{  
   LPC_TMR32B1->IR = 1;         /* clear interrupt flag */
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
void enable_timer32(uint8_t timer_num)
{
   if ( timer_num == 0 )
   {
     LPC_TMR32B0->TCR = 1;
   }
   else
   {
     LPC_TMR32B1->TCR = 1;
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
void disable_timer32(uint8_t timer_num)
{
   if ( timer_num == 0 )
   {
      LPC_TMR32B0->TCR = 0;
   }
   else
   {
      LPC_TMR32B1->TCR = 0;
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
void reset_timer32(uint8_t timer_num)
{
  uint32_t regVal;

  if ( timer_num == 0 )
  {
     regVal = LPC_TMR32B0->TCR;
     regVal |= 0x02;
     LPC_TMR32B0->TCR = regVal;
  }
  else
  {
     regVal = LPC_TMR32B1->TCR;
     regVal |= 0x02;
     LPC_TMR32B1->TCR = regVal;
  }
  return;
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void init32BTMR0_PWM(void)
{
   /*Assuming CCLK (System Clock) = 48Mhz (set by startup code)*/
   LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9); //Enable 32Bit-TMR0 Clock
   LPC_TMR32B0->CTCR = 0x0;
   LPC_TMR32B0->PR = PRESCALE_US; //Increment TC at every 47999+1 clock cycles
   //48000 clock cycles @48Mhz = 1 mS base timer
   LPC_TMR32B0->MR3 = PWM_PERIOD; //1ms Period duration(for 31.250Khz clock)
   LPC_TMR32B0->MR0 = PWM_PERIOD/2; //Default TOFF time in mS 50% DC
   LPC_TMR32B0->MCR = (1<<10); //Reset on MR3 Match
   LPC_TMR32B0->PWMC = 0x9; //Enable PWM Mode for MAT0.0 and MAT3
   LPC_TMR32B0->TCR = 0x2; //Reset Timer
   LPC_TMR32B0->TCR = 0x1; //Enable timer
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
void Init_Timer32(uint8_t timer_num, uint32_t TimerInterval) 
{
  if ( timer_num == 0 )
  {
     /* Some of the I/O pins need to be clearfully planned if
     you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
     LPC_SYSCON->SYSAHBCLKCTRL |= (1<<9);
     /*  Timer0_32 I/O config */
     /* Timer0_32 CAP0 */
     LPC_IOCON->PIO0_1 &= ~0x07;   
     LPC_IOCON->PIO0_1 |= 0x02;   
     #ifdef __JTAG_DISABLED
     /* Timer0_32 MAT3 */
     LPC_IOCON->JTAG_TDI_PIO0_11 &= ~0x07;   
     LPC_IOCON->JTAG_TDI_PIO0_11 |= 0x03;   
     #endif
     LPC_TMR32B0->PR = PRESCALE_US; //Increment TC at every 47999+1 clock cycles
     LPC_TMR32B0->MR0 = PWM_PERIOD-1250;
     LPC_TMR32B0->MR3 = PWM_PERIOD;
     /* Reset on MR0 */
     LPC_TMR32B0->MCR = (1<<10);;         
     LPC_TMR32B0->PWMC = 0x04; //Enable PWM Mode for MAT0.2
  }
  else if ( timer_num == 1 )
  {
     /* Some of the I/O pins need to be clearfully planned if
     you use below module because JTAG and TIMER CAP/MAT pins are muxed. */
     LPC_SYSCON->SYSAHBCLKCTRL |= (1<<10);
     #ifdef __JTAG_DISABLED
     /*  Timer1_32 I/O config */
     /* Timer1_32 CAP0 */
     LPC_IOCON->JTAG_TMS_PIO1_0  &= ~0x07;   
     LPC_IOCON->JTAG_TMS_PIO1_0  |= 0x03;   
     /* Timer1_32 MAT0 */
     LPC_IOCON->JTAG_TDO_PIO1_1  &= ~0x07;   
     LPC_IOCON->JTAG_TDO_PIO1_1  |= 0x03;   
     /* Timer1_32 MAT1 */
     LPC_IOCON->JTAG_nTRST_PIO1_2 &= ~0x07;
     LPC_IOCON->JTAG_nTRST_PIO1_2 |= 0x03;   
     /* Timer1_32 MAT2 */
     LPC_IOCON->ARM_SWDIO_PIO1_3  &= ~0x07;
     LPC_IOCON->ARM_SWDIO_PIO1_3  |= 0x03;   
     #endif
     LPC_TMR32B1->MR0 = TimerInterval;
     /* Interrupt and Reset on MR0 */
     LPC_TMR32B1->MCR = 3;         
     /* Enable the TIMER1 Interrupt */
     NVIC_EnableIRQ(TIMER_32_1_IRQn);
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
