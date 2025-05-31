/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_CAN.C
** Description          : This module is used to configure the CAN
** Date                 : 
** Hardware Platform    : 
******************************************************************************/

/* Define to include local variables in the header file */


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/
/*<list all system include files>*/

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
#include "HAL_CAN_API.h"
#include "HAL_CAN_Config.h"
#include "io78f1827_48.h"
#include <io78f1827_48_ext.h>
#include "system_def.h"
#include "EAL_CAN.h"

/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/
/*<list all local macro declarations> */

/* CAN message register                    */
#define M   ((tsMessage __near *)((u16)CanMessageAdr))  //Can message data byte register page 901

/* Bit access for global macro control     */
/* CGMCTRL                                 */
#define kCGMCTRL_GOM                       ((u16)0x0001u)
#define kCGMCTRL_EFSD                      ((u16)0x0002u)
#define kCGMCTRL_MBON                      ((u16)0x8000u)
#define kClearCGMCTRL_GOM                  ((u16)0x0001u)
#define kSetCGMCTRL_GOM                    ((u16)0x0100u)
#define kSetCGMCTRL_EFSD                   ((u16)0x0200u)
/* CGMCS                                   */
#define kCGMCS_CCP                         0x0f
/* CGMABT                                  */
#define kCGMABT_ABTTRG                     ((u16)0x0001u)
#define kCGMABT_ABTCLR                     ((u16)0x0002u)
#define kSetCGMABT_ABTTRG                  ((u16)0x0100u)
#define kSetCGMABT_ABTCLR                  ((u16)0x0200u)
#define kClearCGMABT_ABTTRG                ((u16)0x0001u)
/* CGMABTD                                 */
#define kCGMABTD_ABTD                      0x0f
/* CAN module register                     */
/* CnCTRL                                  */
#define kCnCTRL_OPMODE                     ((u16)0x0007u)
#define kCnCTRL_OPMODE_Init                ((u16)0x0000u)
#define kCnCTRL_OPMODE_Normal              ((u16)0x0001u)
#define kCnCTRL_OPMODE_Normal_with_ABT     ((u16)0x0002u)
#define kCnCTRL_OPMODE_RX_only             ((u16)0x0003u)
#define kCnCTRL_OPMODE_Single              ((u16)0x0004u)
#define kCnCTRL_OPMODE_SelfTest            ((u16)0x0005u)
#define kCnCTRL_PSMODE                     ((u16)0x0018u)
#define kCnCTRL_PSMODE_Sleep               ((u16)0x0008u)
#define kCnCTRL_PSMODE_Stop                ((u16)0x0018u)
#define kCnCTRL_VALID                      ((u16)0x0020u)
#define kCnCTRL_AL                         ((u16)0x0040u)
#define kCnCTRL_CCERC                      ((u16)0x0080u)
#define kCnCTRL_TSTAT                      ((u16)0x0100u)
#define kCnCTRL_RSTAT                      ((u16)0x0200u)
#define kSetCnCTRL_OPMODE_Normal           ((u16)0x0106u)
#define kSetCnCTRL_OPMODE_Normal_with_ABT  ((u16)0x0205u)
#define kSetCnCTRL_OPMODE_RX_only          ((u16)0x0304u)
#define kSetCnCTRL_OPMODE_Single           ((u16)0x0403u)
#define kSetCnCTRL_OPMODE_SelfTest         ((u16)0x0502u)
#define kSetCnCTRL_PSMODE_Sleep            ((u16)0x0800u)
#define kSetCnCTRL_PSMODE_Stop             ((u16)0x1000u)
#define kSetCnCTRL_AL                      ((u16)0x4000u)
#define kSetCnCTRL_CCERC                   ((u16)0x8000u)
#define kClearCnCTRL_OPMODE                ((u16)0x0007u)
#define kClearCnCTRL_PSMODE_Sleep          ((u16)0x0008u)
#define kClearCnCTRL_PSMODE_Stop           ((u16)0x0010u)
#define kClearCnCTRL_VALID                 ((u16)0x0020u)
#define kClearCnCTRL_AL                    ((u16)0x0040u)

