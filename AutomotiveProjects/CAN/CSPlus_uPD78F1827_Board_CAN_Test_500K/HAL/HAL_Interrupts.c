/******************************************************************************
** Project Name         : 
** File Name            : HAL_Interrupts.C
** Description          : This module is used to configure the Interrupts 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/

/* Define to include local variables in the header file */
#define _HAL_Interrupts_C

#pragma interrupt INTLR1 MD_INTLR1
#pragma interrupt INTLT1 MD_INTLT1
#pragma interrupt INTLS1 MD_INTLS1
#pragma interrupt INTLR0 MD_INTLR0
#pragma interrupt INTLT0 MD_INTLT0
#pragma interrupt INTC0TRX CAN_TX_ISR
#pragma interrupt INTC0REC CAN_RX_ISR
#pragma interrupt INTC0ERR CAN_ERR_ISR
#pragma interrupt INTC0WUP CAN_WUP_ISR
#pragma interrupt INTLS0 MD_INTLS0
#pragma interrupt INTIIC11 MD_INTIIC11
#pragma interrupt INTWUTM MD_INTWUTM
#pragma interrupt INTTM00 TM00_isr
#pragma interrupt INTTM11 MD_INTTM11
#pragma interrupt INTTM10 MD_INTTM10
#pragma interrupt INTTM12 TM12_isr
#pragma interrupt INTCSI00 SPI0_ISR
#pragma interrupt INTWDTI MD_INTWDTI

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
/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*<list all local macro declarations> */

/******************************************************************************
**               D A T A                                       */
/******************************************************************************/


/**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLR0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------

**-----------------------------------------------------------------------------
*/
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLR0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTLR1(void)
{

}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLT0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTLT1(void)
{

}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLS0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTLS1(void)
{

   
}


/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLR0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTLR0(void)
{
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLT0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTLT0(void)
{
  
}
/******************************************************************************
* Function Name     : INTC0TRX()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
__interrupt void CAN_TX_ISR(void)
{
    
    /* Interrupt proceeesing for Noraml operation */
    if((CINTS & 0x0001) == 0x0001)
    {
       CINTS |= 0x0001;
    }
    /*
    if ((CanGetMCTRL(0) & kMCTRL_RDY)  != 0)
    {
       CanWriteMCTRL(0, (u16)(kClearMCTRL_RDY|kClearMCTRL_IE));
    }
    if ((CanGetMCTRL(1) & kMCTRL_RDY)  != 0)
    {
       CanWriteMCTRL(1, (u16)(kClearMCTRL_RDY|kClearMCTRL_IE));
    }
    if ((CanGetMCTRL(2) & kMCTRL_RDY)  != 0)
    {
       CanWriteMCTRL(2, (u16)(kClearMCTRL_RDY|kClearMCTRL_IE));
    }
    */
    CanTxCmplt();
    
} 

/******************************************************************************
* Function Name     : CAN_RX_ISR()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
__interrupt void CAN_RX_ISR(void)
{
  CanReceive();
  CINTS = 0x0002;
  
}
/******************************************************************************
* Function Name     : CAN_ERR_ISR()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
__interrupt void CAN_ERR_ISR(void)
{


}
/******************************************************************************
* Function Name     : CAN_WUP_ISR()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
__interrupt void CAN_WUP_ISR(void)
{


}

/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTLS0 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTLS0(void)
{

   
}
/*
**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTIIC11 interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTIIC11(void)
{
  
}
/**-----------------------------------------------------------------------------
**
**  Abstract:
**   This function is INTWUTM interrupt service routine.
**
**  Parameters:
**   None
**
**  Returns:
**   None
**
**-----------------------------------------------------------------------------
*/
__interrupt void MD_INTWUTM(void)
{
   MOS_Scheduler_ISR();
}
/******************************************************************************
* Function Name     : __interrupt void TM00_isr
* Input Params      : None
* Output Params     : None
* Description        : This is the interrupt rutine for the schedular tick handler
******************************************************************************/
__interrupt void TM00_isr (void)
{
}
/******************************************************************************
* Function Name     : __interrupt void TMR11isr
* Input Params      : None
* Output Params     : None
* Description        : This is the timer 1 interrupt rutine for the PWM generation
******************************************************************************/
__interrupt void MD_INTTM11(void)

{
  TMIF11 = 0U;   /* clear INTTM11 interrupt flag */
}
/******************************************************************************
* Function Name     :__interrupt void MD_INTTM10
* Input Params      : None
* Output Params     : None
* Description        : This is the timer 1 interrupt rutine for the PWM generation
******************************************************************************/
__interrupt void MD_INTTM10(void)
{
  TMIF10 = 0U;/* clear INTTM10 interrupt flag */
}
/******************************************************************************
* Function Name     : __interrupt void TM12_isr
* Input Params      : None
* Output Params     : None
* Description        : This is the timer 1 interrupt rutine for the SPI comm.
******************************************************************************/
__interrupt void TM12_isr (void)
{
  TMIF12 = 0U;/* clear INTTM10 interrupt flag */
  
  /*assign the interbyte delay*/
  
}


/******************************************************************************
* Function Name     : SPI0_ISR
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
__interrupt void SPI0_ISR(void)
{
}
/******************************************************************************
* Function Name     : MD_INTWDTI()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to indicate teh WDT reset done
******************************************************************************/
__interrupt void MD_INTWDTI(void)
{
      WDTIIF = 0U;   /* clear INTWDTI interrupt flag */
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
