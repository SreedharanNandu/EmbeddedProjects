/*==============================================================================================*/
/* Project      = UDS Bootloader for uPD78F1827                                                 */
/* Module       = can_tp.c                                                                      */
/* Description  = CAN Transport Protocol (ISO 15765-2) Implementation                           */
/*==============================================================================================*/

#include "App_Can_Tp.h"
#include "App_Can.h"
#include <string.h>

/*----------------------------------------------------------------------------------------------*/
/* CAN-TP Frame Types                                                                           */
/*----------------------------------------------------------------------------------------------*/
#define CANTP_FRAME_TYPE_SF     0x00u  /* Single Frame */
#define CANTP_FRAME_TYPE_FF     0x10u  /* First Frame */
#define CANTP_FRAME_TYPE_CF     0x20u  /* Consecutive Frame */
#define CANTP_FRAME_TYPE_FC     0x30u  /* Flow Control */

/*----------------------------------------------------------------------------------------------*/
/* Flow Control Status                                                                          */
/*----------------------------------------------------------------------------------------------*/
#define CANTP_FC_CTS            0x00u  /* Continue To Send */
#define CANTP_FC_WAIT           0x01u  /* Wait */
#define CANTP_FC_OVFLW          0x02u  /* Overflow */

/*----------------------------------------------------------------------------------------------*/
/* CAN-TP Timing Parameters (in milliseconds)                                                   */
/*----------------------------------------------------------------------------------------------*/
#define CANTP_TIMEOUT_BS        1000u  /* Time between consecutive frames */
#define CANTP_TIMEOUT_CR        1000u  /* Timeout for receiving CF after FC */
#define CANTP_TIMEOUT_AS        1000u  /* Timeout for transmit confirmation */
#define CANTP_STmin_DEFAULT     10u    /* Minimum separation time */


CAN_Context_t gCanCtx;
CANTP_Context_t gCanTpCtx;

/*----------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                          */
/*----------------------------------------------------------------------------------------------*/
static void CANTP_SendFlowControl(u8 flowStatus);
static void CANTP_SendSingleFrame(const u8 *pData, u16 length);
static void CANTP_SendFirstFrame(const u8 *pData, u16 length);
static void CANTP_SendConsecutiveFrame(void);
static void CANTP_ProcessFlowControl(const u8 *pData);
static void CANTP_Driver_Transmit(const u8 *pData);

/*==============================================================================================*/
/* Function: CANTP_Init                                                                         */
/*==============================================================================================*/
void CANTP_Init(void)
{
    memset(&gCanTpCtx, 0, sizeof(gCanTpCtx));
    gCanTpCtx.state = CANTP_STATE_IDLE;

    memset(&gCanCtx, 0, sizeof(gCanCtx));
    gCanCtx.initialized = 1;
}

/*==============================================================================================*/
/* Function: CANTP_MainFunction                                                                 */
/* Description: Cyclic function to be called every 1ms                                          */
/*==============================================================================================*/
void CANTP_MainFunction(void)
{
    /* Handle transmit state machine */
    switch(gCanTpCtx.state)
    {
        case CANTP_STATE_TX_WAIT_FC:
            if(gCanTpCtx.txTimeout > 0)
            {
                gCanTpCtx.txTimeout--;
                if(gCanTpCtx.txTimeout == 0)
                {
                    /* Timeout - abort transmission */
                    gCanTpCtx.state = CANTP_STATE_IDLE;
                }
            }
            break;
            
        case CANTP_STATE_TX_CF:
            /* Send consecutive frames */
            if(gCanTpCtx.txBytesSent < gCanTpCtx.txLength)
            {
                CANTP_SendConsecutiveFrame();
            }
            else
            {
                gCanTpCtx.state = CANTP_STATE_TX_COMPLETE;
            }
            break;
            
        case CANTP_STATE_RX_WAIT_CF:
            if(gCanTpCtx.rxTimeout > 0)
            {
                gCanTpCtx.rxTimeout--;
                if(gCanTpCtx.rxTimeout == 0)
                {
                    /* Timeout - abort reception */
                    gCanTpCtx.state = CANTP_STATE_IDLE;
                }
            }
            break;
            
        default:
            break;
    }
}

