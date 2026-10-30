#ifndef APP_CAN_TP_H
#define APP_CAN_TP_H

#include "system_def.h"

/*==============================================================================================*/
/* CAN-TP (ISO 15765-2) - minimal-RAM version                                                   */
/*                                                                                              */
/* RAM concept                                                                                  */
/*   * ONE static message buffer (gBuf) shared by request and response. No malloc/calloc.      */
/*   * RX is fully handled in the CAN RX ISR (copy only, no TX from ISR).                       */
/*   * TX (flow control, SF/FF/CF) is handled only in CANTP_MainFunction() / UDS context.        */
/*   * Buffer ownership is passed on by the state variable:                                     */
/*                                                                                              */
/*        IDLE -> RX_WAIT_CF -> RX_READY -> RX_PROC -> TX_xxx -> IDLE        (ISR / UDS / TP)   */
/*                                                                                              */
/*     While the buffer is owned by UDS (RX_PROC) or by the TX engine, new SF/FF frames are     */
/*     dropped. A tester must wait for the response anyway (ISO 14229 request/response).        */
/*   * The largest message the ECU accepts is CANTP_BUF_SIZE. It is reported to the tester in   */
/*     the 0x34 response (maxNumberOfBlockLength) and enforced via FC.OVFLW on a First Frame.   */
/*==============================================================================================*/

/*----------------------------------------------------------------------------------------------*/
/* Configuration                                                                                */
/*----------------------------------------------------------------------------------------------*/
/* Payload bytes per 0x36 TransferData request. Multiple of 4 (flash word).                     */
/* 32 / 64 / 128 / 256 are sensible values. RAM cost is 1:1 with this number.                   */
#ifndef CANTP_XFER_DATA_SIZE
#define CANTP_XFER_DATA_SIZE    256u
#endif

/* Largest TP message = SID + blockSequenceCounter + payload (= maxNumberOfBlockLength)         */
#define CANTP_BUF_SIZE          (CANTP_XFER_DATA_SIZE + 2u)

/* Slack behind the message so the payload can be padded to a flash word in place               */
#define CANTP_BUF_PAD           3u

/* Flow control values that WE send to the tester when receiving a multi-frame message          */
#ifndef CANTP_RX_BS
#define CANTP_RX_BS             0u      /* 0 = whole message in one block (buffer holds it all) */
#endif
#ifndef CANTP_RX_STMIN_MS
#define CANTP_RX_STMIN_MS       1u      /* min. CF spacing in ms (0 only after ISR load is checked) */
#endif

#define CANTP_TIMEOUT_MS        1000u   /* N_Bs / N_Cr / N_As                                    */
#define CANTP_WFT_MAX           10u     /* max. consecutive FC.WAIT accepted while transmitting  */
#define CANTP_PAD_BYTE          0xCCu

/* Compile-time check: buffer must hold the biggest non-transfer response (e.g. RDBI 20 bytes) */
/* and the FF length field is only 12 bit.                                                      */
typedef char CANTP_CfgCheck_t[((CANTP_BUF_SIZE >= 24u) && (CANTP_BUF_SIZE <= 4095u)) ? 1 : -1];

/*----------------------------------------------------------------------------------------------*/
/* API                                                                                          */
/*----------------------------------------------------------------------------------------------*/
typedef enum {
    CANTP_OK = 0,
    CANTP_ERROR,
    CANTP_BUSY,
    CANTP_NO_DATA
} CANTP_Result_t;

/* Init: call once at cold start (after EAL_CAN_Init) */
void CANTP_Init(void);

/* Cyclic 1 ms task, main context: TX engine, flow control, timeouts */
void CANTP_MainFunction(void);

/* Called from the CAN RX ISR for every received frame of the physical request ID */
void CANTP_ProcessRxFrame(const u8 *pData, u8 length);

/* UDS side (main context only) ------------------------------------------------------------- */

/* Returns CANTP_OK and the buffer if a complete request is waiting. The buffer now belongs to  */
/* the caller until CANTP_Transmit() or CANTP_ReleaseRequest() is called.                       */
/* The response may be built IN PLACE in the same buffer.                                       */
CANTP_Result_t CANTP_GetRequest(u8 **ppMsg, u16 *pLength);

/* Send the first 'length' bytes of the buffer as response (SF or multi-frame). Zero copy.      */
/* Ownership of the buffer passes to the TP until the last frame is queued.                     */
CANTP_Result_t CANTP_Transmit(u16 length);

/* Finish a request without sending a response (e.g. suppressPosRspMsgIndicationBit).           */
/* Harmless if CANTP_Transmit() was already called.                                             */
void CANTP_ReleaseRequest(void);

/* Send a single frame (1..7 bytes) right now WITHOUT touching the buffer, e.g. 7F xx 78        */
/* (response pending) before a long flash operation. Only valid while a request is owned.       */
CANTP_Result_t CANTP_SendImmediate(const u8 *pData, u8 length);

/* TRUE when nothing is received/transmitted (e.g. before jumping to the application / reset)   */
u8 CANTP_IsIdle(void);

/* Number of frames discarded because the buffer was busy (debug aid, wraps at 255)             */
u8 CANTP_GetDropCount(void);

#endif /* APP_CAN_TP_H */
