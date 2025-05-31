#include "p30f3010.h"
#include "timer.h"
#include "outcompare.h"

/********************************************************************
*    Function Name:  OpenTimer2                                     *
*    Description:    This routine configures the timer control      *
*                    register and timer period register.            *
*    Parameters:     config: bit definitions to configure Timer2    *
*                    period: value to be loaded to PR reg           *
*    Return Value:   None                                           *
********************************************************************/

void OpenTimer3(unsigned int config,unsigned int period)
{
    TMR3  = 0;          /* Reset Timer3 to 0x0000 */
    PR3   = period;     /* assigning Period to Timer period register */
    T3CON = config;     /* configure control reg */
}
/********************************************************************
*    Function Name:  ReadTimer3                                     *
*    Description:    This routine reads the 16-bit value from       *
*                    Timer3 Register.                               *
*    Parameters:     None                                           *
*    Return Value:   unsigned int: Timer  16-bit value              *
********************************************************************/

unsigned int ReadTimer3(void)
{
    return (TMR3);      /* Return the Timer3 register */
}
/********************************************************************
*    Function Name:  WriteTimer3                                    *
*    Description:    This routine writes a 16-bit value to Timer3   *
*    Parameters:     unsigned int: value to write to Timer          *
*    Return Value:   None                                           *
********************************************************************/

void WriteTimer3(unsigned int timer)
{
    TMR3 = timer;     /* assign timer value to Timer3 Register */
}
/********************************************************************
*    Function Name:  CloseTimer3                                    *
*    Description:    This routine disables the Timer3 and its       *
*                    interrupt and flag bits.                       *
*    Parameters:     None                                           *
*    Return Value:   None                                           *
********************************************************************/

void CloseTimer3(void)
{
    IEC0bits.T3IE = 0;      /* Disable the Timer3 interrupt */
    T3CONbits.TON = 0;      /* Disable timer3 */
    IFS0bits.T3IF = 0;      /* Clear Timer interrupt flag */
}
/*******************************************************************
*    Function Name: ConfigIntTimer3                                *
*    Description:   This function configures interrupt and sets    *
*                   Interrupt Priority                             *
*    Parameters:    unsigned int config                            *
*    Return Value:  None                                           *
*******************************************************************/

void ConfigIntTimer3(unsigned int config)
{
    IFS0bits.T3IF = 0;                   /* clear IF bit */
    IPC1bits.T3IP = (config &0x0007);    /* assigning Interrupt Priority */
    IEC0bits.T3IE = (config &0x0008)>>3; /* Interrupt Enable /Disable */
}

