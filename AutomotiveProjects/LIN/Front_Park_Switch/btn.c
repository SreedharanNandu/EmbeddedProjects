/*----------------------------------------------------------------------------
 * Name:   btn.c
 * Note(s):
 *----------------------------------------------------------------------------
 *----------------------------------------------------------------------------*/
#include "btn.h"
#include "hal_lin.h"


/**********************************************************************************
Global definition
****************************** *****************************************************/
#define SAMPLE_MS                10U        // must match your scheduler
#define DEBOUNCE_COUNT           5U         // 5 * 10ms = 50ms debounce
#define STUCK_MS                 300000U    // 300000ms = 5 minutes; change as needed
#define ADC_PRESSED_THRESHOLD    300U      // example: ADC value when switch is solidly pressed
#define ADC_RELEASED_THRESHOLD   10U       // example: ADC when released (near zero)
/**********************************************************************************
Global variables
***********************************************************************************/
volatile unsigned char stuck_flag;
volatile BtnState_t debounced_state;
volatile BtnEvent_t  pending_event;
volatile unsigned char stable_count;
volatile unsigned long time_in_state_ms;   // monotonic time in current debounced state

/***********************************************************************************
Function Prototypes
***********************************************************************************/
static unsigned int Read_Adc(void);



/*****************************************************************************
** Function name: Read ADC value 
**
** Descriptions:  
**
** parameters:   None
** Returned value:  
** 
*****************************************************************************/
static unsigned int Read_Adc(void) 
{
   ADCON0 |= 0x02u;                  // Start conversion by setting GO bit
   while (ADCON0 & 0x02u);           // Wait for conversion to complete by checking GO bit
   return ((ADRESH << 8) | ADRESL);  // Combine high and low bytes
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
   unsigned int adcVal;
   BtnRaw_t rawVal;
   BtnState_t new_state;
   BtnState_t prev_state;

   adcVal = Read_Adc();
   if(adcVal > 0x03FFu)
   {
      adcVal = 0u;
   }

   if(adcVal >= ADC_PRESSED_THRESHOLD) 
   {
      rawVal = BTN_RAW_PRESSED;
   }
   else if(adcVal <= ADC_RELEASED_THRESHOLD) 
   {
      rawVal = BTN_RAW_RELEASED;
   }
   else
   {
     //do nothing
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
         if(stable_count < DEBOUNCE_COUNT) 
         {
            stable_count++;
         }

         if(stable_count >= DEBOUNCE_COUNT)
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
   if((!stuck_flag) && (time_in_state_ms >= STUCK_MS))
   {
       stuck_flag = 1u;
       if(debounced_state == BTN_STATE_PRESSED)
       {
          pending_event = BTN_EVENT_STUCK_PRESSED;
       }
       else
       {
          pending_event = BTN_EVENT_STUCK_RELEASED;
       }
   }
   Lin_Data[0].data[0] = debounced_state;
}



