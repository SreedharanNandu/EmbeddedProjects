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
* Filename:	CG_port.h
* Abstract:	This file implements device driver for PORT module.
* APIlib:	CodeGenerator for 78K0R/Fx3 V3.02.00.01 [12 Apr 2017]
* Device:	uPD78F1827_48
* Compiler:	CC78K0R
* Creation date:	11-08-2025
*******************************************************************************
*/

#ifndef _MDPORT_
#define _MDPORT_
/*
*******************************************************************************
**  Register bit define
*******************************************************************************
*/
/*
	Port Mode Register (PMm)
*/
/* Pmn pin I/O mode selection (PMm7 - PMm0) */
#define _01_PMn0_NOT_USE	0x01U	/* not use Pn0 as digital I/O */
#define _01_PMn0_MODE_INPUT	0x01U	/* use Pn0 as input mode */
#define _00_PMn0_MODE_OUTPUT	0x00U	/* use Pn0 as output mode */
#define _02_PMn1_NOT_USE	0x02U	/* not use Pn1 as digital I/O */
#define _02_PMn1_MODE_INPUT	0x02U	/* use Pn1 as input mode */
#define _00_PMn1_MODE_OUTPUT	0x00U	/* use Pn1 as output mode */
#define _04_PMn2_NOT_USE	0x04U	/* not use Pn2 as digital I/O */
#define _04_PMn2_MODE_INPUT	0x04U	/* use Pn2 as input mode */
#define _00_PMn2_MODE_OUTPUT	0x00U	/* use Pn2 as output mode */
#define _08_PMn3_NOT_USE	0x08U	/* not use Pn3 as digital I/O */
#define _08_PMn3_MODE_INPUT	0x08U	/* use Pn3 as input mode */
#define _00_PMn3_MODE_OUTPUT	0x00U	/* use Pn3 as output mode */
#define _10_PMn4_NOT_USE	0x10U	/* not use Pn4 as digital I/O */
#define _10_PMn4_MODE_INPUT	0x10U	/* use Pn4 as input mode */
#define _00_PMn4_MODE_OUTPUT	0x00U	/* use Pn4 as output mode */
#define _20_PMn5_NOT_USE	0x20U	/* not use Pn5 as digital I/O */
#define _20_PMn5_MODE_INPUT	0x20U	/* use Pn5 as input mode */
#define _00_PMn5_MODE_OUTPUT	0x00U	/* use Pn5 as output mode */
#define _40_PMn6_NOT_USE	0x40U	/* not use Pn6 as digital I/O */
#define _40_PMn6_MODE_INPUT	0x40U	/* use Pn6 as input mode */
#define _00_PMn6_MODE_OUTPUT	0x00U	/* use Pn6 as output mode */
#define _80_PMn7_NOT_USE	0x80U	/* not use Pn7 as digital I/O */
#define _80_PMn7_MODE_INPUT	0x80U	/* use Pn7 as input mode */
#define _00_PMn7_MODE_OUTPUT	0x00U	/* use Pn7 as output mode */

/*
	Port Register (Pm)
*/
/* Pmn pin data (Pm0 to Pm7) */
#define _00_Pn0_OUTPUT_0	0x00U	/* Pn0 output 0 */
#define _01_Pn0_OUTPUT_1	0x01U	/* Pn0 output 1 */
#define _00_Pn1_OUTPUT_0	0x00U	/* Pn1 output 0 */
#define _02_Pn1_OUTPUT_1	0x02U	/* Pn1 output 1 */
#define _00_Pn2_OUTPUT_0	0x00U	/* Pn2 output 0 */
#define _04_Pn2_OUTPUT_1	0x04U	/* Pn2 output 1 */
#define _00_Pn3_OUTPUT_0	0x00U	/* Pn3 output 0 */
#define _08_Pn3_OUTPUT_1	0x08U	/* Pn3 output 1 */
#define _00_Pn4_OUTPUT_0	0x00U	/* Pn4 output 0 */
#define _10_Pn4_OUTPUT_1	0x10U	/* Pn4 output 1 */
#define _00_Pn5_OUTPUT_0	0x00U	/* Pn5 output 0 */
#define _20_Pn5_OUTPUT_1	0x20U	/* Pn5 output 1 */
#define _00_Pn6_OUTPUT_0	0x00U	/* Pn6 output 0 */
#define _40_Pn6_OUTPUT_1	0x40U	/* Pn6 output 1 */
#define _00_Pn7_OUTPUT_0	0x00U	/* Pn7 output 0 */
#define _80_Pn7_OUTPUT_1	0x80U	/* Pn7 output 1 */

