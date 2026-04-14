#include <stdio.h>
#include "type.h"
#include <msp430.h>


static volatile unsigned char  pin[NUM_BUTTONS];
static volatile unsigned char  prev_pin[NUM_BUTTONS];
static volatile unsigned char  btn_state[NUM_BUTTONS];
static volatile unsigned long  break_timer[NUM_BUTTONS];


/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Scan_Buttons(void)
{
  Scan_Rotary_Button();
}
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Scan_Rotary_Button(void)
{
   unsigned char btn;
   for(btn=0;btn<NUM_BUTTONS;btn++)
   {
      /* Read GPIO pin based on button index */
      if(btn == ROT_BTN1)
      {
          pin[btn] = ((P2IN & BIT2)?1u:0u);   //pin 2.2
      }
      else if(btn == ROT_BTN2)
      {
          pin[btn] = ((P1IN & BIT3)?1u:0u);   //pin 1.3
      }

      if (!pin[btn] && prev_pin[btn])
      {
         /* Button pressed */
         break_timer[btn] = 0;
         btn_state[btn]   = 1;
      }
      else if (!pin[btn] && !prev_pin[btn])
      {
         /* Button held low */
         if (btn_state[btn] == 1)
         {
             break_timer[btn]++;
         }
      }
      else if (pin[btn] && !prev_pin[btn])
      {
         /* Button released */
         if (break_timer[btn] > 0)
         {
             btn_state[btn] = 2;
         }
      }
      else if (pin[btn] && prev_pin[btn])
      {
         /* Released and normal run */
         if ((btn_state[btn] == 2) && (break_timer[btn] > 0u) && (break_timer[btn] <= BREAK_TIMEOUT))
         {
             /* Short press */
             break_timer[btn] = 0;
             btn_state[btn]   = 0;
             if(btn == ROT_BTN1)
             {
                Radio_Enc1_Button(SHORT_PRESS);
             }
             else if(btn == ROT_BTN2)
             {
                Radio_Enc2_Button(SHORT_PRESS);
             }
         }
         else if ((btn_state[btn] == 2) && (break_timer[btn] > BREAK_TIMEOUT))
         {
             /* Long press */
             break_timer[btn] = 0;
             btn_state[btn]   = 0;
             if(btn == ROT_BTN1)
             {
                Radio_Enc1_Button(LONG_PRESS);
             }
             else if(btn == ROT_BTN2)
             {
                Radio_Enc2_Button(LONG_PRESS);
             }
         }
      }
      prev_pin[btn] = pin[btn];
   }
}