/*==============================================================================================*/
/* Function: CANTP_ProcessRxFrame                                                               */
/*==============================================================================================*/
void CANTP_ProcessRxFrame(const u8 *pData, u8 length)
{
    u8 frameType;
    u16 dataLength;
    u8 i;
    
    if(length < 1)
    {
        return;
    }
    
    frameType = pData[0] & 0xF0u;
    
    switch(frameType)
    {
        case CANTP_FRAME_TYPE_SF:  /* Single Frame */
            dataLength = pData[0] & 0x0F;
            if(dataLength > 0 && dataLength <= 7 && length >= (dataLength + 1))
            {
                memcpy(gCanTpCtx.rxBuffer, &pData[1], dataLength);
                gCanTpCtx.rxLength = dataLength;
                gCanTpCtx.state = CANTP_STATE_RX_COMPLETE;
            }
            break;
            
        case CANTP_FRAME_TYPE_FF:  /* First Frame */
            dataLength = (u16)(((pData[0] & 0x0F) << 8) | pData[1]);
            if(dataLength <= CANTP_MAX_DATA_LENGTH)
            {
                gCanTpCtx.rxLength = dataLength;
                gCanTpCtx.rxBytesReceived = 0;
                
                /* Copy first 6 bytes of data */
                for(i = 0; i < 6 && i < dataLength; i++)
                {
                    gCanTpCtx.rxBuffer[gCanTpCtx.rxBytesReceived++] = pData[2 + i];
                }
                
                gCanTpCtx.rxSequenceNumber = 1;
                gCanTpCtx.state = CANTP_STATE_RX_WAIT_CF;
                gCanTpCtx.rxTimeout = CANTP_TIMEOUT_BS;
                
                /* Send Flow Control - Continue To Send */
                CANTP_SendFlowControl(CANTP_FC_CTS);
            }
            else
            {
                /* Buffer overflow */
                CANTP_SendFlowControl(CANTP_FC_OVFLW);
            }
            break;
            
        case CANTP_FRAME_TYPE_CF:  /* Consecutive Frame */
            if(gCanTpCtx.state == CANTP_STATE_RX_WAIT_CF)
            {
                u8 sequenceNumber = pData[0] & 0x0F;
                
                if(sequenceNumber == gCanTpCtx.rxSequenceNumber)
                {
                    /* Copy data */
                    for(i = 1; i < length && gCanTpCtx.rxBytesReceived < gCanTpCtx.rxLength; i++)
                    {
                        gCanTpCtx.rxBuffer[gCanTpCtx.rxBytesReceived++] = pData[i];
                    }
                    
                    /* Increment sequence number */
                    gCanTpCtx.rxSequenceNumber++;
                    if(gCanTpCtx.rxSequenceNumber > 0x0F)
                    {
                        gCanTpCtx.rxSequenceNumber = 0;
                    }
                    
                    /* Reset timeout */
                    gCanTpCtx.rxTimeout = CANTP_TIMEOUT_BS;
                    
                    /* Check if reception complete */
                    if(gCanTpCtx.rxBytesReceived >= gCanTpCtx.rxLength)
                    {
                        gCanTpCtx.state = CANTP_STATE_RX_COMPLETE;
                    }
                }
                /* Else: Wrong sequence number - ignore frame */
            }
            break;
            
        case CANTP_FRAME_TYPE_FC:  /* Flow Control */
            if(gCanTpCtx.state == CANTP_STATE_TX_WAIT_FC)
            {
                CANTP_ProcessFlowControl(pData);
            }
            break;
            
        default:
            break;
    }
}

/*==============================================================================================*/
/* Function: CANTP_SendFlowControl                                                              */
/*==============================================================================================*/
static void CANTP_SendFlowControl(u8 flowStatus)
{
    u8 fcFrame[8];
    
    fcFrame[0] = CANTP_FRAME_TYPE_FC | (flowStatus & 0x0F);
    fcFrame[1] = 0x00;  /* Block Size (0 = unlimited) */
    fcFrame[2] = CANTP_STmin_DEFAULT;  /* STmin */
    
    /* Fill remaining bytes with padding */
    memset(&fcFrame[3], 0xCC, 5);
    
    CANTP_Driver_Transmit(fcFrame);
}

/*==============================================================================================*/
/* Function: CANTP_ProcessFlowControl                                                           */
/*==============================================================================================*/
static void CANTP_ProcessFlowControl(const u8 *pData)
{
    u8 flowStatus = pData[0] & 0x0F;
    
    switch(flowStatus)
    {
        case CANTP_FC_CTS:
            gCanTpCtx.txBlockSize = pData[1];
            gCanTpCtx.txSTmin = pData[2];
            gCanTpCtx.txFrameCounter = 0;
            gCanTpCtx.state = CANTP_STATE_TX_CF;
            break;
            
        case CANTP_FC_WAIT:
            /* Wait - reset timeout */
            gCanTpCtx.txTimeout = CANTP_TIMEOUT_AS;
            break;
            
        case CANTP_FC_OVFLW:
            /* Overflow - abort transmission */
            gCanTpCtx.state = CANTP_STATE_IDLE;
            break;
            
        default:
            break;
    }
}

/*==============================================================================================*/
/* Function: CANTP_SendSingleFrame                                                              */
/*==============================================================================================*/
static void CANTP_SendSingleFrame(const u8 *pData, u16 length)
{
    u8 sfFrame[8];
    u8 i;
    
    sfFrame[0] = (u8)(length & 0x0Fu);
    
    for(i = 0; i < length && i < 7; i++)
    {
        sfFrame[1 + i] = pData[i];
    }
    
    /* Fill remaining bytes with padding */
    for(; i < 7; i++)
    {
        sfFrame[1 + i] = 0xCC;
    }
    
    CANTP_Driver_Transmit(sfFrame);
    gCanTpCtx.state = CANTP_STATE_TX_COMPLETE;
}