/* CnLEC                                   */
#define kCnLEC_LEC                         ((u16)0x0007u)
#define kClearCnLEC_LEC                    ((u16)0x0000u)
/* CnINFO                                  */
#define kCnINFO_RECS                       ((u16)0x0003u)
#define kCnINFO_TECS                       ((u16)0x000cu)
#define kCnINFO_TECS_Warning               ((u16)0x0004u)
#define kCnINFO_TECS_Passive               ((u16)0x000cu)
#define kCnINFO_RECS_Warning               ((u16)0x0001u)
#define kCnINFO_RECS_Passive               ((u16)0x0003u)
#define kCnINFO_BOFF                       ((u16)0x0010u)
/* CnERC                                   */
#define kCnERC_TEC                         ((u16)0x00ffu)
#define kCnERC_REC                         ((u16)0xff00u)
/* CnIE                                    */
#define kCnIE_CIE0                         ((u16)0x0001u)
#define kCnIE_CIE1                         ((u16)0x0002u)
#define kCnIE_CIE2                         ((u16)0x0004u)
#define kCnIE_CIE3                         ((u16)0x0008u)
#define kCnIE_CIE4                         ((u16)0x0010u)
#define kCnIE_CIE5                         ((u16)0x0020u)
#define kSetCnIE_CIE0                      ((u16)0x0100u)
#define kSetCnIE_CIE1                      ((u16)0x0200u)
#define kSetCnIE_CIE2                      ((u16)0x0400u)
#define kSetCnIE_CIE3                      ((u16)0x0800u)
#define kSetCnIE_CIE4                      ((u16)0x1000u)
#define kSetCnIE_CIE5                      ((u16)0x2000u)
#define kClearCnIE_CIE0                    ((u16)0x0001u)
#define kClearCnIE_CIE1                    ((u16)0x0002u)
#define kClearCnIE_CIE2                    ((u16)0x0004u)
#define kClearCnIE_CIE3                    ((u16)0x0008u)
#define kClearCnIE_CIE4                    ((u16)0x0010u)
#define kClearCnIE_CIE5                    ((u16)0x0020u)
/* CnINTS                                  */
#define kCnINTS_CINTS0                     ((u16)0x0001u)
#define kCnINTS_CINTS1                     ((u16)0x0002u)
#define kCnINTS_CINTS2                     ((u16)0x0004u)
#define kCnINTS_CINTS3                     ((u16)0x0008u)
#define kCnINTS_CINTS4                     ((u16)0x0010u)
#define kCnINTS_CINTS5                     ((u16)0x0020u)
#define kClearCnINTS_CINTS0                ((u16)0x0001u)
#define kClearCnINTS_CINTS1                ((u16)0x0002u)
#define kClearCnINTS_CINTS2                ((u16)0x0004u)
#define kClearCnINTS_CINTS3                ((u16)0x0008u)
#define kClearCnINTS_CINTS4                ((u16)0x0010u)
#define kClearCnINTS_CINTS5                ((u16)0x0020u)
/* CnBRP                                   */
#define kCnBRP                             ((u8)0xff)
/* CnBTR                                   */
#define kCnBTR_TSEG                        ((u16)0x070fu)
#define kCnBTR_SJW                         ((u16)0x3000u)
/* CnLIPT                                  */
#define kCnLIPT                            ((u8)0xff)
#define kClearCnLIPT                       ((u8)0x00)
/* CnRGPT                                  */
#define kCnRGPT_ROVF                       ((u16)0x0001u)
#define kCnRGPT_RHPM                       ((u16)0x0002u)
#define kCnRGPT_RGPT                       ((u16)0xff00u)
#define kClearCnRGPT_ROVF                  ((u16)0x0001u)
/* CnLOPT                                  */
#define kCnLOPT_LOPT                       ((u8)0xff)
/* CnTGPT                                  */
#define kCnTGPT_TOVF                       ((u16)0x0001u)
#define kCnTGPT_THPM                       ((u16)0x0002u)
#define kCnTGPT_TGPT                       ((u16)0xff00u)
#define kClearCnTGPT_TOVF                  ((u16)0x0001u)
/* CnTS                                    */
#define kCnTS_TSEN                         ((u16)0x0001u)
#define kCnTS_TSSEL                        ((u16)0x0002u)
#define kCnTS_TSLOCK                       ((u16)0x0004u)
#define kSetCnTS_TSEN                      ((u16)0x0100u)
#define kSetCnTS_TSSEL                     ((u16)0x0200u)
#define kSetCnTS_TSLOCK                    ((u16)0x0400u)
#define kClearCnTS_TSEN                    ((u16)0x0001u)
#define kClearCnTS_TSSEL                   ((u16)0x0002u)
#define kClearCnTS_TSLOCK                  ((u16)0x0004u)
/* CAN message register                    */
/* MDLC                                    */
#define kMDLC_MDLC                         ((u8)0x0f)
/* MCONF                                   */
#define kMCONF_MA                          ((u8)0x07)
#define kMCONF_MT                          ((u8)0x38)
#define kMCONF_MT_RX_FULL                  ((u8)0x08)
#define kMCONF_RTR                         ((u8)0x40)
#define kMCONF_OWS                         ((u8)0x80)
#define kSetMCONF_MA_CH1                   ((u8)0x01)
#define kSetMCONF_MA_CH2                   ((u8)0x02)
#define kSetMCONF_MA_CH3                   ((u8)0x03)
#define kSetMCONF_MA_CH4                   ((u8)0x04)
#define kSetMCONF_MA_CH5                   ((u8)0x05)
#define kSetMCONF_MA_CH6                   ((u8)0x06)
#define kSetMCONF_MA_CH7                   ((u8)0x07)
#define kSetMCONF_MT_TX                    ((u8)0x00)
#define kSetMCONF_MT_RX_FULL               ((u8)0x08)
#define kSetMCONF_MT_RX_MASK1              ((u8)0x10)
#define kSetMCONF_MT_RX_MASK2              ((u8)0x18)
#define kSetMCONF_MT_RX_MASK3              ((u8)0x20)
#define kSetMCONF_MT_RX_MASK4              ((u8)0x28)
#define kSetMCONF_RTR                      ((u8)0x40)
#define kSetMCONF_OWS                      ((u8)0x80)

/* FX3 78k0R */
#define CGMCTRL   (CAN_REG(u16, (CanMacroAdr + 0x00u)))    /* CAN Global Macro Control Register                            */
#define CGMABT    (CAN_REG(u16, (CanMacroAdr + 0x06u)))    /* CAN Global Macro Automatic Block Transmission Register       */
#define CGMCS     (CAN_REG(u8,  (CanMacroAdr + 0x0Eu)))    /* CAN Global Macro Clock Selection Register                    */
#define CGMABTD   (CAN_REG(u8,  (CanMacroAdr + 0x0Fu)))    /* CAN Global Macro Automatic Block Transmission Delay Register */

