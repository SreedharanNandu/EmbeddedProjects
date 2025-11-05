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
* Filename:	CG_system.c
* Abstract:	This file implements device driver for System module.
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
#include "CG_system.h"
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
**	This function initializes the clock generator.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void CLOCK_Init(void)
{
	UCHAR temp_stabset, temp_stabwait;

	/* Set fMX */
	CMC = _40_CG_HISYS_OSC | _01_CG_SYSOSC_OVER10M | _00_CG_SUB_PORT;
	OSTS = _07_CG_OSCSTAB_SEL18;
	GUARD |= _01_CG_CSC_ENWR;
	MSTOP = 0U;
	temp_stabset = _FF_CG_OSCSTAB_STA18;
	do
	{
		temp_stabwait = OSTC;
		temp_stabwait &= temp_stabset;
	}
	while (temp_stabwait != temp_stabset);
	/* Set fMAIN */
	DI();
	OSMC = _00_CG_FCLK_UNDER20M | _01_CG_FCLK_OVER10M;
	NOP();
	NOP();
	EI();
	MCM0 = 1U;
	/* Set fSUB */
	XTSTOP = 1U;
	/* Set fCLK */
	CSS = 0U;
	CKC &= (UCHAR)~_07_CG_CPUCLK;
	CKC |= _00_CG_CPUCLK_MAIN0;
	/* Set fIH */
	HIOSTOP = 1U;
	GUARD &= (UCHAR)~_01_CG_CSC_ENWR;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
