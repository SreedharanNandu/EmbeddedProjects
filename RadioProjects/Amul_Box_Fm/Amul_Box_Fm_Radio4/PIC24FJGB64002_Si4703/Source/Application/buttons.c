#include <stdio.h>
#include "types.h"
#include "hardware.h"
#include "buttons.h"
#include "radio.h"

 volatile unsigned char pin[MAX_BTNS],prev_pin[MAX_BTNS];
 volatile unsigned long break_timer[MAX_BTNS];
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
   for(i=0u;i<MAX_BTNS;i++)
   {   
      if(i == VOL_BTN)
      {
         prev_pin[i] = pin[i] = VolRotBtn_Pin();
      }
      else if(i == TUNE_BTN)
      {
         prev_pin[i] = pin[i] = TuneRotBtn_Pin();
      }
      btn_state[i] = 0u;
      break_timer[i] = 0u;
   }

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
   for(i=0u;i<MAX_BTNS;i++)
   {   
      if(i == VOL_BTN)
      {
         pin[i] = VolRotBtn_Pin();
      }
      else if(i == TUNE_BTN)
      {
         pin[i] = TuneRotBtn_Pin();
      }
      if(!pin[i] && prev_pin[i])
      {
         //button pressed
         break_timer[i]=0u;
         btn_state[i] = 1u;
      }
      else if(!pin[i] && !prev_pin[i])
      {
        //button pressed low
         if(btn_state[i] == 1u)
         {
           break_timer[i]++; 
         }
      }
      else if(pin[i] && !prev_pin[i])
      {
        //button release
        if(break_timer[i] > 0u)
        {
           btn_state[i] = 2u;
        }       
      }
      else if(pin[i] && prev_pin[i])
      {
        //released & normal run
        if((btn_state[i] == 2u) && (break_timer[i] > 0u) && (break_timer[i] <= BREAK_TIMEOUT))//short press
        {
           break_timer[i] = 0u;
           btn_state[i] = 0u;
           if(i == VOL_BTN)
           {
              Radio_Enc_Vol_Button(SHORT_PRESS);
           }
           else if(i == TUNE_BTN)
           {
              Radio_Enc_Tune_Button(SHORT_PRESS);
           }
        }
        else if((btn_state[i] == 2u) && (break_timer[i] > BREAK_TIMEOUT))//long press
        {
           break_timer[i] = 0u;
           btn_state[i] = 0u;
           if(i == VOL_BTN)
           {
              Radio_Enc_Vol_Button(LONG_PRESS);
           }
           else if(i == TUNE_BTN)
           {
              Radio_Enc_Tune_Button(LONG_PRESS);
           }
        }
      }
      prev_pin[i] = pin[i];
   }
}
