/*****************************************************************************
** File Name    : main.c
** Initial Date :  
** Created by   : 
** Description  : 
** Notes        : 
*****************************************************************************/
#include "types.h"
#include <string.h>
#include "process.h"
#include "hwio.h"
#include "led.h"
#include "uart.h"
#include "buttons.h"
#include "rotary.h"
#include "radio.h"
#include "memory.h"
#include "eeprom.h"

/*Very important Do not delete*/
_CONFIG2(FNOSC_PRIPLL|POSCMOD_XT|I2C1SEL_PRI|PLL96MHZ_ON|PLLDIV_DIV12)


/*Very important Do not delete for CLKDIV*/
static void SwitchSystemClock(void);
 

/*****************************************************************************
* Function   : 
* Created By :    
* Date       :    
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
static void SwitchSystemClock(void)
{
   register uint16_t ClockSwitchTimeout;
   /*
   * At Power On Reset the configuration words set the system clock
   * to use the FRC oscillator. At this point we need to enable the
   * PLL to get the system clock running at 32MHz.
   * 
   * Clock switching on the 24FJ family with the PLL can be a bit tricky.
   * 
   * First we need to check if the configuration words enabled clock
   * switching at all, then turn off the PLL, then setup the PLL and
   * finally enable it. Sounds simple, I know. Make sure you verify this 
   * clock setup on the real hardware.
   */
   if(!OSCCONbits.CLKLOCK) /* if primary oscillator switching is unlocked */
   {
      /* Select primary oscillator as FRC */
      __builtin_write_OSCCONH(0);
       
      /* Request switch primary to new selection */
      __builtin_write_OSCCONL(1);
       
      /* wait, with timeout, for clock switch to complete */
      for(ClockSwitchTimeout=10000; --ClockSwitchTimeout && OSCCONbits.OSWEN;);
       
      CLKDIV = 0x0000; /* set for FRC clock 8MHZ operations */
       
      /* Select primary oscillator as PRI+PLL */
      __builtin_write_OSCCONH(3);
       
      /* Request switch primary to new selection */
      __builtin_write_OSCCONL(1);
      
      /* wait, with timeout, for clock switch to complete */
      for(ClockSwitchTimeout=10000; --ClockSwitchTimeout && OSCCONbits.OSWEN;);
       
      /* wait, with timeout, for the PLL to lock */
      for(ClockSwitchTimeout=10000; --ClockSwitchTimeout && !OSCCONbits.LOCK;);
      
      /* at this point the system oscillator should be 32MHz */
   }
}

/*****************************************************************************
* Function   : 
* Created By :    
* Date       :    
* Overview   : Initialising the Peripherals 
* Argument   : None
* Output     :  
* Note       : Init System required lots of clean up   
*****************************************************************************/
int main(void)
{
   /*This is need else debugger wont connect*/
   int wait4me = 10000u;
   while(wait4me--);

   SwitchSystemClock();
   Init_HWIO(); // Initialize all system peripherals
   Init_Process();
   Turn_On_Timer1();
   while(1)
   {        
      if(_10msCounter >= 10u)  //10ms
      {
         _10msCounter = 0;
         LED_Control();     
         Fast_Periodic_Encoder();
         Scan_Buttons();
      }            
      if(_100msCounter >= 100u) //100ms 
      {
         _100msCounter = 0;
         Process_Commands(); 
         Fast_Periodic_Encoder();
         Process_App_Si();
         Slow_Periodic_Encoder();
         Process_Eeprom();
      }            
      if(_1sCounter >= 1000u) //1s 
      {
         Fast_Periodic_Encoder();
         _1sCounter = 0;
         Start_LED(LED_A,BLINK_ON,1,(unsigned long)50,(unsigned long)3000,TURN_ON,0);
      }            
      Fast_Periodic_Encoder();
      
   }
   return 1;
}


// EOF

