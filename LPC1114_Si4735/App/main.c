/*
   filename:- main.c
*/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"                        /* LPC11xx definitions */
#include "gpio.h"
#include "i2c.h"
#include "system.h"
#include "timer16.h"
#include "App_Si.h"
#include "App_Si_Intf.h"
#include "App_Btn.h"
#include <string.h>
#include "EEPROM.h"
#include "lcd.h"
#include "rotary.h"

u8 sleep_check;
power_mode_T Power_Mode;

void Init_HWIO(void);
void Init_App(void);


/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
int main(void)
{

   /* Setup the microcontroller system. Initialize the Embedded Flash Interface,  
      initialize the PLL and update the SystemFrequency variable. */



   SystemInit();

   Init_HWIO();
   Init_App();
   Enable_Timer16(0);
   Enable_Timer16(1);


   while (1)
   {
      
      if(timer16_1ms_cnt >= 1u)
      {
         timer16_1ms_cnt = 0u;
         Fast_Periodic_Encoder();
         Fast_Periodic_Encoder1();
      }

      if(timer16_10ms_counter >= 10U)//10ms
      {
         timer16_10ms_counter=0;
         App_Btn_Process();
         App_LCD_Periodic();
         App_Si_Periodic();
      }
      if(timer16_100ms_counter >= 100u)//100ms
      {
         timer16_100ms_counter = 0;
         Slow_Periodic_Encoder();
         Slow_Periodic_Encoder1();
      }

      if(timer16_1s_counter >= 1000u)//1s
      {
         timer16_1s_counter=0u;
      }

      #if SLEEP_ENABLED
      Sleep_Check();
      #endif
   }
}  

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Init_HWIO(void)
{
   GPIOInit();
   Init_Timer16(0, TIME_INTERVAL);
   Init_Timer16(1, TIME_INTERVAL);
   Init_I2C();
}

/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Init_App(void)
{
   Power_Mode = PWR_ON;
#if defined(TMR_USED)
   Reset_All_Timers();
#endif
   Init_Encoder();
   Init_Encoder1();
   Init_Btn();
   Init_LCD();
   Update_Nvm_Radio(READ_DATA);
   Validate_Nvm_Radio();
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Init_Shutdown(void)
{
   
   __disable_irq();
           
   /* Turn on all the IRC & Flash  */
   //LPC_SYSCON->PDRUNCFG &= ~( (1<<0)| (1<<1) | (1<<2)|(1<<3)|(1<<9) ) ;        
   //nandu LPC_SYSCON->PDRUNCFG |=(1<<3);
   
   /* Switch MAINCLKSEL to IRC */
   LPC_SYSCON->MAINCLKSEL = 0;
   LPC_SYSCON->MAINCLKUEN = 0;
   LPC_SYSCON->MAINCLKUEN = 1;
   while (!(LPC_SYSCON->MAINCLKUEN & 0x01));
   
   /* Ensure DPDEN is disabled in the power control register */
   LPC_PMU->PCON    = (1<<11); //Clear DPDFLAG if it was set
   /* Clear the Deep Sleep Flag */
   LPC_PMU->PCON = (1<<8);

   /* All OFF */
   LPC_SYSCON->PDSLEEPCFG =  0x000018FF;

   /* Specify peripherals to be powered up again when returning from deep sleep mode */
   LPC_SYSCON->PDAWAKECFG = LPC_SYSCON->PDRUNCFG;
   
   
   /* Only edge trigger. activation polarity on P0.8 is FALLING EDGE. */
   LPC_SYSCON->STARTAPRP0 &=~(1<<8); ;
   /* Clear all wakeup source */
   LPC_SYSCON->STARTRSRP0CLR |=(1<<8); 
   /* Enable Port 0.8 as wakeup source. */
   LPC_SYSCON->STARTERP0 = (0x1<<8);
   
   NVIC_ClearPendingIRQ(WAKEUP8_IRQn);
   NVIC_EnableIRQ(WAKEUP8_IRQn); // P0.8 as wakeup
   
   SCB->SCR = 4; ;
   
   __enable_irq();
   
   /* Enter Deep Sleep mode */
   __WFI();

   //Zzzzz..should go to sleep here

   SystemInit();
   Init_HWIO();
   Init_App();
   Enable_Timer16(0);
   Enable_Timer16(1);

   
}
/*****************************************************************************
** Function name  : 
** Descriptions   : 
** parameters     : 
** Returned value : 
*****************************************************************************/
void Sleep_Check(void)
{
   if(sleep_check & (LCD_READY|SI_READY))
   {
      Init_Shutdown();
   }
}

