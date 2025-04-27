/******************** (C) COPYRIGHT 2010 Embest Info&Tech Co.,LTD. ************
* File Name          : BUTTON_main.c
* Author             : Wuhan R&D Center, Embest
* Date First Issued  : 01/18/2010
* Description        : BUTTON_main program body.
*******************************************************************************
*******************************************************************************
* History:
* 01/18/2010       : V1.0         initial version
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"         /* LPC11xx Peripheral Registers */
#include "gpio.h"
#include "uart.h"
#include "system.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
volatile unsigned char key_pressed,mode_pressed;
/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void Scan_Buttons(void)
{
   static unsigned char pin,prev_pin;
   static unsigned long break_timer=0;
   static unsigned char btn_state;
   #define BREAK_TIMEOUT 30000u

   pin = GPIOGetValue(PORT3,5);
   if(!pin && prev_pin)
   {
      //button pressed
      break_timer=0;
      btn_state = 1;
   }
   else if(!pin && !prev_pin)
   {
     //button pressed low
      if(btn_state == 1)
      {
        break_timer++; 
      }
   }
   else if(pin && !prev_pin)
   {
     //button release
     btn_state = 4;
   }
   else if(pin && prev_pin)
   {
     //released & normal run
     if((btn_state == 4) && (break_timer <= BREAK_TIMEOUT))
     {
        break_timer = 0;
        btn_state = 0;
        mode_pressed = !mode_pressed;
     }
     else if((btn_state == 4) && (break_timer > BREAK_TIMEOUT))
     {
        break_timer = 0;
        btn_state = 0;
        Execute_Commands('S');
     }
   }
   prev_pin = pin;
}
void Process_Buttons(void)
{
   Scan_Buttons();
   if(mode_pressed)
   {
      Execute_Commands('M');   
      mode_pressed = 0;
   }
}

/**
  * @}
  */ 

/**
  * @}
  */ 

/************* (C) COPYRIGHT 2010 Wuhan R&D Center, Embest *****END OF FILE****/
