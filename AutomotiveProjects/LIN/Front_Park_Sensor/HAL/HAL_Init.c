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
   HAL_ADC_Init();

   UARTF1_Init();//sensor Rx - P1.0 and P1.1 mapped to ILIN(marked as LIN_TX2 and LIN_RX2 in schematic
   UARTF0_Init();//display Tx - P1.3 and P1.4 mapped to TLIN(marked as LIN_TX1 and LIN_RX1 in schematic

   
   WAKEUPTM_Init();
   HAL_ADC_Start_Operation();

}

