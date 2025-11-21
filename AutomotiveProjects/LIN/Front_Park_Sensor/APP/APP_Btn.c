//filename :-App_Btn.c
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/

#include <CG_macrodriver.h>
#include "system_def.h"
#include "HAL_PortPin_API.h"

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
#include "HAL_Timer_API.h"
#include "HAL_Init.h"
#include "HAL_ADC_API.h"
#include "HAL_USART.h"
#include "HAL_Timer_API.h"
#include "App_Main.h"

#include "string.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/
/* ---------- CONFIG ---------- */
#define SAMPLE_MS               10U        // must match your scheduler
#define DEBOUNCE_COUNT          5U         // 5 * 10ms = 50ms debounce
#define STUCK_MS                300000U    // 300000ms = 5 minutes; change as needed

/* ADC thresholds (values in ADC counts) - tune to your resistor divider and ADC range */
#define ADC_PRESSED_THRESHOLD   3000U      // example: ADC value when switch is solidly pressed
#define ADC_RELEASED_THRESHOLD  500U       // example: ADC when released (near zero)


/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
/* ---------- Globals (module scope) ---------- */
volatile u8 stable_count;
volatile u32 time_in_state_ms;   // monotonic time in current debounced state
volatile bool stuck_flag;
volatile BtnState_t debounced_state;

/* Event buffer (single event FIFO) */
volatile BtnEvent_t  pending_event;
/*Adc voltage*/
volatile u16 SwV_in_mV;
volatile u16 PinV_in_mV;
/**************************************************************************
 *       local prototypes
 *************************************************************************/

/******************************************************************************
**      F U N C T I O N      D E F I N I T I O N S
******************************************************************************/


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
void App_Btn_Task_10ms(void)
{
   u16 adcVal;
   BtnRaw_t rawVal;
   BtnState_t new_state;
   BtnState_t prev_state;

   if(HAL_ADC_Read_Counts(&adcVal))
   {
      if(adcVal != 0xFFFFu)
      {
         PinV_in_mV = (u16)((u32)adcVal * 825u)/256u;
         SwV_in_mV = (u16)((u32)adcVal * 93u)/5u;
      }
   }

   if (adcVal >= ADC_PRESSED_THRESHOLD) 
   {
      rawVal = BTN_RAW_PRESSED;
   }
   else if (adcVal <= ADC_RELEASED_THRESHOLD) 
   {
      rawVal = BTN_RAW_RELEASED;
   }
   else
   {
      rawVal = BTN_RAW_UNKNOWN; // in hysteresis band
   }


   if(rawVal == BTN_RAW_PRESSED)
   {
      new_state = BTN_STATE_PRESSED;
   }
   else 
   {
      if(rawVal == BTN_RAW_RELEASED)
      {
         new_state = BTN_STATE_RELEASED;
      }
   }
   
   if(new_state != BTN_STATE_UKNOWN)
   {
      if(new_state == debounced_state)
      {
         /* stable same state -> reset unstable counter and increment time_in_state */
         stable_count = 0;
         time_in_state_ms += SAMPLE_MS;

         /* Clear stuck flag if it was previously set but condition cleared */
         if(stuck_flag)
         {
             // if previously stuck but still same physical reading, keep stuck; allow external clear if needed
             // (Alternatively, you may clear stuck when specific recovery condition happens)
         }
      }
      else
      {
         time_in_state_ms = 0; // sample sequences for new state timing starts now
         /* state differs -> increment stability counter; only commit if stable for DEBOUNCE_COUNT samples */
         if (stable_count < DEBOUNCE_COUNT) 
         {
            stable_count++;
         }

         if (stable_count >= DEBOUNCE_COUNT)
         {
            /* commit state change */
            prev_state = debounced_state;
            debounced_state = new_state;
            stable_count = 0u;
            time_in_state_ms = 0u;
            stuck_flag = 0u; //reset stuck when state moves

            /* generate edge event */
            if((prev_state == BTN_STATE_RELEASED) && (debounced_state == BTN_STATE_PRESSED))
            {
               pending_event = BTN_EVENT_OFF_TO_ON;
            }
            else if((prev_state == BTN_STATE_PRESSED) && (debounced_state == BTN_STATE_RELEASED))
            {
               pending_event = BTN_EVENT_ON_TO_OFF;
            }
         }
      }
   }
   else
   {
       /* In hysteresis band -> do not change counters; treat as 'no decision' */
       // optional: increment time_in_state_ms or not — above approach keeps it unchanged
   }

   /* Stuck detection: if we remain continuously in the same debounced state for STUCK_MS, flag stuck */
   if ((!stuck_flag) && (time_in_state_ms >= STUCK_MS))
   {
       stuck_flag = 1u;
       if (debounced_state == BTN_STATE_PRESSED)
       {
          pending_event = BTN_EVENT_STUCK_PRESSED;
       }
       else
       {
          pending_event = BTN_EVENT_STUCK_RELEASED;
       }
   }
}


/******************************************************************************
* Function Name     : 
* Input Params      : None
* Output Params     : None
* Description       : 
******************************************************************************/
BtnState_t Get_Button_State(void)
{
   BtnState_t tempState = BTN_STATE_UKNOWN;
   if(!stuck_flag)
   {
      tempState = debounced_state;
   }
   return tempState;

}
/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No            :
** Date(dd/mm/yy)         :
** Author                 : 
** Description of change  :
**
 
*******************************************************************************/