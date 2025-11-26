#ifndef APP_CAN_TP_H
#define APP_CAN_TP_H

#include "system_def.h"


#define CANTP_MAX_DATA_LENGTH   4096u

typedef enum {
    CANTP_OK = 0,
    CANTP_ERROR,
    CANTP_BUSY,
    CANTP_NO_DATA,
    CANTP_TIMEOUT
} CANTP_Result_t;

/*----------------------------------------------------------------------------------------------*/
/* CAN-TP State Machine                                                                         */
/*----------------------------------------------------------------------------------------------*/
typedef enum {
    CANTP_STATE_IDLE,
    CANTP_STATE_RX_WAIT_CF,
    CANTP_STATE_RX_COMPLETE,
    CANTP_STATE_TX_SF,
    CANTP_STATE_TX_FF,
    CANTP_STATE_TX_WAIT_FC,
    CANTP_STATE_TX_CF,
    CANTP_STATE_TX_COMPLETE
} CANTP_State_t;

/*----------------------------------------------------------------------------------------------*/
/* CAN-TP Context Structure                                                                     */
/*----------------------------------------------------------------------------------------------*/
typedef struct {
    CANTP_State_t state;
    u8  rxBuffer[CANTP_MAX_DATA_LENGTH];
    u16 rxLength;
    u16 rxBytesReceived;
    u8  rxSequenceNumber;
    u16 rxTimeout;
    
    u8  txBuffer[8];
    u16 txLength;
    u16 txBytesSent;
    u8  txSequenceNumber;
    u16 txTimeout;
    u8  txBlockSize;
    u8  txSTmin;
    u8  txFrameCounter;
    
    u8  canFrame[8];
    u8  canFrameLength;
} CANTP_Context_t;

/*----------------------------------------------------------------------------------------------*/
/* CAN Driver State                                                                             */
/*----------------------------------------------------------------------------------------------*/
typedef struct {
    u8  rxBuffer[8];
    u8  rxLength;
    u8  rxAvailable;
    u8  initialized;
} CAN_Context_t;

extern CANTP_Context_t gCanTpCtx;
extern CAN_Context_t gCanCtx;


void CANTP_Init(void);
void CANTP_MainFunction(void);
CANTP_Result_t CANTP_SendData(const u8 *pData, u16 length);
CANTP_Result_t CANTP_ReceiveData(void);
void CANTP_ProcessRxFrame(const u8 *pData, u8 length);

#endif /* APP_CAN_TP_H */