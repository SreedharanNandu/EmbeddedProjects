/******************************************************************************
*******************************************************************************
** Project Name        :
** File Name           : HAL_CAN_API.h
** Description         : This module is used to configure the CAN 
** Date                : 
** Hardware Platform   : 
******************************************************************************/
#ifndef _HAL_CAN_API_H
#define _HAL_CAN_API_H

/******************************************************************************
**               P R O J E C T        I N C L U D E S
******************************************************************************/
/*list all include files*/

#include "HAL_CAN_Config.h"
/******************************************************************************
**               S T A R T      G L O B A L      D E F I N I T I O N 
******************************************************************************/

#include "system_def.h"

/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*list all global macros*/

#define CHECK_CGMCTRL_GOM_BIT     0x0001
#define ENABLE_CGMCTRL_GOM_BIT    0x0100
#define CCTRL_PSMODE              0x0018
#define CCTRL_STOPMODE            0x0018
#define CCTRL_STOP_CLR            0x0010
#define CCTRL_SLEEPMODE           0x0008
#define CCTRL_OPMODE              0x0007

#define ENABLE_CGMCTRL_EFSD_BIT   0x0200
#define ENABLE_CGMCS_REG          0x00
#define ENABLE_CBRP_REG           0x07
#define ENABLE_CBTR_REG           0x120D
#define ENABLE_TX_Inter_CMP       0x0100
#define ENABLE_RX_Inter_CMP       0x0200
#define ENABLE_ERR_Inter_CMP      0x0400
#define ENABLE_WUP_Inter_CMP      0x2000
#define ENABLE_Rx_CMASK1H_ID      0x01FF
#define ENABLE_Rx_CMASK1L_ID      0xFFFF
#define SET_CCERC_BIT             0x8000
#define Clr_Valid_BIT             0x0020
#define SET_NORMAL_CAN_MODE       0x0106

#define SET_MCTRL_RDY             0x0100
#define SET_MCTRL_TRQ             0x0200
#define SET_MCTRL_DN              0x0400
#define SET_MCTRL_IE              0x0800
#define Clr_MCTRL_RDY             0x0001
#define Clr_MCTRL_TRQ             0x0002
#define Clr_MCTRL_DN              0x0004
#define Clr_MCTRL_IE              0x0008
#define Clr_MCTRL_MOW             0x0010

#define SET_CMDLC_BIT             0x08
#define Clr_CMDB0_REG             0x00
#define Clr_MCONF_RTR             0x00







#define NUMBER_OF_HW_OBJECTS             0x10
#define CanModuleAdr                     0x0F05D0
#define CanMacroAdr                      0x0F05C0
#define CanMessageAdr                    0x0F0600
#define CAN_TX_SLOT                      0

/* MCTRL */
#define kMCTRL_RDY                  ((u16)0x0001u)
#define kMCTRL_TRQ                  ((u16)0x0002u)
#define kMCTRL_DN                   ((u16)0x0004u)
#define kMCTRL_IE                   ((u16)0x0008u)
#define kMCTRL_MOW                  ((u16)0x0010u)
#define kSetMCTRL_RDY               ((u16)0x0100u)
#define kSetMCTRL_TRQ               ((u16)0x0200u)
#define kSetMCTRL_DN                ((u16)0x0400u)
#define kSetMCTRL_IE                ((u16)0x0800u)
#define kClearMCTRL_RDY             ((u16)0x0001u)
#define kClearMCTRL_TRQ             ((u16)0x0002u)
#define kClearMCTRL_DN              ((u16)0x0004u)
#define kClearMCTRL_IE              ((u16)0x0008u)
#define kClearMCTRL_MOW             ((u16)0x0010u)



//#define CAN_BRP     0x001D     // divide by 30 for 33.3kbps 
#define CAN_BRP     0x0001     // divide by  1 for 500kbps
#define CAN_BTR     0x220B     //sjw = 3tq,
                               //tseg2=(setting is 2)3tq,
                               //tseg1=(setting is 12)12tq 
                               //FOR - 33.3kbps
                                 //i.e 1+12+3 tq is 16tq so 16Mhz/30/16 = 33.3kbps;
                                 //sp = tseg2/(sync+prop+pseg1+pseg2) = (1- (3/(1+12+3)) = 81.25%
                               //FOR - 500kbps
                                 //i.e 1+12+3 tq is 16tq so 16Mhz/2/16 = 500kbps;
                                 //sp = tseg2/(sync+prop+pseg1+pseg2) = (1- (3/(1+12+3)) = 81.25%

/* Macro for CAN hardware register access */
#define CAN_REG(type, address)  (*(volatile type __near *)((u16)(address)))
/* Message Control Register  00 */
#define MCTRL00                 (CAN_REG(u16, (CanMessageAdr + 0x0Eu)))




/******************************************************************************
**               D A T A
******************************************************************************/
/*list all global variables*/



/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************/
/*list all global functions*/




#endif /* for #define _HAL_CAN_API_H */


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


