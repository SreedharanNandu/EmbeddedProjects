/******************************************************************************
** Project Name         : 
** File Name            : HAL_Interrupts.C
** Description          : This module is used to configure the Interrupts 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/

#define INT_VECTOR_RELOCATABLE 1

#if (INT_VECTOR_RELOCATABLE == 1)/*Use relocated vector table*/
#else
   #pragma interrupt INTWDTI MD_INTWDTI
   #pragma interrupt INTLVI MD_INTLVI
   #pragma interrupt INTP0 MD_INTP0
   #pragma interrupt INTP1 MD_INTP1
   #pragma interrupt INTP2 MD_INTP2
   #pragma interrupt INTP3 MD_INTP3
   #pragma interrupt INTP4 MD_INTP4
   #pragma interrupt INTP5 MD_INTP5
   #pragma interrupt INTCLM MD_INTCLM
   #pragma interrupt INTCSI00 MD_INTCSI00
   /*#pragma interrupt INTCSI01 MD_INTCSI01*//*NA for FC3*/
   #pragma interrupt INTDMA0 MD_INTDMA0
   #pragma interrupt INTDMA1 MD_INTDMA1
   #pragma interrupt INTWUTM MD_INTWUTM
   #pragma interrupt INTFL MD_INTFL
   #pragma interrupt INTLT0 MD_INTLT0
   #pragma interrupt INTLR0 MD_INTLR0
   #pragma interrupt INTLS0 MD_INTLS0
   #pragma interrupt INTPLR0 MD_INTPLR0
   /*#pragma interrupt INTP8 MD_INTP8*/ /*NA for FC3*/
   #pragma interrupt INTTM00 MD_INTTM00
   #pragma interrupt INTTM01 MD_INTTM01
   #pragma interrupt INTTM02 MD_INTTM02
   #pragma interrupt INTTM03 MD_INTTM03
   #pragma interrupt INTAD MD_INTAD
   #pragma interrupt INTLT1 MD_INTLT1
   #pragma interrupt INTLR1 MD_INTLR1
   #pragma interrupt INTLS1 MD_INTLS1
   #pragma interrupt INTPLR1 MD_INTPLR1
   #pragma interrupt INTCSI10 MD_INTCSI10
   #pragma interrupt INTIIC11 MD_INTIIC11
   #pragma interrupt INTTM04 MD_INTTM04
   #pragma interrupt INTTM05 MD_INTTM05
   #pragma interrupt INTTM06 MD_INTTM06
   #pragma interrupt INTTM07 MD_INTTM07
   #pragma interrupt INTP6 MD_INTP6
   #pragma interrupt INTP7 MD_INTP7
   #pragma interrupt INTC0ERR MD_INTC0ERR
   #pragma interrupt INTC0WUP MD_INTC0WUP
   #pragma interrupt INTC0REC MD_INTC0REC
   #pragma interrupt INTC0TRX MD_INTC0TRX
   #pragma interrupt INTTM10 MD_INTTM10
   #pragma interrupt INTTM11 MD_INTTM11
   #pragma interrupt INTTM12 MD_INTTM12
   #pragma interrupt INTTM13 MD_INTTM13
   #pragma interrupt INTMD MD_INTMD
   /*#pragma interrupt INTST2 MD_INTST2*/ /*NA for FC3*/
   /*#pragma interrupt INTSR2 MD_INTSR2*/ /*NA for FC3*/
   /*#pragma interrupt INTPR2 MD_INTPR2*/ /*NA for FC3*/
   #pragma interrupt INTTM14 MD_INTTM14
   #pragma interrupt INTTM15 MD_INTTM15
   #pragma interrupt INTTM16 MD_INTTM16
   #pragma interrupt INTTM17 MD_INTTM17
   /*#pragma interrupt INTTM20 MD_INTTM20*//*NA for FC3*/
   /*#pragma interrupt INTTM21 MD_INTTM21*//*NA for FC3*/
   /*#pragma interrupt INTTM22 MD_INTTM22*//*NA for FC3*/
   /*#pragma interrupt INTTM23 MD_INTTM23*//*NA for FC3*/
   /*#pragma interrupt INTTM25 MD_INTTM25*//*NA for FC3*/
   /*#pragma interrupt INTTM27 MD_INTTM27*//*NA for FC3*/
   #pragma interrupt INTDMA2 MD_INTDMA2
   #pragma interrupt INTDMA3 MD_INTDMA3
#endif

/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
/*<list all system include files>*/

#include <CG_macrodriver.h>

