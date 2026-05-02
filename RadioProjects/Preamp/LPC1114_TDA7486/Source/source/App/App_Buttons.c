/*filename:-App_Buttons.c
 *
 * Hardware: 4 x 10k resistors in series (VDD -> R1 -> R2 -> R3 -> R4 -> GND)
 *           Each switch shorts its junction node to GND when pressed.
 *           ADC channel 0 / AD0 (PIO0_11, pin 4) reads the ladder voltage.
 *
 * LPC1114 ADC: 10-bit, VREF = VDD  =>  full scale = 1023 counts
 *
 *   No press  : ADC ~ 1023   (> THR_NONE  900)
 *   BTN_1 SW1 : ADC ~  768   (640 .. 900)   top node    = 3/4 VDD
 *   BTN_2 SW2 : ADC ~  512   (384 .. 640)   mid node    = 1/2 VDD
 *   BTN_3 SW3 : ADC ~  256   (128 .. 384)   lower node  = 1/4 VDD
 *   BTN_4 SW4 : ADC ~    0   (  0 .. 128)   bottom node = 0 V
 */

#include <stdio.h>
#include "APP_Buttons.h"
#include "adc.h"
#include "App_Led.h"
#include "Cal_Const.h"
#include "App_Audio.h"
/*-----------------------------------------------------------------------------
                            ADC DECODE THRESHOLDS  (midpoint values)
  ---------------------------------------------------------------------------*/
#define ADC_THR_NONE   800u   /* above this  -> no button pressed            */
#define ADC_THR_1      700u   /*  BTN_1  (764 counts)         */
#define ADC_THR_2      600u   /*  BTN_2  (679 counts)         */
#define ADC_THR_3      500u   /*  BTN_3  (509 counts)         */
#define ADC_THR_4        0u   /*  BTN_4  (  0 counts)         */

/*-----------------------------------------------------------------------------
                            MODULE VARIABLES
  ---------------------------------------------------------------------------*/
volatile unsigned char btn_pin[MAX_BTNS];
volatile unsigned char prev_btn_pin[MAX_BTNS];
volatile unsigned long btn_break_timer[MAX_BTNS];
volatile unsigned char btn_state[MAX_BTNS];
volatile unsigned short Adc_Pin_Val;

static void App_Press_Btn1(Btn_Trigger_T trigger);
static void App_Press_Btn2(Btn_Trigger_T trigger);
static void App_Press_Btn3(Btn_Trigger_T trigger);
static void App_Press_Btn4(Btn_Trigger_T trigger);

void (*const app_btn_press[MAX_BTNS])(Btn_Trigger_T trigger) = 
{
   App_Press_Btn1,
   App_Press_Btn2,
   App_Press_Btn3,
   App_Press_Btn4

};


/*-----------------------------------------------------------------------------
                            LOCAL PROTOTYPE
  ---------------------------------------------------------------------------*/
static void ADC_DecodeButtons(unsigned short adc_val);

/*******************************************************************************
 Func Name    : ADC_DecodeButtons
 Arguments    : adc_val - raw 10-bit ADC reading
 Return       : none
 Description  : Converts the ADC ladder voltage into virtual btn_pin[] states.
                0 = pressed (active-low), 1 = released, matching the original
                GPIO convention.  Only the topmost pressed switch is decoded.
*******************************************************************************/
static void ADC_DecodeButtons(unsigned short adc_val)
{
   unsigned char i;

   /* Default all buttons to released */
   for (i = 0u; i < (unsigned char)MAX_BTNS; i++)
   {
      btn_pin[i] = 1u;
   }

   if (adc_val > ADC_THR_NONE)
   {
      /* No button pressed - leave all released */
   }
   else if (adc_val > ADC_THR_1)
   {
      btn_pin[BTN_1] = 0u;
   }
   else if (adc_val > ADC_THR_2)
   {
      btn_pin[BTN_2] = 0u;
   }
   else if (adc_val > ADC_THR_3)
   {
      btn_pin[BTN_3] = 0u;
   }
   else if (adc_val == ADC_THR_4)
   {
      btn_pin[BTN_4] = 0u;
   }
}