/* Registers of the CAN Module */
#define CnMASK1L  (CAN_REG(u16, (CanModuleAdr + 0x00u)))   /* CAN Module Mask 1 Low Register                               */
#define CnMASK1H  (CAN_REG(u16, (CanModuleAdr + 0x02u)))   /* CAN Module Mask 1 High Register                              */
#define CnMASK2L  (CAN_REG(u16, (CanModuleAdr + 0x04u)))   /* CAN Module Mask 2 Low Register                               */
#define CnMASK2H  (CAN_REG(u16, (CanModuleAdr + 0x06u)))   /* CAN Module Mask 2 High Register                              */
#define CnMASK3L  (CAN_REG(u16, (CanModuleAdr + 0x08u)))   /* CAN Module Mask 3 Low Register                               */
#define CnMASK3H  (CAN_REG(u16, (CanModuleAdr + 0x0Au)))   /* CAN Module Mask 3 High Register                              */
#define CnMASK4L  (CAN_REG(u16, (CanModuleAdr + 0x0Cu)))   /* CAN Module Mask 4 Low Register                               */
#define CnMASK4H  (CAN_REG(u16, (CanModuleAdr + 0x0Eu)))   /* CAN Module Mask 4 High Register                              */
#define CnCTRL    (CAN_REG(u16, (CanModuleAdr + 0x10u)))   /* CAN Module Control Register                                  */
#define CnLEC     (CAN_REG(u8,  (CanModuleAdr + 0x12u)))   /* CAN Module Last Error Code Register                          */
#define CnINFO    (CAN_REG(u8,  (CanModuleAdr + 0x13u)))   /* CAN Module Information Register                              */
#define CnERC     (CAN_REG(u16, (CanModuleAdr + 0x14u)))   /* CAN Module Error Counter                                     */
#define CnIE      (CAN_REG(u16, (CanModuleAdr + 0x16u)))   /* CAN Module Interrupt Enable Register                         */
#define CnINTS    (CAN_REG(u16, (CanModuleAdr + 0x18u)))   /* CAN Module Interrupt Status Register                         */
#define CnBRP     (CAN_REG(u8,  (CanModuleAdr + 0x1Au)))   /* CAN Module Bit-Rate Prescaler Register                       */
#define CnBTR     (CAN_REG(u16, (CanModuleAdr + 0x1Cu)))   /* CAN Module Bit Rate Register                                 */
#define CnLIPT    (CAN_REG(u8,  (CanModuleAdr + 0x1Fu)))   /* CAN Module Last In-Pointer Register                          */
#define CnRGPT    (CAN_REG(u16, (CanModuleAdr + 0x20u)))   /* CAN Module Receive History List Get Pointer Register         */
#define CnLOPT    (CAN_REG(u8,  (CanModuleAdr + 0x22u)))   /* CAN Module Last Out-Pointer Register                         */
#define CnTGPT    (CAN_REG(u16, (CanModuleAdr + 0x24u)))   /* CAN Module Transmit History List Get Pointer Register        */
#define CnTS      (CAN_REG(u16, (CanModuleAdr + 0x26u)))   /* CAN Module Time Stamp Register                               */

/* Registers of Message Buffers */
#define MIDL00    (CAN_REG(u16, (CanMessageAdr + 0x0Au)))  /* Message Identifier Low Register  00                          */
#define MIDL01    (CAN_REG(u16, (CanMessageAdr + 0x1Au)))  /* Message Identifier Low Register  01                          */
#define MIDL02    (CAN_REG(u16, (CanMessageAdr + 0x2Au)))  /* Message Identifier Low Register  02                          */
#define MIDL03    (CAN_REG(u16, (CanMessageAdr + 0x3Au)))  /* Message Identifier Low Register  03                          */
#define MIDL04    (CAN_REG(u16, (CanMessageAdr + 0x4Au)))  /* Message Identifier Low Register  04                          */
#define MIDL05    (CAN_REG(u16, (CanMessageAdr + 0x5Au)))  /* Message Identifier Low Register  05                          */
#define MIDL06    (CAN_REG(u16, (CanMessageAdr + 0x6Au)))  /* Message Identifier Low Register  06                          */
#define MIDL07    (CAN_REG(u16, (CanMessageAdr + 0x7Au)))  /* Message Identifier Low Register  07                          */
#define MIDL08    (CAN_REG(u16, (CanMessageAdr + 0x8Au)))  /* Message Identifier Low Register  08                          */
#define MIDL09    (CAN_REG(u16, (CanMessageAdr + 0x9Au)))  /* Message Identifier Low Register  09                          */
#define MIDL10    (CAN_REG(u16, (CanMessageAdr + 0xAAu)))  /* Message Identifier Low Register  10                          */
#define MIDL11    (CAN_REG(u16, (CanMessageAdr + 0xBAu)))  /* Message Identifier Low Register  11                          */
#define MIDL12    (CAN_REG(u16, (CanMessageAdr + 0xCAu)))  /* Message Identifier Low Register  12                          */
#define MIDL13    (CAN_REG(u16, (CanMessageAdr + 0xDAu)))  /* Message Identifier Low Register  13                          */
#define MIDL14    (CAN_REG(u16, (CanMessageAdr + 0xEAu)))  /* Message Identifier Low Register  14                          */
#define MIDL15    (CAN_REG(u16, (CanMessageAdr + 0xFAu)))  /* Message Identifier Low Register  15                          */

