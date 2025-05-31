/*****************************************************************************
filename:- timer.c
******************************************************************************/
#include <msp430.h>
#include<msp430g2553.h>
#include "type.h"



volatile unsigned int _1msCounter;
volatile unsigned int _10msCounter;
volatile unsigned int _100msCounter;
volatile unsigned int _1sCounter;

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
unsigned char exitLoop(unsigned char id)
{
   volatile unsigned char status = 0;//means timeout
   static unsigned char cnt = 0;
   static unsigned char prev_id = 0;
   
   if(id != prev_id)
   {
      cnt = 0;
   }
   else
   {
      cnt++;
   }
   if(cnt > MAX_EXIT_LOOP_TH)
   {
      status = 1;
   }
   prev_id = id;
   return status;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Delay_Ms(unsigned int ms) 
{
    unsigned int i=0;
   
   // We will use SMCLK, and the timer will be divided by 8 (dividing the 1 MHz clock to 125kHz)
   TACTL = TASSEL_2 | ID_3 | MC_1 | TACLR;  // Use SMCLK, divide by 8, up mode, clear timer

   // Loop for the number of milliseconds
   for (i = 0; i < ms; i++) 
   {
       TACCR0 = 1999u;  // 2000 cycles for 1 ms (16 MHz / 8 = 2 MHz, so 2000 cycles = 1 ms)
       while(!(TACTL & TAIFG));  // Wait until the timer overflow flag is set
       {
          if(exitLoop(37))
          {
             return;
          }
       }
       TACTL &= ~TAIFG;  // Clear the overflow flag
   }
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void InitTimerA1(void) 
{
   TA1CTL = TASSEL_2 | ID_3 | MC_1 | TACLR ;  // Use SMCLK, divide by 8, up mode, clear timer
   TA1CCTL0 = CCIE;
   // Configure Timer_B0 to generate a 1 ms overflow
   TA1CCR0 = 1999u;  // 2000 cycles for 1 ms
}


/*******************************************************************************
 Func Name    :Timer_A1 CCR0 interrupt service routine
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A1_ISR(void) 
{
   _1msCounter++;
   _10msCounter++;  // Increment millisecond counter every 10 ms interrupt
   _100msCounter++;  // Increment millisecond counter every 100 ms interrupt
   _1sCounter++;  // Increment millisecond counter every 1000 ms interrupt
   if(_100msCounter > 150u)  //possible that 100ms task is not running correctly so wait till 50ms more for recovery
   {
      Software_Reset();
   }
}


