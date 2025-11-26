/******************************************************************************
** Project Name         : 
** File Name            : INTF_VTMR_API.h
** Description          : 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
#ifndef INTF_VTMR_API_H
#define INTF_VTMR_API_H

/******************************************************************************
** P R O J E C T     I N C L U D E S
******************************************************************************/
#include "system_def.h"
/******************************************************************************
**S T A R TG L O B A LD E F I N I T I O N 
******************************************************************************/


/*****************************************************************************
**                  M A C R OD E F I N I T I O N S
******************************************************************************/
#define INTF_VTMR_ID_WAKEUP_UART_MSG              (u32)2000u
#define ENC_MSG_TIMEOUT_MS                        ((u8)30)
#define LIN_ENC_MSG_TIMEOUT_MS                    ((u8)49)
#define ILLUM_SIGNAL_TIMEOUT_MS                   ((u16)1) 
#define INTF_VTMR_ID_VOL_WU_TKNOB_TIMEOUT_MS      ((u32)(2000uL))
#define INTF_VTMR_ID_RF_DEACTV_TIMEOUT            (100u)
#define INTF_VTMR_ID_RF_LED_TIMEOUT               (5u)
#define INTF_VTMR_ID_LB_TIMEOUT                   (5000u)
#define INTF_VTMR_ID_NCF_RST_TIMEOUT              (500u)
#define INTF_VTMR_ID_NFC_CMD_TIMEOUT              (5u)
#define INTF_VTMR_ID_NFC_CS_TIMEOUT               (10u)
#define INTF_VTMR_ID_INIT_FIDM_TIMEOUT            (60u)
#define INTF_VTMR_ID_READ_FIDM_TIMEOUT            (1000u)
/******************************************************************************
**D A T A
******************************************************************************/
typedef enum
{  
  INTF_VTMR_ID_SIMPLE,
  INTF_VTMR_ID_MAX_CNT
}INTF_VTMR_Id_T;  

/* Timer status IDLE or ACTIVE or EXPIRED */
typedef enum
{
   INTF_VTMR_STATUS_IDLE,
   INTF_VTMR_STATUS_ACTIVE,
   INTF_VTMR_STATUS_EXPIRED
          
}INTF_VTMR_Status_T;



  /* Structure for Timer status */      
typedef struct
{
  u8  timer_status;
  u32 timer_count;

}INTF_VTMR_Data_T;


/******************************************************************************
**                 G L O B A LF U N C T I O N S
******************************************************************************/
 
extern void INTF_VTMR_Set_Timer_Status(INTF_VTMR_Id_T p_timer_id_E, INTF_VTMR_Status_T p_status_E);
extern void INTF_VTMR_Set_Timer_Count(INTF_VTMR_Id_T p_timer_id_E , u32 p_count_U32);
extern void INTF_VTMR_Reset_All_Timers(void);
extern INTF_VTMR_Status_T INTF_VTMR_Get_Timer_Status_E(INTF_VTMR_Id_T p_timer_id_E);
extern void INTF_VTMR_PowerUp_Init(void);
extern void INTF_VTMR_Periodic_Task(void);
extern void INTF_VTMR_Start_Timer( INTF_VTMR_Id_T p_timer_id_E ,u32 p_time_ms_U32);
extern void INTF_VTMR_Stop_Timer(INTF_VTMR_Id_T p_timer_ID_E);
 

/******************************************************************************
**E N DG L O B A LD E F I N I T I O N 
******************************************************************************/



/******************************************************************************
**S T A R TL O C A LD E F I N I T I O N 
******************************************************************************/


/******************************************************************************
**                  L O C A LF U N C T I O N S
******************************************************************************/



/******************************************************************************
**E N DL O C A LD E F I N I T I O N 
******************************************************************************/

#endif 

/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No                :
** Date(dd/mm/yy)             :
** Author                     :
** Description of change      :
*******************************************************************************/


