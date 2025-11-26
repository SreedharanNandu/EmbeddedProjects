/******************************************************************************
*******************************************************************************
*** Project Name      : 
** File Name          : INTF_VTMR.C
** Description        : 
** Date               : 
** Hardware Platform  :
******************************************************************************/

/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include "INTF_VTMR_API.h"
#include "INTF_VTMR_Config.h"
#include "system_def.h"

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/


/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

/******************************************************************************
**                       D A T A
******************************************************************************/
static INTF_VTMR_Data_T  fl_vtmr_data_S[INTF_VTMR_ID_MAX_CNT];
/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/


/******************************************************************************
* Function Name  : INTF_VTMR_PowerUp_Init
* Input Params    : None
* Output Params  : None
* Description    : This function is used to reset the all timers 
******************************************************************************/
void INTF_VTMR_PowerUp_Init(void)
{
   /* To reset the all the timers */
   INTF_VTMR_Reset_All_Timers();
}
/******************************************************************************
* Function Name  : INTF_VTMR_Periodic_Task
* Input Params   : None
* Output Params  : None
* Description    : This function is used to set the time delay for timer this 
                   function called from sheduler for Xmsec
******************************************************************************/
void INTF_VTMR_Periodic_Task(void)
{
   u8 l_timer_id_U8;
   u32  l_count_U32;
  
    INTF_VTMR_Status_T l_status_E;

   for(l_timer_id_U8=0; l_timer_id_U8 < INTF_VTMR_ID_MAX_CNT;l_timer_id_U8++)
   {
      if(l_timer_id_U8 < 0xff) 
      {
         /* copying the data from the timer array status to the status  variable */
         l_status_E = (INTF_VTMR_Status_T)fl_vtmr_data_S[l_timer_id_U8].timer_status;
         /* copying the data from the timer array count  to the counter variable */
         l_count_U32  = fl_vtmr_data_S[l_timer_id_U8].timer_count;
     
         if(l_status_E == INTF_VTMR_STATUS_ACTIVE)
         {
            if(l_count_U32 < INTF_VTMR_PERIODIC_RATE_MS)
            {
               /* set the timer satus for virtual timer */
               INTF_VTMR_Set_Timer_Status((INTF_VTMR_Id_T)l_timer_id_U8,INTF_VTMR_STATUS_EXPIRED);
            }
            else
            {
               /* set the timer count for virtual timer */
               INTF_VTMR_Set_Timer_Count((INTF_VTMR_Id_T)l_timer_id_U8 ,(l_count_U32 - INTF_VTMR_PERIODIC_RATE_MS));
            }
         }
      } 
   }
   
}
    
/******************************************************************************
* Function Name  : INTF_VTMR_Start_Timer
* Input Params   : INTF_VTMR_Id_T p_timer_id : To pass the parameter  timer id 
* Output Params  : None
* Description    : This function is used to start the timer
******************************************************************************/
void INTF_VTMR_Start_Timer(INTF_VTMR_Id_T p_timer_id_E , u32 p_time_ms_U32)
{
   /* To set the timer id  and delay for timer  */
   INTF_VTMR_Set_Timer_Count(p_timer_id_E , p_time_ms_U32);
   /* Set the timer id and timer status is active */
   INTF_VTMR_Set_Timer_Status( p_timer_id_E ,INTF_VTMR_STATUS_ACTIVE);
}
/******************************************************************************
* Function Name  : INTF_VTMR_Get_Timer_Status_E
* Input Params   : INTF_VTMR_Id_T p_timer_id : To pass the parameter  timer id 
* Output Params  : INTF_VTMR_Status_T l_timer_status : this variable is 
                   Retuns the timer status
* Description    : This function is used to get the timer status 
******************************************************************************/
INTF_VTMR_Status_T  INTF_VTMR_Get_Timer_Status_E(INTF_VTMR_Id_T p_timer_id_E)
{
   INTF_VTMR_Status_T l_timer_status_E; 
   
   /* To read  the timer status for all  timers */  
   l_timer_status_E = (INTF_VTMR_Status_T)fl_vtmr_data_S[p_timer_id_E].timer_status; 
 
   return(l_timer_status_E);
}
/******************************************************************************
* Function Name  : INTF_VTMR_Set_Timer_Status
* Input Params   : INTF_VTMR_Id_T p_timer_id : To pass the parameter  timer id 
                   INTF_VTMR_Status_T l_timer_status : To pass the timer status
               0 for timer is idle
               1 for timer is active
                   2 for timer is expired
* Output Params   : None
* Description     : This function is used to set the Timer status for ALL IDs
******************************************************************************/
void  INTF_VTMR_Set_Timer_Status(INTF_VTMR_Id_T  p_timer_id_E , INTF_VTMR_Status_T  p_status_E)
{
   /* To set the Timer status for Virtual Timer */
   fl_vtmr_data_S[p_timer_id_E].timer_status = p_status_E;
}
/******************************************************************************
* Function Name   : void HAL_TMR_Set_Timer_Count(VTMR_ID TimerID ,u16 count)
* Input Params    : Input Params    : INTF_VTMR_Id_T p_timer_id : To pass the parameter  timer id 
* Output Params   : None
* Description     : This function is used to set the Timer status for ALL IDs
******************************************************************************/
void INTF_VTMR_Set_Timer_Count(INTF_VTMR_Id_T p_timer_id_E ,u32 p_count_U32)
{
 /* To set the Timer count for virtual Timer */ 
  fl_vtmr_data_S[p_timer_id_E].timer_count = p_count_U32;
}
/******************************************************************************
* Function Name  : VTMR_Reset_All_Timers
* Input Params   : None 
* Output Params  : None
* Description    : This function is used to reset the Timers
******************************************************************************/
void INTF_VTMR_Reset_All_Timers(void)
{
   u16 l_index_U16;
   for(l_index_U16 = 0; l_index_U16 <  INTF_VTMR_ID_MAX_CNT; l_index_U16++)
   {
      /* To reset the all the timers */
      INTF_VTMR_Set_Timer_Status((INTF_VTMR_Id_T)l_index_U16, INTF_VTMR_STATUS_IDLE);
   }

}
/******************************************************************************
* Function Name  : VTMR_Stop_Timer
* Input Params   : INTF_VTMR_Id_T p_timer_id : To pass the parameter  timer id 
* Output Params  : None
* Description    : This function is used to stop the Timer
******************************************************************************/
void INTF_VTMR_Stop_Timer(INTF_VTMR_Id_T p_timer_id_E)
{
    /* To stop the timer  for vitual timer */
    INTF_VTMR_Set_Timer_Status(p_timer_id_E,INTF_VTMR_STATUS_IDLE);
}

/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No                :
** Date(dd/mm/yy)             :
** Author                     :
** Description of change      :
*******************************************************************************/