/*==============================================================================================*/
/* Function: CANTP_SendFirstFrame                                                               */
/*==============================================================================================*/
static void CANTP_SendFirstFrame(const u8 *pData, u16 length)
{
    u8 ffFrame[8];
    u8 i;
    
    ffFrame[0] = (u8)(CANTP_FRAME_TYPE_FF | ((length >> 8) & 0x0Fu));
    ffFrame[1] = (u8)(length & 0xFF);
    
    /* Copy first 6 bytes */
    for(i = 0; i < 6 && i < length; i++)
    {
        ffFrame[2 + i] = pData[i];
    }
    
    gCanTpCtx.txBytesSent = 6;
    gCanTpCtx.txSequenceNumber = 1;
    
    CANTP_Driver_Transmit(ffFrame);
    
    gCanTpCtx.state = CANTP_STATE_TX_WAIT_FC;
    gCanTpCtx.txTimeout = CANTP_TIMEOUT_BS;
}

/*==============================================================================================*/
/* Function: CANTP_SendConsecutiveFrame                                                         */
/*==============================================================================================*/
static void CANTP_SendConsecutiveFrame(void)
{
    u8 cfFrame[8];
    u8 i;
    u8 bytesToSend;
    
    cfFrame[0] = CANTP_FRAME_TYPE_CF | (gCanTpCtx.txSequenceNumber & 0x0F);
    
    /* Calculate bytes to send in this frame */
    bytesToSend = (u8)((gCanTpCtx.txLength - gCanTpCtx.txBytesSent) > 7 ? 
                   7 : (gCanTpCtx.txLength - gCanTpCtx.txBytesSent));
    
    for(i = 0; i < bytesToSend; i++)
    {
        cfFrame[1 + i] = gCanTpCtx.txBuffer[gCanTpCtx.txBytesSent++];
    }
    
    /* Fill remaining bytes with padding */
    for(; i < 7; i++)
    {
        cfFrame[1 + i] = 0xCC;
    }
    
    CANTP_Driver_Transmit(cfFrame);
    
    /* Increment sequence number */
    gCanTpCtx.txSequenceNumber++;
    if(gCanTpCtx.txSequenceNumber > 0x0F)
    {
        gCanTpCtx.txSequenceNumber = 0;
    }
    
    /* Check block size */
    gCanTpCtx.txFrameCounter++;
    if(gCanTpCtx.txBlockSize > 0 && gCanTpCtx.txFrameCounter >= gCanTpCtx.txBlockSize)
    {
        /* Block complete - wait for next Flow Control */
        gCanTpCtx.state = CANTP_STATE_TX_WAIT_FC;
        gCanTpCtx.txTimeout = CANTP_TIMEOUT_BS;
    }
}

/*==============================================================================================*/
/* Function: CANTP_SendData                                                                     */
/*==============================================================================================*/
CANTP_Result_t CANTP_SendData(const u8 *pData, u16 length)
{
    if(pData == NULL || length == 0)
    {
        return CANTP_ERROR;
    }
    
    if(gCanTpCtx.state != CANTP_STATE_IDLE && gCanTpCtx.state != CANTP_STATE_TX_COMPLETE)
    {
        return CANTP_BUSY;
    }
    
    /* Copy data to transmit buffer */
    memcpy(gCanTpCtx.txBuffer, pData, length);
    gCanTpCtx.txLength = length;
    gCanTpCtx.txBytesSent = 0;
    
    if(length <= 7)
    {
        /* Single Frame */
        CANTP_SendSingleFrame(pData, length);
    }
    else
    {
        /* Multi-frame transmission - start with First Frame */
        CANTP_SendFirstFrame(pData, length);
    }
    
    return CANTP_OK;
}

/*==============================================================================================*/
/* Function: CANTP_ReceiveData                                                                  */
/*==============================================================================================*/
CANTP_Result_t CANTP_ReceiveData(void)
{
    
    if(gCanTpCtx.state == CANTP_STATE_RX_COMPLETE)
    {
        /* Reset state */
        gCanTpCtx.state = CANTP_STATE_IDLE;
        return CANTP_OK;
    }
    
    return CANTP_NO_DATA;
}

/*==============================================================================================*/
/* Function: CANTP_Driver_Transmit                                                                */
/* Description: Transmit CAN frame                                                              */
/*              USER TO IMPLEMENT: Send data via CAN controller                                 */
/*==============================================================================================*/
static void CANTP_Driver_Transmit(const u8 *pData)
{
    u8 test_msg_number;
    
    if( (gCanCtx.initialized) || (pData != NULL) )
    {
        test_msg_number = App_Get_Tx_Msg_No(CAN_TX_ID);
        EAL_CAN_Tx_Msg(test_msg_number, pData);
    }	
}
