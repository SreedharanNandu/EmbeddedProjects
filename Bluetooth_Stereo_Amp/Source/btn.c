/*----------------------------------------------------------------------------
 * Name:   btn.c
 * Note(s):
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include "btn.h"
#include "system.h"



unsigned char key_pressed[MAX_PUSH_BTN];
unsigned short int key_pressed_type[MAX_PUSH_BTN];
btn_T btn_state;

volatile unsigned char pin_state[MAX_PUSH_BTN];
volatile unsigned char prev_pin_state[MAX_PUSH_BTN];
volatile unsigned long pin_timer[MAX_PUSH_BTN];
volatile unsigned long pin_idle_timer[MAX_PUSH_BTN];
volatile unsigned char btn_sm[MAX_PUSH_BTN] = {0,0};



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
   unsigned char idx;
   btn_state = BTN_INIT;
   for(idx=0;idx<MAX_PUSH_BTN;idx++)
   {
      key_pressed[idx] = 0;
      pin_timer[idx] = 0;
      pin_state[idx] = 0;
      prev_pin_state[idx] = 0;
      key_pressed_type[idx] = 0;
      btn_sm[idx] = 0xFF;

   }

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
   static volatile unsigned char idx;
   switch(btn_state)
   {

      case BTN_INIT:
           btn_state = BTN_RUNNING;
           break;
      case BTN_RUNNING:
           switch(idx)
           {
              case 0:
                   pin_state[0] = Read_Info_Pin();//INFO
                   break;
              case 1:
                   pin_state[1] = Read_Pwr_Pin();//PWR
                   break;
           }       
           if((pin_state[idx] == 0) && (prev_pin_state[idx] == 0))
           {
              btn_sm[idx] = 2;
              pin_timer[idx]++;
              pin_idle_timer[idx] = 0;
           }
           else if((pin_state[idx] == 1) && (prev_pin_state[idx] == 0))
           {
              btn_sm[idx] = 3;
              pin_timer[idx]++;
              pin_idle_timer[idx] = 0;
           }
           else if((pin_state[idx] == 1) && (prev_pin_state[idx] == 1))
           {
              if((btn_sm[idx] == 2)||(btn_sm[idx] == 3))
              {
                 btn_sm[idx] = 4;
              }
              else if(btn_sm[idx] == 0)
              {
                 if(pin_timer[idx]!=0u)
                 {
                    pin_timer[idx]=0;
                 }              
              }
              if(pin_idle_timer[idx] < PIN_MAKE_LONG_THR)
              {
                 pin_idle_timer[idx]++;
              }
           }
           else if((pin_state[idx] == 0) && (prev_pin_state[idx] == 1))
           {
              pin_idle_timer[idx] = 0;
              btn_sm[idx] = 1;
              pin_timer[idx]++;
           }
           else
           {
              //do nothing
           }
           
           
           if((pin_idle_timer[idx] >= (unsigned long)PIN_MAKE_THR) && (pin_idle_timer[idx] < (unsigned long)PIN_MAKE_LONG_THR))
           {
              if(idx == PWR_BTN)
              {
                 if(btn_sm[idx] == 4)//hi level triggered
                 {
                    if(key_pressed[idx])
                    {
                       key_pressed[idx] = 0;
                       key_pressed_type[idx] = LONG_PRESS_TYPE;
                       pin_idle_timer[idx] = PIN_MAKE_LONG_THR;
                       btn_sm[idx] = 0xFF;
                    }
                 }
              }
           }
           if((pin_timer[idx] >= (unsigned long)PIN_MAKE_THR) && (pin_timer[idx] < (unsigned long)PIN_MAKE_LONG_THR))
           {
              if(idx == PWR_BTN)
              {
                 if(btn_sm[idx] == 2)//low level triggered
                 {
                    key_pressed[idx] = 1;
                    key_pressed_type[idx] = LONG_PRESS_TYPE;
                    pin_timer[idx] = 0;
                    pin_idle_timer[idx] = 0;
                    btn_sm[idx] = 0;
                 }
              }
              else if(idx == INFO_BTN)
              {
                 if(btn_sm[idx] == 4)//short press,release , edge triggered
                 {
                    key_pressed[idx] = 1;
                    key_pressed_type[idx] = SHORT_PRESS_TYPE;
                    pin_timer[idx] = 0;
                    pin_idle_timer[idx] = 0;
                    btn_sm[idx] = 0;
                 }
              }
              else
              {
              
              }
           }
           else if((pin_timer[idx] >= (unsigned long)PIN_MAKE_LONG_THR) && (pin_timer[idx] < (unsigned long)PIN_MAKE_STUCK_THR))
           {
              if(btn_sm[idx] == 4)//long press,release
              {
                 key_pressed[idx] = 1;
                 key_pressed_type[idx] = LONG_PRESS_TYPE;	
                 pin_timer[idx] = 0;
                 btn_sm[idx] = 0;
              }
           }
           else if(pin_timer[idx] >= (unsigned long)PIN_MAKE_STUCK_THR)
           {
              if(btn_sm[idx] == 4)//stuck,release
              {
                 key_pressed[idx] = 0;
                 key_pressed_type[idx] = STUCK_TYPE;
                 pin_timer[idx] = 0;
                 btn_sm[idx] = 0;
              }
           }
           else
           {
              //do nothing
           }
           prev_pin_state[idx] = pin_state[idx];
           idx++;
           if(idx == MAX_PUSH_BTN)
           {
              idx = 0;
           }
           break;
      default:     
           break;
   }
}

/*-----------------------------------------------------------------------------
 * Version      Description                       Name       Date
 *
 *  1.0         Created initial version           Nandu      02-04-2025
 *---------------------------------------------------------------------------*/


