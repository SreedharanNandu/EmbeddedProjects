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
#include <io78f1827_48.h>
#include <io78f1827_48_ext.h>
#include "system_def.h"
#include "HAL_ADC_API.h"

/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

#define HAL_SLAVE_BATTERY_VOLTAGE 2
/******************************************************************************
**               D A T A
******************************************************************************/
u16 fl_ADC_Counts_u16[4]={0u,0u,0u,0u};

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
   ADCEN = 0U;   /* supply AD clock */
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
   switch(adchan)
   {
    case  ADCHANNEL1:
          *buffer = (ADCR1>>6); 
          break;
    case  ADCHANNEL2:
          *buffer = (ADCR2>>6); 
          break;
    case  ADCHANNEL4:
          *buffer = (ADCR4>>6); 
          break;
    case  ADCHANNEL5:
          *buffer = (ADCR5>>6); 
          break;
    default:
          break;
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
* Function Name     : Task_ADC_Convert
* Input Params      : None
* Output Params     : None
* Description       : This function is used to ADC conversion.
******************************************************************************/
void Task_ADC_Convert(void)
{
   static u8 ad_sel_cnt=0;
   
   switch(ad_sel_cnt)
   {
   
      case 0:
            if(ADIF)
            {
               HAL_ADC_Read_Channel_Counts(&fl_ADC_Counts_u16[0]);
               ad_sel_cnt = 1;
               ADIF = 0;
               HAL_ADC_Select_Channel(ADCHANNEL1);
            }
            break;
      case 1:
            if(ADIF)
            {
               HAL_ADC_Read_Channel_Counts(&fl_ADC_Counts_u16[1]);
               ad_sel_cnt = 2;
               ADIF = 0;
               HAL_ADC_Select_Channel(ADCHANNEL2);
            }
            break;
      case 2:
            if(ADIF)
            {
               HAL_ADC_Read_Channel_Counts(&fl_ADC_Counts_u16[2]);
               ad_sel_cnt = 3;
               ADIF = 0;
               HAL_ADC_Select_Channel(ADCHANNEL4);
            }
            break;
      case 3:
            if(ADIF)
            {
               HAL_ADC_Read_Channel_Counts(&fl_ADC_Counts_u16[3]);
               ad_sel_cnt = 0;
               ADIF = 0;
               HAL_ADC_Select_Channel(ADCHANNEL5);
            }
            break;
      default:
            break; 
   }            
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

