/******************************************************************************
** Project Name         : 
** File Name            : HAL_Interrupts.C
** Description          : This module is used to configure the Interrupts 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/

/* Define to include local variables in the header file */
#define _HAL_Interrupts_C


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
/*<list all system include files>*/
#include <io78f1827_48.h>
#include <io78f1827_48_ext.h>
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

__interrupt void TM00_isr(void);
__interrupt void TM02_isr(void);
__interrupt void P0_isr(void);
__interrupt void LIN_INTLT0_ISR(void);
__interrupt void LIN_INTLR0_ISR(void);
__interrupt void LIN_INTLS0_ISR(void);
__interrupt void SPI0_ISR(void); 
__interrupt void MD_INTWDTI(void);
__interrupt void CAN_TX_ISR(void);
__interrupt void CAN_RX_ISR(void);
__interrupt void CAN_ERR_ISR(void);
__interrupt void CAN_WUP_ISR(void);
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
#pragma vector = INTLR1_vect
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
#pragma vector = INTLT1_vect
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
#pragma vector = INTLS1_vect
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
#pragma vector = INTLR0_vect
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
#pragma vector = INTLT0_vect
__interrupt void MD_INTLT0(void)
{
  
}
/******************************************************************************
* Function Name     : INTC0TRX_vect()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
#pragma vector=INTC0TRX_vect

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
#pragma vector=INTC0REC_vect
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
#pragma vector=INTC0ERR_vect
__interrupt void CAN_ERR_ISR(void)
{


}
/******************************************************************************
* Function Name     : CAN_WUP_ISR()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to call the Transmit interrupt.
******************************************************************************/
#pragma vector=INTC0WUP_vect
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
#pragma vector = INTLS0_vect
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
#pragma vector = INTIIC11_vect
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
#pragma vector = INTWUTM_vect
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
#pragma vector=INTTM00_vect
__interrupt void TM00_isr (void)
{
}
/******************************************************************************
* Function Name     : __interrupt void TMR11isr
* Input Params      : None
* Output Params     : None
* Description        : This is the timer 1 interrupt rutine for the PWM generation
******************************************************************************/
#pragma vector= INTTM11_vect
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
#pragma vector = INTTM10_vect
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
#pragma vector = INTTM12_vect
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
#pragma vector=INTCSI00_vect
__interrupt void SPI0_ISR(void)
{
}
/******************************************************************************
* Function Name     : MD_INTWDTI()
* Input Params      : None
* Output Params     : None
* Description       : This function is used to indicate teh WDT reset done
******************************************************************************/
#pragma vector = INTWDTI_vect
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
