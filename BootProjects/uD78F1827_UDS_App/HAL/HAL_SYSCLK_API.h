/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name         : HAL_SYSCLK_API.h
** Description         : This module is used to configure the system clock 
** Date                 : 
** Hardware Platform           :
******************************************************************************/
#ifndef HAL_SYSCLK_API_H
#define HAL_SYSCLK_API_H



/******************************************************************************
**   P R O J E C T        I N C L U D E S
******************************************************************************/




/******************************************************************************
**   S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/


/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
//#pragma language=extended
/****************** For Clock Configuration START*********************************************/

#define HAL_HIGH_SPEED_X1_OSC                          0x40u
#define HAL_X1_OSC_GREATER_THAN_10M                    0x01u

#define HAL_OSC_STABILIZATION_TIME_SELECT18            0x07u
#define HAL_STABILIZATION_TIME_STATUS                   0xFFu
#define HAL_High_Speed_External_Clock_Stop_Control        CSC.7
#define HAL_Sub_clock_Operation_Stop_Control            CSC.6
#define HAL_High_Speed_Internal_Clock_Stop_Control        CSC.0
#define HAL_GUARD_Wrte_Enable_For_CSC_Reg             GUARD.0 
#define HAL_OSC_Stabilization_Time_Status_Reg           OSTC
#define HAL_Data_Flash_Operating_Freq                  OSMC.2
#define HAL_Code_Flash_Operating_Freq                  OSMC.0

#define HAL_System_Clock_Control_Reg                CKC
#define HAL_Main_System_Clock_Select                    CKC.4
#define HAL_CPU_Peripheral_Clock_Select                 CKC.6

#define HAL_DATA_FLASH_FREQ_LESS_THAN_20M             0u
#define HAL_DATA_FLASH_FREQ_MORE_THAN_10M             1u
#define HAL_PLL_CLOCK_OR_CLOCK_THROUGH_SELECT         0u
#define HAL_HIGH_SPEED_SYSTEM_CLOCK_fMX               1u


/******************************************************************************
**               D A T A
******************************************************************************/
/*list all global variables*/



/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************
*/

extern void HAL_SYSCLK_Clock_Init(void);

#endif /* for #define _MODULE_NAME_H */


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


