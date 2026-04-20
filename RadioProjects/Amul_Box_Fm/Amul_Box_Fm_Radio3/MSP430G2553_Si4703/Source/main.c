/*****************************************************************************
filename:- main.c
******************************************************************************/
#include <msp430.h>
#include<msp430g2553.h>
#include "type.h"





#define Toggle_LED()    \
                     P1OUT ^= BIT0; /*Hearbeat LED*/

/*  ---------------Pin Out-----------------
    MSP430  pin12 -  RST      - P2.4  IP   
    MSP430  pin13 -  RSEN     - P2.5  OP   
    MSP430  pin14 -  SCL      - P1.6  OP   
    MSP430  pin15 -  SDA      - P1.7  INOP 
    MSP430  pin9  -  ENC A    - P2.1  IN   
    MSP430  pin8  -  ENC B    - P2.0  IN   
    MSP430  pin10 -  ENC SW   - P2.2  IN   
    MSP430  pin11 -  TBD      - P2.3  IN   
    MSP430  pin2  -  LED_HB   - P1.0  OP   
    MSP430  pin3  -  RXD      - P1.1  IN   
    MSP430  pin4  -  TXD      - P1.2  OP   
   ----------------------------------------
*/

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void main(void)
{
   unsigned int delay_stabilize = 1000u;

   Stop_Watchdog(); 
   // Configure Clock: Set DCO to 16 MHz
   DCOCTL = CALDCO_16MHZ;     // DCO to 16 MHz
   BCSCTL1 = CALBC1_16MHZ;    // Set BCS to 16 MHz
   BCSCTL3 = LFXT1S_2;
   
   while(delay_stabilize--)
   {
     __no_operation();
   }
   InitGPIO();
   Amp_Mute(ON);
   PowerUp_Check();
   PreInit_Radio();
   Init_Radio();
   Init_Encoder();
   InitTimerA1();
   ReadEE();
   Init_Uart(K_Uart_Baud);
   
   __bis_SR_register(GIE); // Enable global interrupts
   Start_Watchdog();
    
   while(1)
   {

      if(_10msCounter >= 10u)  //10ms
      {
         _10msCounter = 0u;
         Fast_Periodic_Encoder();
         Scan_Buttons();
         Led_Task();
         Service_Watchdog();
         // TBD:-Reset the Watchdog Timer to prevent it from resetting the device
      }            
      if(_100msCounter >= 100u) //100ms 
      {
         _100msCounter = 0u;
         Fast_Periodic_Encoder();
         Process_Eeprom();
         Process_App_Si();
         Process_Commands();
         Slow_Periodic_Encoder();
      }            
      if(_1sCounter >= 1000u) //1s 
      {
         Fast_Periodic_Encoder();
         _1sCounter = 0u;
         Toggle_LED();
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
   unsigned char i;

   for(i=0u;i<5u;i++)
   {
      Toggle_LED();
      Delay_Ms(K_Power_Led_Delay);
      Toggle_LED();
      Delay_Ms(K_Power_Led_Delay);
   }
}

/*******************************************************************************
 Func Name    :Sleep
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Enter_Sleep(void) 
{
   Stop_Watchdog();
   __bis_SR_register(LPM3_bits+GIE);
   __no_operation();

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
    Stop_Watchdog();  // Disable the watchdog timer to prevent accidental reset
    WDTCTL = WDTPW + WDTCNTCL; // Clear the watchdog timer

    // Set the Watchdog Timer in "reset mode" (causes a reset after the timeout)
    WDTCTL = WDTPW + WDTSSEL + WDTIS1;  // Select ACLK as the clock source and set the timeout interval (e.g., 32 ms)
    
    // The following line will cause the MSP430 to reset when the WDT overflows
    __bis_SR_register(LPM3_bits);  // Enter low-power mode while waiting for the reset
    while(1);  // Infinite loop to allow reset to occur
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void InitGPIO(void)
{

   P1DIR |= (BIT0);                       // Set P1.0 as output
   P1DIR &= ~(BIT3 + BIT4 + BIT5 );       // Set P1.3, P1.4, P1.5 as inputs
   P1IE  |= BIT3;                         // P1.3 interupt enable
   P1IES |= BIT3;                         // P1.3 Falling edge trigger
   P1IFG &= ~BIT3;                        // P1.3 Clear interrupt flag
   P1SEL |= BIT1 + BIT2;                  // Set RX and TX for UART
   P1SEL2 |= BIT1 + BIT2;                 // Set RX and TX for UART (alternative function)
   P1SEL  &= ~(BIT3 + BIT4 + BIT5);       // Set P1.3, P1.4, P1.5 as gpio
   P1SEL2 &= ~(BIT3 + BIT4 + BIT5);       // Set P1.3, P1.4, P1.5 as gpio
   P2DIR &= ~(BIT0 + BIT1 + BIT2);        // Set P2.0, P2.1, P2.2 as inputs
   P2DIR |= BIT3;                         // Set P2.3 as output
   P1OUT &= (~BIT0);                      // Set p1.0 to LOW

}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void ShutGPIO(void)
{
   P1SEL  &= ~(BIT6 | BIT7);// remove I2C peripheral function
   P1SEL2 &= ~(BIT6 | BIT7);
   P1DIR  &= ~(BIT6 | BIT7);// set as input = hi-Z
   P1OUT  &= ~(BIT6 | BIT7);// P1OUT bits don't matter when DIR=input, but clear them for cleanliness:
   P1OUT  |= BIT0;
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
#pragma vector = PORT1_VECTOR
__interrupt void Port1_ISR(void)
{
    P1IFG &= ~BIT3;                       // Clear P1.3 flag
    __bic_SR_register_on_exit(LPM3_bits); // Wake up exit LPM3
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       :Trap handler - catches any unexpected interrupt.
*                    Forces a WDT reset immediately.                  
******************************************************************************/
#pragma vector = TRAPINT_VECTOR   /* INT00 */
__interrupt void Trap_ISR(void)
{
    Fault(FLT_TRAP,1);
    WDTCTL = WDTPW + WDTCNTCL;   /* kick WDT into reset mode     */
    /* spin - WDT resets within ~32ms */
    while(1);
}


