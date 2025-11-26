/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_Timer0.C
** Description          : This module is used to configure the Timer 
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
#include "HAL_Timer_API.h"

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
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function initializes the WAKEUPTM module.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void WAKEUPTM_Init(void)
{
   WUTEN = 1U;   /* supply input clock */
   WUTMCE = 0U;   /* disable WUTM operation */
   WUTMMK = 1U;   /* disable INTWUTM interrupt */
   WUTMIF = 0U;   /* clear INTWUTM interrupt flag */
   
   /* Set INTWUTM low priority */
   WUTMPR1 = 1U;
   WUTMPR0 = 1U;
   WUTMCKE = 1U;   /* enable WAKEUPTM operation clock */
   PCKSEL |= _02_WAKEUPTM_INTERNAL_CLOCK2;//16Mhz/256 = 16usec
   WUTMCMP = _0137_WAKEUPTM_WUTMCMP_VALUE;//5msec

   WAKEUPTM_Start();

}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts WAKEUPTM counter.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void WAKEUPTM_Start(void)
{
   WUTMIF = 0U;   /* clear INTWUTM interrupt flag */
   WUTMMK = 0U;   /* enable INTWUTM interrupt */
   WUTMCE = 1U;   /* enable WUTM operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops WAKEUPTM counter.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void WAKEUPTM_Stop(void)
{
   WUTMCE = 0U;   /* disable WUTM operation */
   WUTMMK = 1U;   /* disable INTWUTM interrupt */
   WUTMIF = 0U;   /* clear INTWUTM interrupt flag */
}
/******************************************************************************
* Function Name     : HAL_TAU0_Init
* Input Params      : None
* Output Params     : None
* Description       : This function is used for 1msec Intialization for scheduler.
******************************************************************************/
void TAU0_Init(void)
{
   TAU0EN = 1U;      /* supply input clock */
   TPS0 = _0000_TAU_CKM0_FCLK_0 | _0040_TAU_CKM1_FCLK_4 | _0800_TAU_CKM2_FCLK_8 | _F000_TAU_CKM3_FCLK_15;

   /* Stop all channels */
   TT0 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON | _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON | _0040_TAU_CH6_STOP_TRG_ON | _0080_TAU_CH7_STOP_TRG_ON;

   /* Mask channel 0 interrupt */
   TMMK00 = 1U;      /* disable INTTM00 interrupt */
   TMIF00 = 0U;      /* clear INTTM00 interrupt flag */

   /* Mask channel 1 interrupt */
   TMMK01 = 1U;      /* disable INTTM01 interrupt */
   TMIF01 = 0U;      /* clear INTTM01 interrupt flag */

   /* Set INTTM00 low priority */
   TMPR100 = 1U;
   TMPR000 = 1U;

   /* Channel 0 is used as master channel for PWM output function */
   TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0800_TAU_COMBINATION_MASTER | _0000_TAU_TRIGGER_SOFTWARE | _0001_TAU_MODE_PWM_MASTER;
   TDR00 = 0x5355;//0x063F;
   TOM0 &= ~_0001_TAU_CH0_OUTPUT_COMBIN;
   TOL0 &= ~_0001_TAU_CH0_OUTPUT_LEVEL_L;
   TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;

   /* Channel 1 is used as slave channel for PWM output function */
   TMR01 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
   TDR01 = 0x29aa;//0x031F;
   TOM0 |= _0002_TAU_CH1_OUTPUT_COMBIN;
   TOL0 |= _0002_TAU_CH1_OUTPUT_LEVEL_L;
   //TOL0 &= ~_0002_TAU_CH1_OUTPUT_LEVEL_L;
   TO0 &= ~_0002_TAU_CH1_OUTPUT_VALUE_1;
   TOE0 |= _0002_TAU_CH1_OUTPUT_ENABLE;

   /* Set TO01 pin */
   P3 &= 0xFEU;
   PM3 &= 0xFEU;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts TAU0 channel 0 counter.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU0_Channel0_Start(void)
{
   TOE0 |= _0002_TAU_CH1_OUTPUT_ENABLE;
   TS0 |= _0001_TAU_CH0_START_TRG_ON | _0002_TAU_CH1_START_TRG_ON;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops TAU0 channel 0 counter.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU0_Channel0_Stop(void)
{
   TT0 |= _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON;
   TOE0 &= ~_0002_TAU_CH1_OUTPUT_ENABLE;
}

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function changes TAU0 channel 1 duty value when used as PWM slave channel.
**
**  Parameters:
**   ratio: duty value to change
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU0_Channel1_ChangeDuty(u8 ratio)
{
   u32 reg = 0U;
   
   if (ratio > 100U)
   {
      ratio = 100U;
   }
   reg = TDR00;
   reg = (reg + 1U) * ratio / 100U;
   TDR01 = (u16)reg;
}

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function initializes the TAU1 module.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Init(void)
{
   TAU1EN = 0U;      /* supply input clock */

}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts TAU1 channel 0 counter.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Channel0_Start(void)
{
   TOE1 |= _0002_TAU_CH1_OUTPUT_ENABLE | _0004_TAU_CH2_OUTPUT_ENABLE | _0008_TAU_CH3_OUTPUT_ENABLE | _0010_TAU_CH4_OUTPUT_ENABLE;
   TS1 |= _0001_TAU_CH0_START_TRG_ON | _0002_TAU_CH1_START_TRG_ON | _0004_TAU_CH2_START_TRG_ON | _0008_TAU_CH3_START_TRG_ON | _0010_TAU_CH4_START_TRG_ON;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops TAU1 channel 0 counter.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Channel0_Stop(void)
{
   TT1 |= _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON | _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON;
   TOE1 &= ~_0002_TAU_CH1_OUTPUT_ENABLE & ~_0004_TAU_CH2_OUTPUT_ENABLE & ~_0008_TAU_CH3_OUTPUT_ENABLE & ~_0010_TAU_CH4_OUTPUT_ENABLE;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function changes TAU1 channel 1 duty value when used as PWM slave channel.
**
**  Parameters:
**   ratio: duty value to change
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Channel1_ChangeDuty(u8 ratio)
{
   u32 reg = 0U;
   
   if (ratio > 100U)
   {
      ratio = 100U;
   }
   reg = TDR10;
   reg = (reg + 1U) * ratio / 100U;
   TDR11 = (u16)reg;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function changes TAU1 channel 2 duty value when used as PWM slave channel.
**
**  Parameters:
**   ratio: duty value to change
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Channel2_ChangeDuty(u8 ratio)
{
   u32 reg = 0U;
   
   if (ratio > 100U)
   {
      ratio = 100U;
   }
   reg = TDR10;
   reg = (reg + 1U) * ratio / 100U;
   TDR12 = (u16)reg;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function changes TAU1 channel 3 duty value when used as PWM slave channel.
**
**  Parameters:
**   ratio: duty value to change
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Channel3_ChangeDuty(u8 ratio)
{
   u32 reg = 0U;
   
   if (ratio > 100U)
   {
      ratio = 100U;
   }
   reg = TDR10;
   reg = (reg + 1U) * ratio / 100U;
   TDR13 = (u16)reg;
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function changes TAU1 channel 4 duty value when used as PWM slave channel.
**
**  Parameters:
**   ratio: duty value to change
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void TAU1_Channel4_ChangeDuty(u8 ratio)
{
   u32 reg = 0U;
   
   if (ratio > 100U)
   {
      ratio = 100U;
   }
   reg = TDR10;
   reg = (reg + 1U) * ratio / 100U;
   TDR14 = (u16)reg;
}
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