/******************************************************************************
** Project Name         : 
** File Name            : HAL_Init.c
** Description          : This module is used for initialization
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include "HAL_Init.h"





/* Define to include local variables in the header file */
void Init_HW(void)
{

   HAL_SYSCLK_Clock_Init();
   HAL_Port_Init();
   TAU0_Init();
   WDT_Init();

   UARTF1_Init();//not used 
   UARTF0_Init();


   CanInit();

   #if (GMLAN_ENABLED == TRUE)
   App_Trcv_Mode_HVWU_To_Normal();
   #endif

   INTF_VTMR_PowerUp_Init();

   WAKEUPTM_Init();

   RST_NFC(1);     
   FIDM_IRQ(1);    

}

