/*filename :- isr.c*/                                                            

#include "p30f3010.h"
#include "ports.h"
#include "timer.h"
#include "qei.h"
#include <libpic30.h>
#include <string.h>
#include <stdio.h>
#include "system.h"
#include "rotary.h"
#include "bm.h"




static volatile unsigned int Timer_Tick;


/* Define global variables with attributes          */

int variable1 __attribute__((__space__(xmemory)));
int variable2 __attribute__((__space__(ymemory)));


/****** START OF INTERRUPT SERVICE ROUTINES *********/

/* Replace the interrupt function names with the    */
/* appropriate names depending on interrupt source. */

/* The names of various interrupt functions for     */
/* each device are defined in the linker script.    */


/* Interrupt Service Routine 1                      */
/* No fast context save, and no variables stacked   */


/* Interrupt Service Routine 2                      */
/* Fast context save (using push.s and pop.s)       */

/******************************************************************************
* Function Name     : Generates 1ms interrupt
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void)
{
   
   IFS0bits.T1IF = 0; /* clear IF bit */
   Fast_Periodic_Encoder();
   if((Timer_Tick % 5)==0)//5ms
   {
      Task_Activate = ON;
      Task_Now = TASK_5MS;
   }
   if((Timer_Tick % 10) ==0)//10ms
   {
      Task_Activate = ON;
      Task_Now = TASK_10MS;
   }
   if((Timer_Tick % 50)==0)//50ms
   {
      Task_Activate = ON;
      Task_Now = TASK_50MS;
   }
   if((Timer_Tick % 100)==0)//100ms
   {
      Task_Activate = ON;
      Task_Now = TASK_100MS;
   }
   if((Timer_Tick % 1000)==0)//1s
   {
      Task_Activate = ON;
      Task_Now = TASK_1S;
   }
   Timer_Tick++; 
   
        
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__, __shadow__)) _IC1Interrupt(void)
{
   
   IFS0bits.IC1IF = 0; 
   IEC0bits.IC1IE = 0; 


}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__, __shadow__)) _T3Interrupt(void)
{
   
   IFS0bits.T3IF = 0; 
   T3CONbits.TON  = 0;
   IEC0bits.T3IE=0;     
   TMR3 = 0;   
        
 }


/* Interrupt Service Routine 2                      */
/* Fast context save (using push.s and pop.s)       */

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__, __shadow__)) _T2Interrupt(void)
{
   IFS0bits.T2IF = 0; 
}

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__, __shadow__))_U1RXInterrupt(void)
{
   volatile unsigned int rx_data;
   IFS0bits.U1RXIF = 0; 
   rx_data = U1RXREG;
   Bm83_Assemble_Uart_Data((unsigned char)rx_data);
}


/* Interrupt Service Routine 3: INT0Interrupt       */
/* Save and restore variables var1, var2, etc.      */
/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__(__save__(variable1,variable2)))) _INT0Interrupt(void)
{
   IFS0bits.INT0IF = 0;
}
/* Interrupt Service Routine 3: INT2Interrupt       */
/* Save and restore variables var1, var2, etc.      */

/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void __attribute__((__interrupt__(__save__(variable1,variable2)))) _INT2Interrupt(void)
{
   _INT2IF = 0;

}