#define MIDH00    (CAN_REG(u16, (CanMessageAdr + 0x0Cu)))  /* Message Identifier High Register  00                         */
#define MIDH01    (CAN_REG(u16, (CanMessageAdr + 0x1Cu)))  /* Message Identifier High Register  01                         */
#define MIDH02    (CAN_REG(u16, (CanMessageAdr + 0x2Cu)))  /* Message Identifier High Register  02                         */
#define MIDH03    (CAN_REG(u16, (CanMessageAdr + 0x3Cu)))  /* Message Identifier High Register  03                         */
#define MIDH04    (CAN_REG(u16, (CanMessageAdr + 0x4Cu)))  /* Message Identifier High Register  04                         */
#define MIDH05    (CAN_REG(u16, (CanMessageAdr + 0x5Cu)))  /* Message Identifier High Register  05                         */
#define MIDH06    (CAN_REG(u16, (CanMessageAdr + 0x6Cu)))  /* Message Identifier High Register  06                         */
#define MIDH07    (CAN_REG(u16, (CanMessageAdr + 0x7Cu)))  /* Message Identifier High Register  07                         */
#define MIDH08    (CAN_REG(u16, (CanMessageAdr + 0x8Cu)))  /* Message Identifier High Register  08                         */
#define MIDH09    (CAN_REG(u16, (CanMessageAdr + 0x9Cu)))  /* Message Identifier High Register  09                         */
#define MIDH10    (CAN_REG(u16, (CanMessageAdr + 0xACu)))  /* Message Identifier High Register  10                         */
#define MIDH11    (CAN_REG(u16, (CanMessageAdr + 0xBCu)))  /* Message Identifier High Register  11                         */
#define MIDH12    (CAN_REG(u16, (CanMessageAdr + 0xCCu)))  /* Message Identifier High Register  12                         */
#define MIDH13    (CAN_REG(u16, (CanMessageAdr + 0xDCu)))  /* Message Identifier High Register  13                         */
#define MIDH14    (CAN_REG(u16, (CanMessageAdr + 0xECu)))  /* Message Identifier High Register  14                         */
#define MIDH15    (CAN_REG(u16, (CanMessageAdr + 0xFCu)))  /* Message Identifier High Register  15                         */

#define MCTRL01   (CAN_REG(u16, (CanMessageAdr + 0x1Eu)))  /* Message Control Register  01                                 */
#define MCTRL02   (CAN_REG(u16, (CanMessageAdr + 0x2Eu)))  /* Message Control Register  02                                 */
#define MCTRL03   (CAN_REG(u16, (CanMessageAdr + 0x3Eu)))  /* Message Control Register  03                                 */
#define MCTRL04   (CAN_REG(u16, (CanMessageAdr + 0x4Eu)))  /* Message Control Register  04                                 */
#define MCTRL05   (CAN_REG(u16, (CanMessageAdr + 0x5Eu)))  /* Message Control Register  05                                 */
#define MCTRL06   (CAN_REG(u16, (CanMessageAdr + 0x6Eu)))  /* Message Control Register  06                                 */
#define MCTRL07   (CAN_REG(u16, (CanMessageAdr + 0x7Eu)))  /* Message Control Register  07                                 */
#define MCTRL08   (CAN_REG(u16, (CanMessageAdr + 0x8Eu)))  /* Message Control Register  08                                 */
#define MCTRL09   (CAN_REG(u16, (CanMessageAdr + 0x9Eu)))  /* Message Control Register  09                                 */
#define MCTRL10   (CAN_REG(u16, (CanMessageAdr + 0xAEu)))  /* Message Control Register  10                                 */
#define MCTRL11   (CAN_REG(u16, (CanMessageAdr + 0xBEu)))  /* Message Control Register  11                                 */
#define MCTRL12   (CAN_REG(u16, (CanMessageAdr + 0xCEu)))  /* Message Control Register  12                                 */
#define MCTRL13   (CAN_REG(u16, (CanMessageAdr + 0xDEu)))  /* Message Control Register  13                                 */
#define MCTRL14   (CAN_REG(u16, (CanMessageAdr + 0xEEu)))  /* Message Control Register  14                                 */
#define MCTRL15   (CAN_REG(u16, (CanMessageAdr + 0xFEu)))  /* Message Control Register  15     

* CAN Hardware Objects */
#define TxBuf  0
#define RxFull 1
#define RxBasi 2
#define NoUse  3

/*CAN HAL Buffer*/
CAN_RX_BUFF_S CanHALRxobject;

typedef struct
{
   u8 swObjNumber;
   u8 bufferType;
} tCanHwAssignment;

tCanHwAssignment const kCanHwAssignment[NUMBER_OF_HW_OBJECTS] =
{
    0,TxBuf   ,1,TxBuf,  2,TxBuf,    
    3,TxBuf   ,4,TxBuf,  5,TxBuf,     
    0,RxFull  ,1,RxFull, 2,RxFull,   
    3,NoUse   ,4,NoUse,  0,NoUse,    
    0,NoUse   ,0,NoUse,  0,NoUse ,0,NoUse   
};


/******************************************************************************
                   D A T A
******************************************************************************/

