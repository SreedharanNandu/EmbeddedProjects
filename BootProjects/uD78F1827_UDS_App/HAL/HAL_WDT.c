/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_WDT.C
** Description          : This module is used to configure the WDT 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/

/* Define to include local variables in the header file */


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
/*<list all system include files>*/

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/

#include <CG_macrodriver.h>
#include <system_def.h>
#include "HAL_WDT_API.h"
/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*<list all local macro declarations> */

/******************************************************************************
**               D A T A
******************************************************************************/
/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
/******************************************************************************
* Function Name     : WDT_Init
* Input Params      : None
* Output Params     : None
* Description       : This function is used to initialize the Watch dog Timer
******************************************************************************/
void WDT_Init(void)
{
   WDTIMK = 1U;   /* disable INTWDTI interrupt */
   WDTIIF = 0U;   /* clear INTWDTI interrupt flag */
   /* Set INTWDTI low priority */
   WDTIPR1 = 1U;
   WDTIPR0 = 1U;   
   WDTIMK = 0U;   /* enable INTWDTI interrupt */
}
/******************************************************************************
* Function Name     : WDT_Init
* Input Params      : None
* Output Params     : None
* Description       : This function is used to Restart the Watch dog Timer
******************************************************************************/
void WDT_Restart(void)
{
   WDTE = 0xACU;    /* restart watchdog timer */
}
