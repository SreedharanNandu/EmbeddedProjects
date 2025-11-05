/*
*******************************************************************************
* Copyright(C) 2008, 2017 Renesas Electronics Corporation
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* This program must be used solely for the purpose for which
* it was furnished by Renesas Electronics Corporation. No part of this
* program may be reproduced or disclosed to others, in any
* form, without the prior written permission of Renesas Electronics
* Corporation. 
*
* This device driver was created by CodeGenerator for 78K0R/Fx3
* 16-Bit Single-Chip Microcontrollers
* Filename:	CG_timer.c
* Abstract:	This file implements device driver for TAU module.
* APIlib:	CodeGenerator for 78K0R/Fx3 V3.02.00.01 [12 Apr 2017]
* Device:	uPD78F1829_48
* Compiler:	CC78K0R
* Creation date:	06-09-2025
*******************************************************************************
*/

/*
*******************************************************************************
** Pragma directive
*******************************************************************************
*/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
/*
*******************************************************************************
** Include files
*******************************************************************************
*/
#include "CG_macrodriver.h"
#include "CG_timer.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "CG_userdefine.h"

/*
*******************************************************************************
**  Global define
*******************************************************************************
*/
/* Start user code for global. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the TAU0 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void TAU0_Init(void)
{
	TAU0EN = 1U;		/* supply input clock */
	TPS0 = _0000_TAU_CKM0_FCLK_0 | _0040_TAU_CKM1_FCLK_4 | _0800_TAU_CKM2_FCLK_8 | _F000_TAU_CKM3_FCLK_15;
	/* Stop all channels */
	TT0 = _0001_TAU_CH0_STOP_TRG_ON | _0002_TAU_CH1_STOP_TRG_ON | _0004_TAU_CH2_STOP_TRG_ON | _0008_TAU_CH3_STOP_TRG_ON | _0010_TAU_CH4_STOP_TRG_ON | _0020_TAU_CH5_STOP_TRG_ON | _0040_TAU_CH6_STOP_TRG_ON | _0080_TAU_CH7_STOP_TRG_ON;
	/* Mask channel 0 interrupt */
	TMMK00 = 1U;		/* disable INTTM00 interrupt */
	TMIF00 = 0U;		/* clear INTTM00 interrupt flag */
	/* Mask channel 1 interrupt */
	TMMK01 = 1U;		/* disable INTTM01 interrupt */
	TMIF01 = 0U;		/* clear INTTM01 interrupt flag */
	/* Channel 0 is used as master channel for PWM output function */
	TMR00 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0800_TAU_COMBINATION_MASTER | _0000_TAU_TRIGGER_SOFTWARE | _0001_TAU_MODE_PWM_MASTER;
	TDR00 = _1F3F_TAU_TDR00_VALUE;
	TOM0 &= ~_0001_TAU_CH0_OUTPUT_COMBIN;
	TOL0 &= ~_0001_TAU_CH0_OUTPUT_LEVEL_L;
	TOE0 &= ~_0001_TAU_CH0_OUTPUT_ENABLE;
	/* Channel 1 is used as slave channel for PWM output function */
	TMR01 = _0000_TAU_CLOCK_SELECT_CKM0 | _0000_TAU_CLOCK_MODE_CKS | _0000_TAU_COMBINATION_SLAVE | _0400_TAU_TRIGGER_MASTER_INT | _0009_TAU_MODE_PWM_SLAVE;
	TDR01 = _0FA0_TAU_TDR01_VALUE;
	TOM0 |= _0002_TAU_CH1_OUTPUT_COMBIN;
	TOL0 &= ~_0002_TAU_CH1_OUTPUT_LEVEL_L;
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
**	This function starts TAU0 channel 0 counter.
**
**  Parameters:
**	None
**
**  Returns:
**	None
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
**	This function stops TAU0 channel 0 counter.
**
**  Parameters:
**	None
**
**  Returns:
**	None
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
**	This function changes TAU0 channel 1 duty value when used as PWM slave channel.
**
**  Parameters:
**	ratio: duty value to change
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void TAU0_Channel1_ChangeDuty(UCHAR ratio)
{
	ULONG reg = 0U;
	
	if (ratio > 100U)
	{
		ratio = 100U;
	}
	reg = TDR00;
	reg = (reg + 1U) * ratio / 100U;
	TDR01 = (USHORT)reg;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