typedef volatile struct tsMessage
{
   volatile u8  DATA[8];      /* Message Data Byte 0..7 Register   */
   volatile u8  DLC;          /* Message Data Length Code Register */
   volatile u8  CONF;         /* Message Configuration Register    */
   volatile u16 IDL;          /* Message Identifier Low Register   */
   volatile u16 IDH;          /* Message Identifier High Register  */
   volatile u16 CTRL;         /* Message Control Register          */
} tsMessage;



static u32 SupportedRxCANID[4]= {
                                 SET_EXT_ID(CAN_ID_20E),
                                 SET_EXT_ID(CAN_ID_20C),
                                 SET_EXT_ID(CAN_ID_396),
                                 SET_EXT_ID(CAN_ID_397)
                                };
/******************************************************************************
**                  F U N C T I O N     D E F I N I T I O N S
******************************************************************************/
 u8 bCANReceptionFlag;
 
/******************************************************************************
* Name         :  CanWriteMCTRL
* Called by    :  CanInit, CanTransmit, CanMsgTransmitted,
*                 CanProcessRxMsg
* Preconditions:  None
* Parameters   :  object (in)     : index of hardware message object
*                 mctrlState (in) : new value of control register
* Return code  :  None
* Description  :  Write to CAN message control register XX
******************************************************************************/
void CanWriteMCTRL(u8 object, u16 mctrlState)
{
   switch(object)
   {
      case 0:
         MCTRL00 = mctrlState;
      break;
      case 1:
         MCTRL01 = mctrlState;
      break;
      case 2:
         MCTRL02 = mctrlState;
      break;
      case 3:
         MCTRL03 = mctrlState;
      break;
      case 4:
         MCTRL04 = mctrlState;
      break;
      case 5:
         MCTRL05 = mctrlState;
      break;
      case 6:
         MCTRL06 = mctrlState;
      break;
      case 7:
         MCTRL07 = mctrlState;
      break;
      case 8:
         MCTRL08 = mctrlState;
      break;
      case 9:
         MCTRL09 = mctrlState;
      break;
      case 10:
         MCTRL10 = mctrlState;
      break;
      case 11:
         MCTRL11 = mctrlState;
      break;
      case 12:
         MCTRL12 = mctrlState;
      break;
      case 13:
         MCTRL13 = mctrlState;
      break;
      case 14:
         MCTRL14 = mctrlState;
      break;
      case 15:
         MCTRL15 = mctrlState;
      break;
         default:
      break;
   }
}

/******************************************************************************
* Name         :  CanGetMCTRL
* Called by    :  CanInit, CanTransmit, CanMsgTransmitted
* Preconditions:  None
* Parameters   :  object (in) : index of hardware message object
* Return code  :  Current value of CAN message control register
* Description  :  Return value of CAN message control register XX
******************************************************************************/
u16 CanGetMCTRL(u8 object)
{
   switch(object)
   {
      case 0:
         return MCTRL00;
      case 1:
         return MCTRL01;
      case 2:
         return MCTRL02;
      case 3:
         return MCTRL03;
      case 4:
         return MCTRL04;
      case 5:
         return MCTRL05;
      case 6:
         return MCTRL06;
      case 7:
         return MCTRL07;
      case 8:
         return MCTRL08;
      case 9:
         return MCTRL09;
      case 10:
         return MCTRL10;
      case 11:
         return MCTRL11;
      case 12:
         return MCTRL12;
      case 13:
         return MCTRL13;
      case 14:
         return MCTRL14;
      case 15:
         return MCTRL15;
      default:
         return 0;
   }
}

/******************************************************************************
* Name         :  CanWriteCANId
* Called by    :  CanInit, CanTransmit
* Preconditions:  None
* Parameters   :  object (in) : index of hardware message object
*                 IDL (in)    : low part of CAN ID (extended ID only)
*                 IDH (in)    : CAN ID (extended ID: high part)
* Return code  :  None
* Description  :  Write to CAN message ID register XX
******************************************************************************/
void CanWriteCANId(u8 object, u32 ID)
{
   //16 message  buffers so 16 can ids
   switch(object)
   {
      case 0:
         MIDL00 = (u16)(ID & MSK_CANIDL);
         MIDH00 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 1:
         MIDL01 = (u16)(ID & MSK_CANIDL);
         MIDH01 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 2:
         MIDL02 = (u16)(ID & MSK_CANIDL);
         MIDH02 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 3:
         MIDL03 = (u16)(ID & MSK_CANIDL);
         MIDH03 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 4:
         MIDL04 = (u16)(ID & MSK_CANIDL);
         MIDH04 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 5:
         MIDL05 = (u16)(ID & MSK_CANIDL);
         MIDH05 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 6:
         MIDL06 = (u16)(ID & MSK_CANIDL);
         MIDH06 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 7:
         MIDL07 = (u16)(ID & MSK_CANIDL);
         MIDH07 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 8:
         MIDL08 = (u16)(ID & MSK_CANIDL);
         MIDH08 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 9:
         MIDL09 = (u16)(ID & MSK_CANIDL);
         MIDH09 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 10:
         MIDL10 = (u16)(ID & MSK_CANIDL);
         MIDH10 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 11:
         MIDL11 = (u16)(ID & MSK_CANIDL);
         MIDH11 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 12:
         MIDL12 = (u16)(ID & MSK_CANIDL);
         MIDH12 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 13:
         MIDL13 = (u16)(ID & MSK_CANIDL);
         MIDH13 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 14:
         MIDL14 = (u16)(ID & MSK_CANIDL);
         MIDH14 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      case 15:
         MIDL15 = (u16)(ID & MSK_CANIDL);
         MIDH15 = (u16)((ID & MSK_CANIDH) >> 16);
         break;
      default:
         break;
   }
}
 
