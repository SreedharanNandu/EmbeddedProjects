/******************************************************************************
** Project Name         : 
** File Name            : HAL_Init.h
** Description          : This module is used for initialization 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
#ifndef _HAL_Init_H
#define _HAL_Init_H

#include <CG_macrodriver.h>
#include "system_def.h"
#include "HAL_USART.h"
#include "HAL_SYSCLK_API.h"
#include "HAL_Timer_API.h"
#include "HAL_WDT_API.h"
#include "HAL_PortPin_API.h"
#include "HAL_ADC_API.h"
#include "App_Serial.h"
#include "HAL_LIN.h"




#define Enable_Interrupts()           asm( "ei\n" )     // Set interrupt mask
#define Disable_Interrupts()          asm( "di\n" )     // Reset interrupt mask
#define BSP_ENABLE_INTERRUPTS()       asm( "ei\n" )     // Set interrupt mask
#define BSP_DISABLE_INTERRUPTS()      asm( "di\n" )     // Reset interrupt mask
#define BSP_HALT_CPU()                asm( "halt\n" )    // Halt mode (clock stopped)
#define NO_OPERATION()                asm( "nop\n" )     // No operation
#define STOP_OPERATION()              asm( "stop\n" )     // stop operation   //renesas

/******************************************************************************
**   P R O J E C T        I N C L U D E S
******************************************************************************/
/*list all include files*/


/******************************************************************************
**   S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/

/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
extern void Init_HW(void);

#endif /* for #define _MODULE_NAME_H */


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


