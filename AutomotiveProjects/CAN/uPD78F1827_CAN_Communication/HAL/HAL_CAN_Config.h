/******************************************************************************
*******************************************************************************
** Project Name         :
** File Name            : HAL_CAN_Config.h
** Description          : This module is used to configure the CAN 
** Date                 : 
** Hardware Platform    : 
******************************************************************************/
#ifndef _HAL_CAN_Config_H
#define _HAL_CAN_Config_H

/******************************************************************************
**         P R O J E C T        I N C L U D E S
******************************************************************************/
/*list all include files*/
#include "system_def.h"
#include "EAL_CAN.h"
/******************************************************************************
**      S T A R T      G L O B A L   D E F I N I T I O N 
******************************************************************************/


/*****************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

/* Std CAN driver macros *******************/
/* Convert ID to hardware format */
#define MK_STD_ID(id)                ((u16)(((id) & 0x7FFu) << 2))

/* Convert ID from hardware format to ID */
#define XT_STD_ID(id)                ((u16)(((id) >> 2) & 0x7FFu))

/*----------------------------------------------------------------------------*/
/*  [Register] CnMIDHm,CnMIDLm                                                */
/*----------------------------------------------------------------------------*/
#define   MSK_CANIDH         (0xffff0000)
#define   MSK_CANIDL         (0x0000ffff)

/*----------------------------------------------------------------------------*/
/*  CAN-ID format mode                                                        */
/*----------------------------------------------------------------------------*/
#define CANID_EXT            (0x80000000)
#define CANID_STD_MSK        (0x000007ff)
#define CANID_EXT_MSK        (0x1fffffff)
#define EXTD_ID_TYPE         (u8)0x01
#define STD_ID_TYPE          (u8)0x00

/*
 *      Conversion macro
 */
#define   SET_STD_ID(id)            (u32)((u32)id << 18)
#define   SET_EXT_ID(id)            ((u32)((u32)id) | CANID_EXT)
#define   GET_STD_ID(id)            ((((u32)id >> 18) & 0x000007ff))
#define   GET_EXT_ID(id)            ((u32)id & CANID_EXT_MSK)



/* Common defines for CAN-hardware  *****************************/
/* CanTransmit() and CanTransmitted() return values */
#define kCanTxOk             0     /* CanTransmit()   : Transmission request accepted. */
                                      /* CanTransmitted(): Transmission now completed.    */
#define kCanTxFailed         1     /* CanTransmit()   : Transmission request failed.   */
                                      /* CanTransmitted(): No transmission in progress    */
#define kCanTxInProgress     2     /* CanTransmit()   : Not allowed                    */
                                      /* CanTransmitted(): Transmission still in progress */


/******************************************************************************
**               D A T A
******************************************************************************/
/*list all global variables*/
/*
** Transmission buffer - TO BE FILLED BY APPLICATION
*/


/*########################### RECEIVE PART #################################*/


/*
** Reception buffer
*/


/* CAN driver interface  */
/* Current received message  */
extern CAN_RX_BUFF_S CanHALRxobject;


extern u8 bCANReceptionFlag;
/******************************************************************************
**                 G L O B A L   F U N C T I O N S
******************************************************************************/
/*list all global functions*/

/* To initialize the CAN Driver */
extern void CanInit( void );

/* Transmit CAN message */
extern u8 HAL_CAN_Transmit(CAN_TX_BUFF_S*,u8 slot);

/* Polling function for transmitted CAN messages */
extern u8 CanMsgTransmitted( unsigned char slot );

/* Check if new message was received */
extern void CanReceive(void);

extern void CanTxCmplt(void);

/* Function to handle CAN receive messages */
extern void CanProcessRxMsg(u8);

/*
** This function shall be called on reception of CAN message.
** This function shall put the received message in Rx Queue.
*/
extern void EAL_CAN_AddtoRxQueue(CAN_RX_BUFF_S *p_rx_buf_S);

void CanWriteMCTRL(u8 object, u16 mctrlState);
u16 CanGetMCTRL(u8 object);
void CanWriteCANId(u8 object, u32 ID);
void CanReadCANId(u8 object, u32 *ID);
void HAL_CAN_Transceiver_Init(void);

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


