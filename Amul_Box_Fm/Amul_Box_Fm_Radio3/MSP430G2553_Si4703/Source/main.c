/*****************************************************************************
filename:- main.c
******************************************************************************/
#include <msp430.h>
#include<msp430g2553.h>
#include "type.h"

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void main(void)
{
   unsigned int delay_stabilize = 1000u;
   
   WDTCTL = WDTPW + WDTHOLD;

   // Configure Clock: Set DCO to 16 MHz
   DCOCTL = CALDCO_16MHZ;     // DCO to 16 MHz
   BCSCTL1 = CALBC1_16MHZ;    // Set BCS to 16 MHz

   while(delay_stabilize--)
   {
     __no_operation();
   }

   // Set MSP430 pin2 P1.0 as output (LED)
   P1DIR |= (BIT0);    

   // Ensure the LED is off initially
   P1OUT &= (~BIT0);

   // Configure UART pins (P1.1 as RX, P1.2 as TX)
   P1SEL |= BIT1 + BIT2;       // Select RX and TX for UART
   P1SEL2 |= BIT1 + BIT2;      // Select RX and TX for UART (alternative function)
   
   /*
     MSP430 pin10 - touch  button i/p - P2.3
     MSP430 pin10 - rot button i/p - P2.2
     MSP430 pin9 - enc A i/p - P2.1
     MSP430 pin8 - enc B i/p - P2.0
    */
   
   //Configure P2.0, P2.1, P2.2, P2.3 as input (high impedance mode)
   P2DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3); // Set P2.0, P2.1, P2.2, P2.3 as inputs
   PowerUp_Check();
   Init_Encoder();
   Init_Radio();
   InitTimerA1();
   ReadEE();
   Init_Uart(9600u);
   __bis_SR_register(GIE);     // Enable global interrupts
   while(1)
   {
      if(_10msCounter >= 10u)  //10ms
      {
         _10msCounter = 0;
         Fast_Periodic_Encoder();
         Scan_Buttons();
         Led_Task();
         // Reset the Watchdog Timer to prevent it from resetting the device
   
      }            
      if(_100msCounter >= 100u) //100ms 
      {
         _100msCounter = 0;
         Fast_Periodic_Encoder();
         Process_Eeprom();
         Process_App_Si();
         Process_Commands();
         Slow_Periodic_Encoder();
      }            
      if(_1sCounter >= 1000u) //1s 
      {
         Fast_Periodic_Encoder();
         _1sCounter = 0;
         P1OUT ^= BIT0; //hearbeat led
      }            
      Fast_Periodic_Encoder();
   }
}
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void PowerUp_Check(void)
{
   unsigned char i=0;

   for(i=0;i<5;i++)
   {
      P1OUT ^= BIT0;
      Delay_Ms(250u);
      P1OUT ^= BIT0;
      Delay_Ms(250u);
   }
}

/*******************************************************************************
 Func Name    :Timer_A1 CCR0 interrupt service routine
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Software_Reset(void) 
{
    // Configure the Watchdog Timer to trigger a reset
    WDTCTL = WDTPW + WDTHOLD;  // Disable the watchdog timer to prevent accidental reset
    WDTCTL = WDTPW + WDTCNTCL; // Clear the watchdog timer

    // Set the Watchdog Timer in "reset mode" (causes a reset after the timeout)
    WDTCTL = WDTPW + WDTSSEL + WDTIS1;  // Select ACLK as the clock source and set the timeout interval (e.g., 32 ms)
    
    // The following line will cause the MSP430 to reset when the WDT overflows
    __bis_SR_register(LPM3_bits);  // Enter low-power mode while waiting for the reset
    while(1);  // Infinite loop to allow reset to occur
}


