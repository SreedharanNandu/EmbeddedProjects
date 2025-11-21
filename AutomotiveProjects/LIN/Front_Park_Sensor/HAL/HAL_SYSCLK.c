/******************************************************************************
** Project Name         :
** File Name            : HAL_SYSCLK.C
** Description          : This module is used to configure the system clock 
** Date                 : 
** Hardware Platform    :
******************************************************************************/

/* Define to include local variables in the header file */
#define HAL_SYSCLK_C


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/

#include <CG_macrodriver.h>

#include <system_def.h>
#include "HAL_SYSCLK_API.h"
/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*<list all local macro declarations> */

/******************************************************************************
**               D A T A
******************************************************************************/
/*<list all local variables>  */



/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
/*<list all function definitins here>  */


/******************************************************************************
* Function Name     : HAL_SYSCLK_Clock_Init
* Input Params      : None
* Output Params     : None
* Description        : This function is used to initialize the system clock as External clock.
******************************************************************************/
void HAL_SYSCLK_Clock_Init(void)
{
  
   u8 temp_stabwait;
   
   /*Set for External Oscillator mode and frequency range from 10-20Mhz */
   CMC = (HAL_HIGH_SPEED_X1_OSC|HAL_X1_OSC_GREATER_THAN_10M);

   /*select oscillator stabilization time as (2^18/fx=16MHz) = 16.384msec*/
   OSTS = HAL_OSC_STABILIZATION_TIME_SELECT18;
   
   /*The GDCSC bit is used to select handling of writing the clock operation status control register (CSC).
   Writing the CSC register is valid only when the GDCSC bit is set.*/
   HAL_GUARD_Wrte_Enable_For_CSC_Reg = ENABLE;
   
   /* Clock Operation Status Control Register,X1 oscillator operating to Start*/
   HAL_High_Speed_External_Clock_Stop_Control = CLEAR;
   
   /*Check for the clock stabilization set by OSTS*/
   do
   {
     /*This is the register that indicates the count status of the X1 clock oscillation stabilization time counter*/
     temp_stabwait = HAL_OSC_Stabilization_Time_Status_Reg;
   }
   while (temp_stabwait != HAL_STABILIZATION_TIME_STATUS);/*wait till MOST8 to MOST18 is set*/
   
   
   /*This register is used to control the step-up circuit of the data flash and code flash for high-speed operation.
   
   If the microcontroller operates at a low speed with a system clock of no more than 20 MHz or 10 MHz, the power
   consumption can be lowered by setting this register to the default value, 00H.*/
   
   HAL_Data_Flash_Operating_Freq = HAL_DATA_FLASH_FREQ_LESS_THAN_20M;
   HAL_Code_Flash_Operating_Freq = HAL_DATA_FLASH_FREQ_MORE_THAN_10M;/*16Mhz*/
   
   temp_stabwait = 100u; 
   while(temp_stabwait--);   

   /*This register is used to select the main system clock and CPU/peripheral hardware clock and set a division ratio.*/
   /* High-speed system clock (fMX) as Main clock*/
   HAL_Main_System_Clock_Select = HAL_HIGH_SPEED_SYSTEM_CLOCK_fMX;/*fMAIN = fMX*/
   
   HAL_Sub_clock_Operation_Stop_Control  = 0u;
   /* PLL clock/Clock through (fPLL) */
   HAL_CPU_Peripheral_Clock_Select = HAL_PLL_CLOCK_OR_CLOCK_THROUGH_SELECT;  /*Fclk = Clock through*/
   
   /*Clear Division of PLL clock (fPLL) bits*/
   HAL_System_Clock_Control_Reg &= (u8)~0x07u; 

   /*fCLK = fPLL*/
   HAL_System_Clock_Control_Reg |= 0x00u; 
   
   HAL_High_Speed_Internal_Clock_Stop_Control = 1u;
   /* after writing e CSC register clear the GDCSC bit of the GUARD register to 1 */
   HAL_GUARD_Wrte_Enable_For_CSC_Reg = DISABLE;
  
}
/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No            :
** Date(dd/mm/yy)         :          
** Author                 : 
** Description of change :
**
*******************************************************************************/

