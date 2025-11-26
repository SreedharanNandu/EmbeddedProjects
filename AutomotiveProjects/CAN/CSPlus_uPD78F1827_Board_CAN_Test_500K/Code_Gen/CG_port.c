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
* Filename:	CG_port.c
* Abstract:	This file implements device driver for PORT module.
* APIlib:	CodeGenerator for 78K0R/Fx3 V3.02.00.01 [12 Apr 2017]
* Device:	uPD78F1827_48
* Compiler:	CC78K0R
* Creation date:	11-08-2025
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
#include "CG_port.h"
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
**	This function initializes the Port I/O.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void PORT_Init(void)
{
	P1 = _00_Pn0_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0 | _00_Pn4_OUTPUT_0 | _00_Pn6_OUTPUT_0;
	P3 = _00_Pn0_OUTPUT_0 | _00_Pn2_OUTPUT_0;
	P4 = _00_Pn0_OUTPUT_0;
	P6 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn3_OUTPUT_0;
	P8 = _00_Pn0_OUTPUT_0 | _00_Pn1_OUTPUT_0 | _00_Pn2_OUTPUT_0 | _00_Pn4_OUTPUT_0 | _00_Pn5_OUTPUT_0 | _00_Pn6_OUTPUT_0;
	P12 = _00_Pn5_OUTPUT_0;
	P14 = _00_Pn0_OUTPUT_0;
	PU1 = _02_PUn1_PULLUP_ON | _20_PUn5_PULLUP_ON | _80_PUn7_PULLUP_ON;
	PU12 = _01_PUn0_PULLUP_ON;
	PM1 = _00_PMn0_MODE_OUTPUT | _02_PMn1_MODE_INPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _00_PMn4_MODE_OUTPUT | _20_PMn5_MODE_INPUT | _00_PMn6_MODE_OUTPUT | _80_PMn7_MODE_INPUT;
	PM3 = _00_PMn0_MODE_OUTPUT | _02_PMn1_MODE_INPUT | _00_PMn2_MODE_OUTPUT | _F8_PM3_DEFAULT;
	PM4 = _00_PMn0_MODE_OUTPUT | _02_PMn1_NOT_USE | _FD_PM4_DEFAULT;
	PM6 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _00_PMn3_MODE_OUTPUT | _F0_PM6_DEFAULT;
	PM8 = _00_PMn0_MODE_OUTPUT | _00_PMn1_MODE_OUTPUT | _00_PMn2_MODE_OUTPUT | _08_PMn3_NOT_USE | _00_PMn4_MODE_OUTPUT | _00_PMn5_MODE_OUTPUT | _00_PMn6_MODE_OUTPUT | _80_PMn7_NOT_USE;
	PM12 = _01_PMn0_MODE_INPUT | _00_PMn5_MODE_OUTPUT | _DE_PM12_DEFAULT;
	PM14 = _00_PMn0_MODE_OUTPUT | _FE_PM14_DEFAULT;
}

/* Start user code for adding. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