/******************************************************************************
* Name         :  CanReadCANId
* Called by    :  CanProcessRxMsg
* Preconditions:  None
* Parameters   :  object (in) : index of hardware message object
*                 IDL (out)   : low part of CAN ID (extended ID only)
*                 IDH (out)   : CAN ID (extended ID: high part)
* Return code  :  None
* Description  :  Read CAN message ID register XX into IDH and/or IDL
******************************************************************************/
void CanReadCANId(u8 object, u32 *ID)
{
   switch(object)
   {
      case 0:
         *ID = (u32)((u32)MIDH00<<16);
         *ID = (u32)(*ID|MIDL00);
         break;
      case 1:
        *ID = (u32)((u32)MIDH01<<16);
        *ID = (u32)(*ID|MIDL01);
         break;
      case 2:
         *ID = (u32)((u32)MIDH02<<16);
         *ID = (u32)(*ID|MIDL02);
         break;
      case 3:
         *ID = (u32)((u32)MIDH03<<16);
         *ID = (u32)(*ID|MIDL03);
         break;
      case 4:
         *ID = (u32)((u32)MIDH04<<16);
         *ID = (u32)(*ID|MIDL04);
         break;
      case 5:
         *ID = (u32)((u32)MIDH05<<16);
         *ID = (u32)(*ID|MIDL05);
         break;
      case 6:
        *ID = (u32)((u32)MIDH06<<16);
        *ID = (u32)(*ID|MIDL06);
         break;
      case 7:
         *ID = (u32)((u32)MIDH07<<16);
         *ID = (u32)(*ID|MIDL07);
         break;
      case 8:
         *ID = (u32)((u32)MIDH08<<16);
         *ID = (u32)(*ID|MIDL08);
         break;
      case 9:
         *ID = (u32)((u32)MIDH09<<16);
          *ID = (u32)(*ID|MIDL09);
         break;
      case 10:
        *ID = (u32)((u32)MIDH10<<16);
        *ID = (u32)(*ID|MIDL10);
         break;
      case 11:
         *ID = (u32)((u32)MIDH11<<16);
         *ID = (u32)(*ID|MIDL11);
         break;
      case 12:
        *ID = (u32)((u32)MIDH12<<16);
         *ID = (u32)(*ID|MIDL12);
         break;
      case 13:
        *ID = (u32)((u32)MIDH13<<16);
        *ID = (u32)(*ID|MIDL13);
         break;
      case 14:
        *ID = (u32)((u32)MIDH14<<16);
        *ID = (u32)(*ID|MIDL14);
         break;
      case 15:
        *ID = (u32)((u32)MIDH15<<16);
        *ID = (u32)(*ID|MIDL15);
         break;
      default:
         break;
   }
}

