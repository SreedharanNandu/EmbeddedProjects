/******************************************************************************
**  License Information:
**  ? Methode Electronics Inc., 20xx-20xx
**  Licensed software and All rights reserved.
**  Property of Methode Electronics Inc, should not be copied, disclosed,
**  reproduced without a prior written consent of Copyright holder
*******************************************************************************
** Project Name         : T1xx_SWP
** File Name            : HAL_USART.c
** Description          : This module is used to define LIN driver
** Date                 : 
** Hardware Platform    : 78K0R/Fx3 Renesas 
******************************************************************************/


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
#include <CG_macrodriver.h>
#include <stdio.h>
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>




/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/

#include "system_def.h"
#include "HAL_USART.h"
#include "system_def.h"
#include "APP_Main.h"
#include "HAL_LIN.h"

/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/* Function Prototypes */
/* local macro declarations */


/******************************************************************************
**               D A T A
******************************************************************************/
/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
/*<list all function definitins here>  */
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**	This function initializes the UARTF0 module.
**
**  Parameters:
**	None
**
**  Returns:
**	None
**
**-----------------------------------------------------------------------------
*/
void UARTF0_Init(void)
{
   LIN0EN = 1U;
   UF0CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF0 operation */
   LTMK0 = 1U;   /* disable INTLT0 interrupt */
   LTIF0 = 0U;   /* clear INTLT0 interrupt flag */
   LRMK0 = 1U;   /* disable INTLR0 interrupt */
   LRIF0 = 0U;   /* clear INTLR0 interrupt flag */
   LSMK0 = 1U;   /* disable INTLS0 interrupt */
   LSIF0 = 0U;   /* clear INTLS0 interrupt flag */
   /* Set INTLT0 low priority */
   LTPR10 = 0U;
   LTPR00 = 0U;
   /* Set INTLR0 low priority */
   LRPR10 = 0U;
   LRPR00 = 0U;
   /* Set INTLS0 low priority */
   LSPR10 = 1U;
   LSPR00 = 1U;
   UF0CTL1 = _0341_UARTF0_K_VALUE;//for 9600bps                 [_E000_UARTF_BASECLK_128 | _0006_UARTF0_K_VALUE(for 10400bps)]
   UF0OPT0 = _14_UARTF_UFNOPT0_INITIALVALUE | _00_UARTF_TRAN_DATALEVEL_NORMAL | _00_UARTF_REC_DATALEVEL_NORMAL;
   UF0OPT1 = _00_UARTF_EXPANSIONBIT_UNUSE | _00_UARTF_NORMAL_MODE;
   UF0OPT2 = _01_UARTF_LT_INT_GENTIME_1 | _00_UARTF_DATA_NOISE_FILTER_USED;
   UF0CTL0 = _10_UARTF_TRANSFDIR_LSB | _00_UARTF_PARITY_NONE | _02_UARTF_DATALENGTH_8BIT | _00_UARTF_STOPLENGTH_1BIT;
   
   /* Set LTXD0 pin 1.3 */
   P1 |= 0x08U;
   PM1 &= 0xF7U;
   /* Set LRXD0 pin 1.4*/
   PM1 |= 0x10U;
   UARTF0_Start();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts the UARTF0 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF0_Start(void)
{
   LTIF0 = 0U;   /* clear INTLT0 interrupt flag */
   LTMK0 = 0U;   /* enable INTLT0 interrupt */
   LRIF0 = 0U;   /* clear INTLR0 interrupt flag */
   LRMK0 = 0U;   /* enable INTLR0 interrupt */
   LSIF0 = 0U;   /* clear INTLS0 interrupt flag */
   LSMK0 = 0U;   /* enable INTLS0 interrupt */
   UF0CTL0 |= _40_UARTF_TRANSMISSION_ENABLE | _20_UARTF_RECEPTION_ENABLE;   /* enable UARTF0 operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops the UARTF0 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF0_Stop(void)
{
   UF0CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF0 operation */
   LTMK0 = 1U;   /* disable INTLT0 interrupt */
   LTIF0 = 0U;   /* clear INTLT0 interrupt flag */
   LRMK0 = 1U;   /* disable INTLR0 interrupt */
   LRIF0 = 0U;   /* clear INTLR0 interrupt flag */
   LSMK0 = 1U;   /* disable INTLS0 interrupt */
   LSIF0 = 0U;   /* clear INTLS0 interrupt flag */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function initializes the UARTF1 module.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF1_Init(void)
{
   LIN1EN = 1U;
   UF1CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF1 operation */
   LTMK1 = 1U;   /* disable INTLT1 interrupt */
   LTIF1 = 0U;   /* clear INTLT1 interrupt flag */
   LRMK1 = 1U;   /* disable INTLR1 interrupt */
   LRIF1 = 0U;   /* clear INTLR1 interrupt flag */
   LSMK1 = 1U;   /* disable INTLS1 interrupt */
   LSIF1 = 0U;   /* clear INTLS1 interrupt flag */
   /* Set INTLT1 low priority */
   LTPR11 = 0U;
   LTPR01 = 0U;
   /* Set INTLR1 low priority */
   LRPR11 = 0U;
   LRPR01 = 0U;
   /* Set INTLS1 low priority */
   LSPR11 = 1U;
   LSPR01 = 1U;
   UF1CTL1 = _0341_UARTF0_K_VALUE;//for 9600bps
   UF1OPT0 = _14_UARTF_UFNOPT0_INITIALVALUE | _00_UARTF_TRAN_DATALEVEL_NORMAL | _00_UARTF_REC_DATALEVEL_NORMAL;
   UF1OPT1 = _00_UARTF_EXPANSIONBIT_UNUSE | _00_UARTF_NORMAL_MODE;
   UF1OPT2 = _01_UARTF_LT_INT_GENTIME_1 | _00_UARTF_DATA_NOISE_FILTER_USED;
   UF1CTL0 = _10_UARTF_TRANSFDIR_LSB | _00_UARTF_PARITY_NONE | _02_UARTF_DATALENGTH_8BIT | _00_UARTF_STOPLENGTH_1BIT;
   STSEL &= 0x7FU;
   /* Set LTXD1 pin */
   P1 |= 0x01U;
   PM1 &= 0xFEU;
   /* Set LRXD1 pin */
   PM1 |= 0x02U;

   UARTF1_Start();
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function starts the UARTF1 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF1_Start(void)
{
   LTIF1 = 0U;   /* clear INTLT1 interrupt flag */
   LTMK1 = 0U;   /* enable INTLT1 interrupt */
   LRIF1 = 0U;   /* clear INTLR1 interrupt flag */
   LRMK1 = 0U;   /* enable INTLR1 interrupt */
   LSIF1 = 0U;   /* clear INTLS1 interrupt flag */
   LSMK1 = 0U;   /* enable INTLS1 interrupt */
   UF1CTL0 |= _40_UARTF_TRANSMISSION_ENABLE | _20_UARTF_RECEPTION_ENABLE;   /* enable UARTF1 operation */
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function stops the UARTF1 operation.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
void UARTF1_Stop(void)
{
   UF1CTL0 &= (u8)(~_40_UARTF_TRANSMISSION_ENABLE & ~_20_UARTF_RECEPTION_ENABLE);   /* disable UARTF1 operation */
   LTMK1 = 1U;   /* disable INTLT1 interrupt */
   LTIF1 = 0U;   /* clear INTLT1 interrupt flag */
   LRMK1 = 1U;   /* disable INTLR1 interrupt */
   LRIF1 = 0U;   /* clear INTLR1 interrupt flag */
   LSMK1 = 1U;   /* disable INTLS1 interrupt */
   LSIF1 = 0U;   /* clear INTLS1 interrupt flag */
}







/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
**
** Revision No          : 00.1
** Date(dd/mm/yy)       : 
** Author               :
** Description of change: Initial release
*******************************************************************************/

