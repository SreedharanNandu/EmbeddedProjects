#include <stdio.h>
#include "APP_Buttons.h"
#include "APP_Si.h"
#include "gpio.h"
#include "App_Led.h"
#include "Cal_Const.h"

volatile unsigned char btn_pin[MAX_BTNS],prev_btn_pin[MAX_BTNS];
volatile unsigned long btn_break_timer[MAX_BTNS];
volatile unsigned char btn_state[MAX_BTNS];


/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Buttons(void)
{
   unsigned char i;
   for(i=0u;i<(unsigned char)MAX_BTNS;i++)
   {
      if(i == TUNE_BTN)
      {
         prev_btn_pin[i] = btn_pin[i] = GPIOGetValue(0u,8u);
      }
      else if(i == PWR_BTN)
      {
         prev_btn_pin[i] = btn_pin[i] = GPIOGetValue(0u,9u);
      }
   }
   btn_break_timer[i] = 0u;
   btn_state[i] = 0u;
}


/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Scan_Buttons(void)
{
   unsigned char i;

   for(i=0u;i<(unsigned char)MAX_BTNS;i++)
   {
      if(i == TUNE_BTN)
      {
         btn_pin[i] = GPIOGetValue(0u,8u);
      }
      else if(i == PWR_BTN)
      {
         btn_pin[i] = GPIOGetValue(0u,9u);
      }
      if(!btn_pin[i] && prev_btn_pin[i])//H to L
      {
         //button pressed
         btn_break_timer[i]=0u;
         btn_state[i] = 1u;
      }
      else if(!btn_pin[i] && !prev_btn_pin[i])// Low level
      {
        //button pressed low
         if(btn_state[i] == 1u)
         {
           btn_break_timer[i]++; 
         }
      }
      else if(btn_pin[i] && !prev_btn_pin[i])//L to H
      {
        //button release
        btn_state[i] = 4u;
      }
      else if(btn_pin[i] && prev_btn_pin[i])//High Level
      {
        //released & normal run
        if((btn_state[i] == 4u) && (btn_break_timer[i] > 0u) && (btn_break_timer[i] <= K_Break_Timeout))//short press
        {
           btn_break_timer[i] = 0u;
           btn_state[i] = 0u;
           if(i == TUNE_BTN)
           {
              Radio_Enc_Tune_Button(SHORT_PRESS);
           }
           else if(i == PWR_BTN)
           {
              Radio_Enc_Pwr_Button(SHORT_PRESS);
           }
        }
        else if((btn_state[i] == 4u) && (btn_break_timer[i] > K_Break_Timeout))//long press
        {
           btn_break_timer[i] = 0u;
           btn_state[i] = 0u;
           if(i == TUNE_BTN)
           {
              Radio_Enc_Tune_Button(LONG_PRESS);
           }
           else if(i == PWR_BTN)
           {
              Radio_Enc_Pwr_Button(LONG_PRESS);
           }
        }
      }
      prev_btn_pin[i] = btn_pin[i];
   }
}
