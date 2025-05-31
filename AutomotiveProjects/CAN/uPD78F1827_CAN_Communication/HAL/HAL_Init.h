/******************************************************************************
** Project Name         : 
** File Name            : HAL_Init.h
** Description          : This module is used for initialization 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
#ifndef _HAL_Init_H
#define _HAL_Init_H

#include "system_def.h"
#include "HAL_USART.h"
#include "io78f1827_48.h"
#include "HAL_SYSCLK_API.h"
#include "HAL_Timer_API.h"
#include "HAL_WDT_API.h"
#include "HAL_CAN_API.h"
#include "HAL_PortPin_API.h"
#include "HAL_ADC_API.h"
#include "INTF_VTMR_API.h"
#include "App_Serial.h"



#define   MD_ON      1U
#define   MD_OFF      0U

#define   MD_TRUE      1U
#define   MD_FALSE   0U

#define MD_SET      1U
#define MD_CLEAR   0U

/* Status list definition */
#define MD_STATUSBASE      0x00U
#define MD_OK         (MD_STATUSBASE + 0x00U)   /* register setting OK */
#define MD_RESET      (MD_STATUSBASE + 0x01U)   /* reset input */
#define MD_SENDCOMPLETE      (MD_STATUSBASE + 0x02U)   /* send data complete */
#define MD_OVF         (MD_STATUSBASE + 0x03U)   /* timer count overflow */
#define MD_SPT         (MD_STATUSBASE + 0x04U)   /* IIC stop */
#define MD_NACK         (MD_STATUSBASE + 0x05U)   /* IIC no ACK */
#define MD_SLAVE_SEND_END   (MD_STATUSBASE + 0x06U)   /* IIC slave send end */
#define MD_SLAVE_RCV_END   (MD_STATUSBASE + 0x07U)   /* IIC slave receive end */
#define MD_MASTER_SEND_END   (MD_STATUSBASE + 0x08U)   /* IIC master send end */
#define MD_MASTER_RCV_END   (MD_STATUSBASE + 0x09U)   /* IIC master receive end */
#define MD_BUSY         (MD_STATUSBASE + 0x0AU)   /* IIC is busy */
#define MD_UNDEREXEC      (MD_STATUSBASE + 0x0BU)   /* DMA transfer under execute */
#define MD_COMPLETED      (MD_STATUSBASE + 0x0CU)   /* DMA transfer completed */
#define MD_BUSY1      (MD_STATUSBASE + 0x0DU)   /* busy 1 */
#define MD_BUSY2      (MD_STATUSBASE + 0x0EU)   /* busy 2 */

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


