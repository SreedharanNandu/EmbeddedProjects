/*----------------------------------------------------------------------------
 * Name:   btn.c
 * Note(s):
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include "btn.h"



volatile unsigned char key_pressed;
volatile unsigned char key_pressed_type;
volatile unsigned char pin_state;
volatile unsigned char prev_pin_state;
volatile unsigned long pin_timer;
volatile unsigned char btn_sm;



/*****************************************************************************
** Function name:  
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
void Init_Btn(void)
{
   key_pressed = 0u;
   pin_timer = 0u;
   pin_state = 0u;
   prev_pin_state = 0u;
   key_pressed_type = 0u;
   btn_sm = 0u;

}
/*****************************************************************************
** Function name:  
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
void App_Btn_Process(void)
{
   pin_state = Read_Info_Pin();//INFO
   if((pin_state == 0u) && (prev_pin_state == 0u))
   {
      btn_sm = 2u;
      pin_timer++;

   }
   else if((pin_state == 1u) && (prev_pin_state == 0u))
   {
      btn_sm = 3u;
      pin_timer++;
   }
   else if((pin_state == 1u) && (prev_pin_state == 1u))
   {
      if((btn_sm == 2u)||(btn_sm == 3u))
      {
         btn_sm = 4u;
         if((pin_timer >= (unsigned long)PIN_MAKE_THR) && (pin_timer < (unsigned long)PIN_MAKE_LONG_THR))
         {
            key_pressed = 1u;
            key_pressed_type = SHORT_PRESS_TYPE;
            pin_timer = 0u;
            btn_sm = 0u;
         }
         else if((pin_timer >= (unsigned long)PIN_MAKE_LONG_THR) && (pin_timer < (unsigned long)PIN_MAKE_STUCK_THR))
         {
            key_pressed = 1u;
            key_pressed_type = LONG_PRESS_TYPE;	
            pin_timer = 0u;
            btn_sm = 0u;
         }
         else if(pin_timer >= (unsigned long)PIN_MAKE_STUCK_THR)
         {
            key_pressed = 1u;
            key_pressed_type = STUCK_TYPE;
            pin_timer = 0u;
            btn_sm = 0u;
         }
      }
      if(pin_timer!=0u)
      {
         pin_timer=0u;
      }              
   }
   else if((pin_state == 0u) && (prev_pin_state == 1u))
   {
      btn_sm = 1u;
      pin_timer++;
   }
   prev_pin_state = pin_state;
}



