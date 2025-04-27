/*----------------------------------------------------------------------------
 * Name:   App_Btn.c
 * Note(s):
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include "LPC11xx.h"
#include "App_Btn.h"
#include "gpio.h"
#include "system.h"



u8 key_pressed[MAX_BTN];
u16 key_pressed_type[MAX_PUSH_BTN];
btn_T btn_state;

static volatile u8 pin_state[MAX_PUSH_BTN];
static volatile u8 prev_pin_state[MAX_PUSH_BTN];
static volatile u32 pin_timer[MAX_PUSH_BTN];
static volatile u8 btn_sm[MAX_PUSH_BTN] = {0,0};



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
   u8 idx;
   btn_state = BTN_INIT;
   for(idx=0;idx<MAX_PUSH_BTN;idx++)
   {
      key_pressed[idx] = 0;
      pin_timer[idx] = 0;
      pin_state[idx] = 0;
      prev_pin_state[idx] = 0;
      key_pressed_type[idx] = 0;
      btn_sm[idx] = 0;

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
   static volatile u8 idx;
   switch(btn_state)
   {

      case BTN_INIT:
           if(Power_Mode == PWR_ON)
           {
              btn_state = BTN_RUNNING;
           }
           break;
      case BTN_RUNNING:
           switch(idx)
           {
              case 0:
                   pin_state[0] = Read_Info_Pin();//INFO
                   break;
              case 1:
                   pin_state[1] = Read_Band_Pin();//BAND
                   break;
           }       
           if((pin_state[idx] == 0) && 
              (prev_pin_state[idx] == 0))
           {
              if(btn_sm[idx] == 1)
              {
                 btn_sm[idx] = 2;
              }
              pin_timer[idx]++;
           }
           else if((pin_state[idx] == 1) && 
                   (prev_pin_state == 0))
           {
              btn_sm[idx] = 3;
              pin_timer[idx]++;
           }
           else if((pin_state[idx] == 1) && 
                   (prev_pin_state[idx] == 1))
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
           }
           else if((pin_state[idx] == 0) && 
                   (prev_pin_state[idx] == 1))
           {
              if(btn_sm[idx] == 0 )
              {
                 btn_sm[idx] = 1;
                 pin_timer[idx]++;
              }
           }
           else
           {
              //do nothing
           }
           if((pin_timer[idx] >= (u32)PIN_MAKE_THR) && (pin_timer[idx] < (u32)PIN_MAKE_LONG_THR))
           {
              if(btn_sm[idx] == 4)//short press,release
              {
                 key_pressed[idx] = 1;
                 key_pressed_type[idx] = SHORT_PRESS_TYPE;
                 pin_timer[idx] = 0;
                 btn_sm[idx] = 0;
              }
           }
           else if((pin_timer[idx] >= (u32)PIN_MAKE_LONG_THR) && (pin_timer[idx] < (u32)PIN_MAKE_STUCK_THR))
           {
              if(btn_sm[idx] == 4)//long press,release
              {
                 key_pressed[idx] = 1;
                 key_pressed_type[idx] = LONG_PRESS_TYPE;	
                 pin_timer[idx] = 0;
                 btn_sm[idx] = 0;
              }
           }
           else if(pin_timer[idx] >= (u32)PIN_MAKE_STUCK_THR)
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
           if(Power_Mode == PWR_OFF)
           {
              btn_state = BTN_SHUT_STATE;
           }
           break;
      case BTN_SHUT_STATE:
           if(Power_Mode == PWR_OFF)
           {
              btn_state = BTN_INIT;
              for(idx=0;idx<MAX_BTN;idx++)
              {
                 key_pressed[idx] = 0;
                 pin_timer[idx] = 0;
                 pin_state[idx] = 0;
                 prev_pin_state[idx] = 0;
                 btn_sm[idx] = 0;
              }
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


