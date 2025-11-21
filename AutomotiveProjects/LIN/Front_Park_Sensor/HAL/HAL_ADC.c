/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_ADC.C
** Description          : This module is used to configure the PWM 
** Date                 : 
** Hardware Platform    :  
******************************************************************************/



/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
/*<list all system include files>*/

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/

#include <CG_macrodriver.h>

#include "system_def.h"
#include "HAL_ADC_API.h"

/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

/******************************************************************************
**               D A T A
******************************************************************************/

/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
/*<list all function definitins here>  */

/******************************************************************************
* Function Name     : HAL_ADC_Init
* Input Params      : None
* Output Params     : None
* Description       : This function is used to initialize the ADC Register
******************************************************************************/
void HAL_ADC_Init(void)
{
   ADCEN = 1U;   /* supply AD clock */
}
/******************************************************************************
* Function Name     : HAL_ADC_Start_Operation
* Input Params      : None
* Output Params     : None
* Description       : This function is used start the ADC operation .
******************************************************************************/
void HAL_ADC_Start_Operation(void)
{
   ADIF = 0u;   /* clear INTAD interrupt flag */
   ADCS = 1u;   /* enable AD conversion */
}
/******************************************************************************
* Function Name     : HAL_Stop_ADC_Operation
* Input Params      : None
* Output Params     : None
* Description       : This function is used to stop the ADC operation.
******************************************************************************/
void HAL_Stop_ADC_Operation(void)
{
   ADCS = 0U;    /* disable AD conversion */
   ADIF = 0U;    /* clear INTAD interrupt flag */
}
/******************************************************************************
* Function Name     : HAL_ADC_Read_Channel_Counts
* Input Params      : None
* Output Params     : None
* Description       : This function is used to get the ADC counts from ADC channel0
******************************************************************************/
void HAL_ADC_Read_Channel_Counts(u16 * buffer)
{
  
   u8  adchan = ADS;
   if(ADCHANNEL0 == adchan)
   {
      *buffer = (ADCR0>>6); 
   }
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function selects analog input channel.
**
**  Parameters:
**   channel: analog input channel
**
**  Returns:
**   MD_OK
**   MD_ARGERROR
**
**-----------------------------------------------------------------------------
*/
void HAL_ADC_Select_Channel(enum ADChannel channel)
{
   ADS = (u8)channel;
}
/******************************************************************************
* Function Name     : HAL_ADC_Read_Counts
* Input Params      : None
* Output Params     : None
* Description       : This function is used to ADC conversion.
******************************************************************************/
u8 HAL_ADC_Read_Counts(u16 *adcValptr)
{
   u8 returnVal = 0u;
   if(ADIF)
   {
      HAL_ADC_Read_Channel_Counts(adcValptr);
      ADIF = 0;
      HAL_ADC_Select_Channel(ADCHANNEL0);
      returnVal = 1u;
   }
   else
   {
      *adcValptr = 0xFFFFu;
   }
   return returnVal;
}
/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No            :
** Date(dd/mm/yy)         :          
** Author                 :
** Description of change :
*******************************************************************************/

