/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "LPC11xx.h"
#include "uart.h"
#include "System.h"
#include "timer16.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */

static tmr_data_t  vtmr[TMR_MAX_CNT];



void Process_Timer(void)
{
   unsigned char id;
   unsigned long  cnt;
  
   tmr_status_t status;

   if(timer16_flag == 1u)
   {
      NVIC_DisableIRQ(TIMER_16_0_IRQn);
      for(id=0; id < TMR_MAX_CNT;id++)
      {
         if(id < (unsigned char)0xff) 
         {
            status = vtmr[id].tmr_status;
            cnt  = vtmr[id].tmr_count;
        
            if(status == TMR_ACTIVE)
            {
               if(cnt < TMR_PERIODIC_RATE_MS)
               {
                  Set_Timer_Status((tmr_id_t)id,(tmr_status_t)TMR_EXPIRED);
               }
               else
               {
                  Set_Timer_Count((tmr_id_t)id ,(cnt - TMR_PERIODIC_RATE_MS));
               }
            }
         } 
      }
      timer16_flag = 0u;
      NVIC_EnableIRQ(TIMER_16_0_IRQn);
   }
}
    
void Start_Timer(tmr_id_t id , unsigned long time)
{
   Set_Timer_Count(id , time);
   Set_Timer_Status( id ,TMR_ACTIVE);
}




tmr_status_t  Get_Timer_Status(tmr_id_t id)
{
   tmr_status_t status; 
   
   status = vtmr[id].tmr_status; 
 
   return(status);
}



void  Set_Timer_Status(tmr_id_t  id , tmr_status_t  status)
{
   vtmr[id].tmr_status = status;
}



void Set_Timer_Count(tmr_id_t id ,unsigned long count)
{
  vtmr[id].tmr_count = count;
}


void Reset_All_Timers(void)
{
   unsigned short  i;
   for(i = 0; i <  TMR_MAX_CNT; i++)
   {
     Set_Timer_Status((tmr_id_t)i, TMR_IDLE);
   }

}



void Stop_Timer(tmr_id_t id)
{
    Set_Timer_Status(id,TMR_IDLE);
}