#include "HAL_Interrupt_API.h"
#include "system_def.h"
#include "MOS.h"
#include "HAL_Timer_API.h"
#include "HAL_Timer_API.h"
#include "HAL_CAN_Config.h"
#include "system_def.h"


#define INTERRUPT_NEC  __interrupt



/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*<list all local macro declarations> */

/******************************************************************************
**               D A T A                                       */
/******************************************************************************/
INTERRUPT_NEC void MD_INTWDTI(void)
{
   WDTIIF = 0U;   /* clear INTWDTI interrupt flag */
}

INTERRUPT_NEC void MD_INTLVI(void)
{
}
INTERRUPT_NEC void MD_INTP0(void)
{
}
INTERRUPT_NEC void MD_INTP1(void)
{
}
INTERRUPT_NEC void MD_INTP2(void)
{
}
INTERRUPT_NEC void MD_INTP3(void)
{
}
INTERRUPT_NEC void MD_INTP4(void)
{
}
INTERRUPT_NEC void MD_INTP5(void)
{

}
INTERRUPT_NEC void MD_INTCLM(void)
{
}

INTERRUPT_NEC void MD_INTCSI00(void)
{
}

INTERRUPT_NEC void MD_INTDMA0(void)
{
}

INTERRUPT_NEC void MD_INTDMA1(void)
{
}
INTERRUPT_NEC void MD_INTWUTM(void)
{
   MOS_Scheduler_ISR();
}

INTERRUPT_NEC void MD_INTFL(void)
{
}

INTERRUPT_NEC void MD_INTLT0(void)
{
}
INTERRUPT_NEC void MD_INTLR0(void)
{
}
INTERRUPT_NEC void MD_INTLS0(void)
{
}
INTERRUPT_NEC void MD_INTPLR0(void)
{
}
INTERRUPT_NEC void MD_INTTM00(void)
{
}
INTERRUPT_NEC void MD_INTTM01(void)
{
}
INTERRUPT_NEC void MD_INTTM02(void)
{
}
INTERRUPT_NEC void MD_INTTM03(void)
{
}
INTERRUPT_NEC void MD_INTAD(void)
{
}
INTERRUPT_NEC void MD_INTLT1(void)
{
}
INTERRUPT_NEC void MD_INTLR1(void)
{
}
INTERRUPT_NEC void MD_INTLS1(void)
{
}
INTERRUPT_NEC void MD_INTPLR1(void)
{
}
INTERRUPT_NEC void MD_INTCSI10(void)
{
}
INTERRUPT_NEC void MD_INTCSI11(void)
{
}
INTERRUPT_NEC void MD_INTIIC11(void)
{
}
INTERRUPT_NEC void MD_INTTM04(void)
{
}
INTERRUPT_NEC void MD_INTTM05(void)
{
}
INTERRUPT_NEC void MD_INTTM06(void)
{
}
INTERRUPT_NEC void MD_INTTM07(void)
{
}
INTERRUPT_NEC void MD_INTP6(void)
{
}
INTERRUPT_NEC void MD_INTP7(void)
{
}
INTERRUPT_NEC void MD_INTC0ERR(void)
{
}
INTERRUPT_NEC void MD_INTC0WUP(void)
{
}
INTERRUPT_NEC void MD_INTC0REC(void)
{
   CanReceive();
   CINTS = 0x0002;
}
INTERRUPT_NEC void MD_INTC0TRX(void)
{
   /* Interrupt proceeesing for Noraml operation */
   if((CINTS & 0x0001) == 0x0001)
   {
      CINTS |= 0x0001;
   }
   CanTxCmplt();
}
INTERRUPT_NEC void MD_INTTM10(void)
{
  TMIF10 = 0U;/* clear INTTM10 interrupt flag */
}
INTERRUPT_NEC void MD_INTTM11(void)
{
  TMIF11 = 0U;   /* clear INTTM11 interrupt flag */
}
INTERRUPT_NEC void MD_INTTM12(void)
{
  TMIF12 = 0U;/* clear INTTM10 interrupt flag */
}
INTERRUPT_NEC void MD_INTTM13(void)
{
}
INTERRUPT_NEC void MD_INTMD(void)
{
}
INTERRUPT_NEC void MD_INTTM14(void)
{
}
INTERRUPT_NEC void MD_INTTM15(void)
{
}
INTERRUPT_NEC void MD_INTTM16(void)
{
}
INTERRUPT_NEC void MD_INTTM17(void)
{
}
INTERRUPT_NEC void MD_INTDMA2(void)
{
}
INTERRUPT_NEC void MD_INTDMA3(void)
{
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