/******************************************************************************
* Name         :  CanInit
* Called by    :  main
* Preconditions:  None
* Parameters   :  None
* Return code  :  None
* Description  :  Initialization of the CAN registers
******************************************************************************/
void CanInit( void )
{
   u8 hwObjHandle; //this is the 16 can message data buffers
   u8 bufferType;
   u8 swObjHandle;

   HAL_CAN_Transceiver_Init();
   
   /* Check if global macro is switched on */
   if(!(CGMCTRL & kCGMCTRL_GOM))
   {
      /* Global macro is disabled -> Do basic set-ups which are only allowed with */
      /* disabled global macro */
      /* Set-up clock selection register */
      CGMCS = 0x00; /*fCAN/1, fCAN: Clock supplied to CAN (fMAIN)*/
      /* Set GOM bit to enable global CAN macro */
      CGMCTRL = kSetCGMCTRL_GOM;
   }

   /* Check if CAN module is in sleep or other power down mode */
   if(CnCTRL & kCnCTRL_PSMODE)
   {
      /* Cancel stop mode if set */
      if((CnCTRL & kCnCTRL_PSMODE_Stop) == kCnCTRL_PSMODE_Stop)
      {
         CnCTRL = kClearCnCTRL_PSMODE_Stop;
      }
      /* Cancel sleep mode if set */
      if((CnCTRL & kCnCTRL_PSMODE_Sleep)==kCnCTRL_PSMODE_Sleep)
      {
         CnCTRL = kClearCnCTRL_PSMODE_Sleep;
      }
      /* Clear all other modes */
      CnCTRL &= kCnCTRL_PSMODE;
   }

   /* Check if CAN module is in init mode */
   if(CnCTRL & kCnCTRL_OPMODE)
   {
      /* Module is in operation mode */
      /* Clear operation mode and wait for entering init mode */
      CnCTRL = kClearCnCTRL_OPMODE;
      while((CnCTRL & kCnCTRL_OPMODE)!=0) 
      {
      }
   }
   
  
   /* Now the global module is enabled and the CAN module is in init mode. */
   /* Now try to initialize the CAN module. */
   /* Set bit-rate prescaler. */
   CnBRP = CAN_BRP;
   /* Set bit rate. */
   CnBTR = CAN_BTR;//3tq,12tq
 
   
   /* Disable interrupt pending flags. */
   CnINTS = 0x3Fu;
   
   CnIE  = 0x2Fu;  // to clear all  Interrupt flag
   
   CnIE  = 0x0300u;  // To set The interrupts only TX and Rx are enabled

   
   /* Set mask register for message reception. */
   CnMASK1L = 0x0000u;
   CnMASK1H = 0x0000u;
   CnMASK2L = 0x0000u;
   CnMASK2H = 0x0000u;
   CnMASK3L = 0x0000u;
   CnMASK3H = 0x0000u;
   CnMASK4L = 0x0000u;
   CnMASK4H = 0x0000u;

  /* Init all msg objects --------------------------------------------------------- */
   for (hwObjHandle=0; hwObjHandle<NUMBER_OF_HW_OBJECTS; hwObjHandle++ )
   {
      /* Clear possible remaining transmit request */
      CanWriteMCTRL(hwObjHandle, (u16)kClearMCTRL_TRQ);
      /* Set message buffer into init configuration */
      while(CanGetMCTRL(hwObjHandle) & kMCTRL_RDY)
      {
         /* Clear ready bit */
         CanWriteMCTRL(hwObjHandle, (u16)kClearMCTRL_RDY);
      }

      /* Set message buffer into init configuration */
      CanWriteMCTRL(hwObjHandle, (u16)(kClearMCTRL_TRQ|kClearMCTRL_DN|kClearMCTRL_MOW|kClearMCTRL_IE));
      M[hwObjHandle].CONF = 0;

      bufferType = kCanHwAssignment[hwObjHandle].bufferType;

      if(bufferType==TxBuf)
      {
         /* Set-up message buffer as transmit buffer */
         M[hwObjHandle].CONF = kSetMCONF_MA_CH1|kSetMCONF_MT_TX;
         /* Enable interrupt generation */
        CanWriteMCTRL(hwObjHandle, (u16)kSetMCTRL_IE);
        
      }
      else
      {
         if(bufferType==RxFull)
         {
            CanWriteMCTRL(hwObjHandle, (u16)(kClearMCTRL_RDY));           
            CanWriteMCTRL(hwObjHandle, (u16)(kClearMCTRL_TRQ | kClearMCTRL_DN | kClearMCTRL_MOW|kClearMCTRL_IE)); 
            
            swObjHandle = kCanHwAssignment[hwObjHandle].swObjNumber;
            /* Set-up message buffer as receive buffer */
            M[hwObjHandle].CONF = kSetMCONF_OWS|kSetMCONF_MA_CH1|kSetMCONF_MT_RX_FULL;
            /* Set-up  receive message ID */
            CanWriteCANId(hwObjHandle, (u32)SupportedRxCANID[(swObjHandle)]);
            
            /* Enable message buffer */
            CanWriteMCTRL(hwObjHandle, (u16)(kSetMCTRL_RDY|kSetMCTRL_IE));
            
         }
      }
   }

   /* Disable interrupt pending flags. */
   CnINTS = 0x3Fu;
   /* Clear valid bit */
   CnCTRL = kClearCnCTRL_VALID;

   /* Set module into normal operation mode */
   CnCTRL = kSetCnCTRL_OPMODE_Normal;
}

 
/******************************************************************************
* Function Name     : HAL_CAN_Trnsreciver_Init
* Input Params      : None
* Output Params     : None
* Description       : This function is used to initialize the CAN Driver
                      Intialization.
******************************************************************************/
void HAL_CAN_Transceiver_Init(void)
{

   
   /* To select the CAN port and set corresponsding bits */
   STSEL_bit.no7 = 0;
  
   P7_bit.no2 = 1;
   PM7_bit.no2 =0;
   PU7_bit.no2 =1;
  
   P7_bit.no3 =0;
   PM7_bit.no3 =1;
  
  
   /* To enable CAN operating clock */
   PCKSEL_bit.no4 =1;    
  
   /*  For enable Highlevel Interrupt registers*/
  
   // The CAN interrupt priorities are level 3 after reset
   // Enable the CAN interrupts
   MK2 = 0;
   //CANRx/Tx interrupt         -> 11(level 3) lowest
   C0RECPR1 = 1;
   C0RECPR0 = 1;
  
   C0TRXPR1 = 1;
   C0TRXPR0 = 1;
  
   IF0 = 0x0000u;
   IF1 = 0x0000u;
   IF2 = 0x0000u;
   IF3 = 0x0000u;

} 


/******************************************************************************
* Name         :  CanTransmit
* Called by    :  Application Layer
* Preconditions:  CAN interface must be initialized before call
* Parameters   :  Pointer to TransmitObject
* Return code  :  The function returns kCanTxOk, or kCanTxFailed in case of BusOff
* Description  :  This function transmits a CAN message if bus is free and not off
******************************************************************************/
u8 HAL_CAN_Transmit( CAN_TX_BUFF_S* msgObject , u8 slot)
{

   if(msgObject->is_ext_frm == EXTD_ID_TYPE)
   {
     msgObject->Msg_ID =  SET_EXT_ID(msgObject->Msg_ID); 
    
   }
   else
   {
      msgObject->Msg_ID = SET_STD_ID(msgObject->Msg_ID);
   }
   
   /* Check if buffer is free, synchronized, no BusOff */
   if ((CanGetMCTRL(slot) & (kMCTRL_RDY|kMCTRL_TRQ)))
   {
      /* Check if message transmitted */
      if(CanMsgTransmitted(slot) != kCanTxOk)
      {
        return kCanTxFailed;
      }
 
   }
   /* Check if hardware is in bus-off state */
   if((CnINFO & kCnINFO_BOFF)==kCnINFO_BOFF)
   {
      return kCanTxFailed;
   }

   /* Copy data into CAN buffer */

   CanWriteCANId(slot, msgObject->Msg_ID);
      
   M[slot].DLC     = msgObject->DLC;
   M[slot].DATA[0] = msgObject->data[0];
   M[slot].DATA[1] = msgObject->data[1];
   M[slot].DATA[2] = msgObject->data[2];
   M[slot].DATA[3] = msgObject->data[3];
   M[slot].DATA[4] = msgObject->data[4];
   M[slot].DATA[5] = msgObject->data[5];
   M[slot].DATA[6] = msgObject->data[6];
   M[slot].DATA[7] = msgObject->data[7];

   /* Set send request to transmit message on bus */
   CanWriteMCTRL(slot, (u16)kSetMCTRL_RDY);
   CanWriteMCTRL(slot, (u16)kSetMCTRL_TRQ);

   return kCanTxOk;
}

