//filename:- App_Lin.c
/***********************************************************************************
System Includes
***********************************************************************************/
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

/***********************************************************************************
User Includes
***********************************************************************************/
#include "string.h"
#include "HAL_Init.h"
#include "APP_LIN.h"
#include "INTF_LIN_CONFIG.h"

/**********************************************************************************
Global variables
***********************************************************************************/
/***********************************************************************************
     User Program Code
***********************************************************************************/
/**********************************************************************************
Function Name:  ILIN Receive
Description:    Sensor Data in
Parameters:     none
Return value:   none
***********************************************************************************/
void App_Notify_Frame_Received_USART1(unsigned char id_index)
{
   if(Lin_Rx_Msg[id_index].id == 0x14u)
   {
      Lin_Comm_Count = 0u;
      Set_Led1(1);
      memcpy(Distance_Received,Lin_Rx_Msg[id_index].buf_ptr,Lin_Rx_Msg[id_index].len);
   }
}

/**********************************************************************************
Function Name:  TLIN transmit
Description:    Display data out
Parameters:     none
Return value:   none
***********************************************************************************/
void App_Notify_Frame_Transmitted_USART0(unsigned char id_index)
{
   if(Lin_Tx_Msg[id_index].id == 0x14u)
   {
      Lin_Comm_Count0 = 0u;
      Set_Led2(1);
   }
}