/*
	Pull-up Resistor Option Register (PUm)
*/
/* Pmn pin on-chip pull-up resistor selection (PUmn) */
#define _00_PUn0_PULLUP_OFF	0x00U	/* Pn0 pull-up resistor not connected */
#define _01_PUn0_PULLUP_ON	0x01U	/* Pn0 pull-up resistor connected */
#define _00_PUn1_PULLUP_OFF	0x00U	/* Pn1 pull-up resistor not connected */
#define _02_PUn1_PULLUP_ON	0x02U	/* Pn1 pull-up resistor connected */
#define _00_PUn2_PULLUP_OFF	0x00U	/* Pn2 Pull-up resistor not connected */
#define _04_PUn2_PULLUP_ON	0x04U	/* Pn2 pull-up resistor connected */
#define _00_PUn3_PULLUP_OFF	0x00U	/* Pn3 pull-up resistor not connected */
#define _08_PUn3_PULLUP_ON	0x08U	/* Pn3 pull-up resistor connected */
#define _00_PUn4_PULLUP_OFF	0x00U	/* Pn4 pull-up resistor not connected */
#define _10_PUn4_PULLUP_ON	0x10U	/* Pn4 pull-up resistor connected */
#define _00_PUn5_PULLUP_OFF	0x00U	/* Pn5 pull-up resistor not connected */
#define _20_PUn5_PULLUP_ON	0x20U	/* Pn5 pull-up resistor connected */
#define _00_PUn6_PULLUP_OFF	0x00U	/* Pn6 pull-up resistor not connected */
#define _40_PUn6_PULLUP_ON	0x40U	/* Pn6 pull-up resistor connected */
#define _00_PUn7_PULLUP_OFF	0x00U	/* Pn7 pull-up resistor not connected */
#define _80_PUn7_PULLUP_ON	0x80U	/* Pn7 pull-up resistor connected */

/*
	Port Input Mode Register (PIMm)
*/
/* Pmn pin input buffer selection (PIMmn) */
#define _00_PIMn0_TTL_OFF	0x00U	/* set Pn0 normal input buffer */
#define _01_PIMn0_TTL_ON	0x01U	/* set Pn0 TTL input buffer */
#define _00_PIMn1_TTL_OFF	0x00U	/* set Pn1 normal input buffer */
#define _02_PIMn1_TTL_ON	0x02U	/* set Pn1 TTL input buffer */
#define _00_PIMn3_TTL_OFF	0x00U	/* set Pn3 normal input buffer */
#define _08_PIMn3_TTL_ON	0x08U	/* set Pn3 TTL input buffer */

/*
	Port Output Mode Register (POMm)
*/
/* Pmn pin output mode selection (POMmn) */
#define _00_POMn3_NCH_OFF	0x00U	/* set Pn3 output normal mode */
#define _08_POMn3_NCH_ON	0x08U	/* set Pn3 output N-ch open-drain mode */

/*
	Port Slew Rate Select Register (PRSEL)
*/
/* Pmn output mode selection (PSRmn) */
#define _00_PSR10_NORMAL_MODE	0x00U	/* set P10/TI00/_SCK10/TO00/CTXD/LTXD1 pin normal output mode */
#define _01_PSR10_SLOW_MODE	0x01U	/* set P10/TI00/_SCK10/TO00/CTXD/LTXD1 pin slow output mode */
#define _00_PSR12_NORMAL_MODE	0x00U	/* set P12/INTP3/TI16/SO10/TO16 pin normal output mode */
#define _02_PSR12_SLOW_MODE	0x02U	/* set P12/INTP3/TI16/SO10/TO16 pin slow output mode */
#define _00_PSR30_NORMAL_MODE	0x00U	/* set P30/INTP2/_SSI00/TI11/TO11 pin normal output mode */
#define _04_PSR30_SLOW_MODE	0x04U	/* set P30/INTP2/_SSI00/TI11/TO11 pin slow output mode */
#define _00_PSR140_NORMAL_MODE	0x00U	/* set P140/PCL pin normal output mode */
#define _20_PSR140_SLOW_MODE	0x20U	/* set P140/PCL pin slow output mode */
/*
*******************************************************************************
**  Macro define
*******************************************************************************
*/
#define	_FE_PM0_DEFAULT		0xFEU	/* PM0 default value */
#define	_F8_PM3_DEFAULT		0xF8U	/* PM3 default value */
#define	_FD_PM4_DEFAULT		0xFDU	/* PM4 default value */
#define	_F0_PM6_DEFAULT		0xF0U	/* PM6 default value */
#define	_F0_PM7_DEFAULT		0xF0U	/* PM7 default value */
#define	_F8_PM9_DEFAULT		0xF8U	/* PM9 default value */
#define	_DE_PM12_DEFAULT	0xDEU	/* PM12 default value */
#define	_FE_PM14_DEFAULT	0xFEU	/* PM14 default value */
/*
*******************************************************************************
**  Function define
*******************************************************************************
*/
void PORT_Init(void);

/* Start user code for function. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#endif