/******************************************************************************
* Name         :  CanMsgTransmitted
* Called by    :  Application/ CanTransmit
* Preconditions:  CAN interface must be initialized before call
* Parameters   :  None
* Return code  :  CAN message was transmitted or not
* Description  :  This function returns kCanTxOk if the message was
*                 transmitted.
******************************************************************************/
u8 CanMsgTransmitted( unsigned char slot )
{
   u8 returnCode = kCanTxFailed;

   if ((CanGetMCTRL(slot) & kMCTRL_RDY)  != 0)
   {
      if ((CanGetMCTRL(slot) & kMCTRL_TRQ) != 0)
      {
         /* Message transmission currently running */
         returnCode = kCanTxInProgress;
      }
      else
      {
         CanWriteMCTRL(slot, (u16)kClearMCTRL_RDY);
         /* Message transmitted */
         returnCode = kCanTxOk;
      }
   }
   return returnCode;
}

/******************************************************************************
* Name         :  CanTxCmplt
* Called by    :  
* Preconditions:  
* Parameters   :  
* Return code  :  
* Description  :  
******************************************************************************/
void CanTxCmplt(void)
{
   EAL_CAN_End_of_Tx();
}
/******************************************************************************
* Name         :  CanReceive
* Called by    :  Application
* Preconditions:  CAN hardware has to be initialized before call
* Parameters   :  None
* Return code  :  New message received (value !=0 )
* Description  :  Function searches for new received can messages
******************************************************************************/
void CanReceive(void)
{

   u16  tempCnRGPT;
   u16  rxObjHandle; 
   /* Check if hardware is in bus-off state */
   if((CnINTS & kCnINTS_CINTS3)!=0)
   {
      if((CnINFO & kCnINFO_BOFF)==kCnINFO_BOFF)
      {
         /* Set macro into init mode        */
         CnCTRL = kClearCnCTRL_OPMODE;
         while((CnCTRL & kCnCTRL_OPMODE)!=0)/*lint !e720 */
         {
         }
         /* Clear valid bit */
         CnCTRL = kClearCnCTRL_VALID;
         /* Set macro back into normal to start bus-off recovery sequence */
         CnCTRL = kSetCnCTRL_OPMODE_Normal;
         /* Clear Bus-Off interrupt */
         CnINTS = kClearCnINTS_CINTS3;
        
      }
      /* Clear Bus-Off interrupt */
      CnINTS = kClearCnINTS_CINTS3;
   }
   
   /* Search for receive object, which causes the interrupt */
   tempCnRGPT = CnRGPT;
   if(tempCnRGPT == 0x0001)
   {
      CnRGPT =  0x0001;
   }
      
   rxObjHandle = (u16)((tempCnRGPT&0xff00)>>8);
  
   /* Check data new flag in callback function */               

   /* Check if message buffer index is in the range of basic CAN message buffers and DN flag is still set */
   if((M[rxObjHandle].CTRL & kMCTRL_DN)==kMCTRL_DN)
   {
      CanProcessRxMsg((u8)rxObjHandle);
   }
   bCANReceptionFlag = 1;// Set status flag CAN (Message received)
      
}


/******************************************************************************
* Name         :  CanProcessRxMessage
* Called by    :  Application
* Preconditions:  CAN hardware has to be initialized before call
* Parameters   :  None
* Return code  :  None
* Description  :  Processing of the Id of the received CAN messages. After the
*                 Type of the message is determined, the right RxCanMsg-Handle
*                 is passed to HandleRxMsg()
*                 Finally the Receivebuffer is released.
******************************************************************************/
void CanProcessRxMsg( u8 index )
{
  u32 CanRxActualId; 
  u8 i;
   

   for(i=0;i<8;i++)
   {
      CanHALRxobject.data[i] = M[index].DATA[i];
   }
   CanHALRxobject.DLC =  M[index].DLC;

   CanReadCANId(index, &CanRxActualId);
   
   if((CanRxActualId & CANID_EXT) == CANID_EXT )
   {   
       CanHALRxobject.Msg_ID = GET_EXT_ID(CanRxActualId);
      
      CanHALRxobject.is_ext_frm = EXTD_ID_TYPE;
   }
   else
   {
     CanHALRxobject.Msg_ID = (u32)GET_STD_ID(CanRxActualId);
     CanHALRxobject.is_ext_frm = STD_ID_TYPE;
   }
   
   EAL_CAN_AddtoRxQueue(&CanHALRxobject);
   
   /* Release receive message object */
   CanWriteMCTRL(index, (u16)kClearMCTRL_DN);
  
 


   
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