/*******************************************************************************
 Func Name    : Init_Buttons
 Arguments    : none
 Return       : none
 Description  : Initialises the ADC driver and captures the initial button state.
*******************************************************************************/
void Init_Buttons(void)
{
   unsigned char  i;
   unsigned short adc_val;

   ADC_Init();   /* configured in adc.c - channel 4, PIO0_11 */

   adc_val = ADC_Read(ADC_BTN_CHANNEL);
   ADC_DecodeButtons(adc_val);

   for (i = 0u; i < (unsigned char)MAX_BTNS; i++)
   {
      prev_btn_pin[i]    = btn_pin[i];
      btn_break_timer[i] = 0u;
      btn_state[i]       = 0u;
   }
}

/*******************************************************************************
 Func Name    : Scan_Buttons
 Arguments    : none
 Return       : none
 Description  : Called every 10 ms.  Reads the ADC ladder, decodes btn_pin[],
                then runs the edge-detect / timer state machine.
*******************************************************************************/
void Scan_Buttons(void)
{
   unsigned char     i;
   unsigned short adc_val;
   
   /* --- 1. Sample the resistor ladder ------------------------------------ */
   Adc_Pin_Val = adc_val = ADC_Read(ADC_BTN_CHANNEL);
   ADC_DecodeButtons(adc_val);

   /* --- 2. State machine ------------------------------------------------- */
   for (i = 0u; i < (unsigned char)MAX_BTNS; i++)
   {
      if (!btn_pin[i] && prev_btn_pin[i])            /* H -> L : press edge */
      {
         btn_break_timer[i] = 0u;
         btn_state[i]       = 1u;
         app_btn_press[i](BTN_FALLING_EDGE);
      }
      else if (!btn_pin[i] && !prev_btn_pin[i])      /* L     : held down   */
      {
         if (btn_state[i] == 1u)
         {
            btn_break_timer[i]++;
         }
         app_btn_press[i](BTN_LEVEL_LOW);
      }
      else if (btn_pin[i] && !prev_btn_pin[i])       /* L -> H : release    */
      {
         btn_state[i] = 4u;
         app_btn_press[i](BTN_RISING_EDGE);
         if ((btn_state[i] == 4u) &&
             (btn_break_timer[i] >  0u) &&
             (btn_break_timer[i] <= K_Break_Timeout))        /* SHORT press  */
         {
            btn_break_timer[i] = 0u;
            btn_state[i]       = 0u;
            app_btn_press[i](BTN_LEVEL_HIGH);
         }
         else if ((btn_state[i] == 4u) &&
                  (btn_break_timer[i] > K_Break_Timeout))    /* LONG press   */
         {
            btn_break_timer[i] = 0u;
            btn_state[i]       = 0u;
            app_btn_press[i](BTN_LEVEL_HIGH);
         }
      }
      else if (btn_pin[i] && prev_btn_pin[i])        /* H     : idle        */
      {
      }

      prev_btn_pin[i] = btn_pin[i];
   }
}

/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
static void App_Press_Btn1(Btn_Trigger_T trigger)
{
   if(trigger == BTN_RISING_EDGE)
   {
    }      
   if(trigger == BTN_FALLING_EDGE)
   {
      AudioCtrl_ButtonUp();
   }   
   if(trigger == BTN_LEVEL_LOW)
   {

   }   
}

/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
static void App_Press_Btn2(Btn_Trigger_T trigger)
{
   if(trigger == BTN_RISING_EDGE)
   {
   }      
   if(trigger == BTN_FALLING_EDGE)
   {
      AudioCtrl_ButtonDown();
   }   
   if(trigger == BTN_LEVEL_LOW)
   {
   }   

}
/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
static void App_Press_Btn3(Btn_Trigger_T trigger)
{
   if(trigger == BTN_RISING_EDGE)
   {
   }      
   if(trigger == BTN_FALLING_EDGE)
   {
      AudioCtrl_ButtonMenu();
   }   
   if(trigger == BTN_LEVEL_LOW)
   {
   }   

}
/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
static void App_Press_Btn4(Btn_Trigger_T trigger)
{
   if(trigger == BTN_RISING_EDGE)
   {
   }      
   if(trigger == BTN_FALLING_EDGE)
   {
      AudioCtrl_ButtonEffects();
   }   
   if(trigger == BTN_LEVEL_LOW)
   {
   }   

}
