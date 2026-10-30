/*App_Uds.c*/

#include "CG_macrodriver.h"
#include "system_def.h"
#include "App_Can_Tp.h"
#include <string.h>
#include "APP_Uds.h"
#include "HAL_WDT_API.h"
#include "App_Boot.h"
#include "MOS.h"


/*----------------------------------------------------------------------------------------------*/
/* UDS Service IDs (ISO 14229-1)                                                                */
/*----------------------------------------------------------------------------------------------*/
#define UDS_SID_DIAGNOSTIC_SESSION_CONTROL      0x10
#define UDS_SID_ECU_RESET                       0x11
#define UDS_SID_SECURITY_ACCESS                 0x27
#define UDS_SID_COMMUNICATION_CONTROL           0x28
#define UDS_SID_TESTER_PRESENT                  0x3E
#define UDS_SID_READ_DATA_BY_ID                 0x22
#define UDS_SID_WRITE_DATA_BY_ID                0x2E
#define UDS_SID_ROUTINE_CONTROL                 0x31
#define UDS_SID_CONTROL_DTC                     0x85
#define UDS_SID_REQUEST_DOWNLOAD                0x34
#define UDS_SID_REQUEST_UPLOAD                  0x35
#define UDS_SID_TRANSFER_DATA                   0x36
#define UDS_SID_REQUEST_TRANSFER_EXIT           0x37

#define UDS_POSITIVE_RESPONSE_OFFSET            0x40

/*----------------------------------------------------------------------------------------------*/
/* UDS Negative Response Codes                                                                  */
/*----------------------------------------------------------------------------------------------*/
#define UDS_NRC_POSITIVE_RESPONSE               0x00
#define UDS_NRC_GENERAL_REJECT                  0x10
#define UDS_NRC_SERVICE_NOT_SUPPORTED           0x11
#define UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED      0x12
#define UDS_NRC_INCORRECT_MESSAGE_LENGTH        0x13
#define UDS_NRC_CONDITIONS_NOT_CORRECT          0x22
#define UDS_NRC_REQUEST_SEQUENCE_ERROR          0x24
#define UDS_NRC_REQUEST_OUT_OF_RANGE            0x31
#define UDS_NRC_SECURITY_ACCESS_DENIED          0x33
#define UDS_NRC_INVALID_KEY                     0x35
#define UDS_NRC_EXCEEDED_NUMBER_OF_ATTEMPTS     0x36
#define UDS_NRC_REQUIRED_TIME_DELAY_NOT_EXPIRED 0x37
#define UDS_NRC_UPLOAD_DOWNLOAD_NOT_ACCEPTED    0x70
#define UDS_NRC_TRANSFER_DATA_SUSPENDED         0x71
#define UDS_NRC_GENERAL_PROGRAMMING_FAILURE     0x72
#define UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER    0x73
#define UDS_NRC_RESPONSE_PENDING                0x78

/*----------------------------------------------------------------------------------------------*/
/* UDS Session Types                                                                            */
/*----------------------------------------------------------------------------------------------*/
#define UDS_SESSION_DEFAULT                     0x01
#define UDS_SESSION_PROGRAMMING                 0x02
#define UDS_SESSION_EXTENDED                    0x03

/*----------------------------------------------------------------------------------------------*/
/* UDS Security Access                                                                          */
/*----------------------------------------------------------------------------------------------*/
#define UDS_SECURITY_LEVEL_LOCKED               0x00
#define UDS_SECURITY_LEVEL_UNLOCKED             0x01

/*----------------------------------------------------------------------------------------------*/
/* Data Identifiers (DIDs)                                                                      */
/*----------------------------------------------------------------------------------------------*/
#define UDS_DID_APPLICATION_VERSION             0xF181
#define UDS_DID_ECU_SERIAL_NUMBER               0xF18C
#define UDS_DID_ECU_HARDWARE_NUMBER             0xF191
#define UDS_DID_APPLICATION_STATUS              0xF1A0   


/*----------------------------------------------------------------------------------------------*/
/* UDS State Variables                                                                          */
/*----------------------------------------------------------------------------------------------*/
typedef struct {
    u8  currentSession;
    u8  securityLevel;
    u8  isSessionActive;
    u8  isProgrammingComplete;
    u32 downloadAddress;
    u32 downloadSize;
    u32 bytesReceived;
    u8  blockSequenceCounter;
    u8  transferInProgress;
    u16 blockSize;
    u8  eraseDone;
    u32 eraseStart;
    u32 eraseEnd;
    u8  requestSeen;          
} UDS_State_t;

volatile UDS_State_t gUdsState;

extern const __far AppFblShared_T AppFblShared;

/* No separate response buffer any more: the response is built in the TP's shared buffer.       */

/*----------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                          */
/*----------------------------------------------------------------------------------------------*/
static void UDS_ProcessRequest(u8 *msg, u16 length);
static void UDS_SendResponse(u16 length);
static void UDS_SendNegativeResponse(u8 *msg, u8 sid, u8 nrc);
void        UDS_SendResponsePending(u8 sid);

/* Service Handlers */
static void UDS_HandleDiagnosticSessionControl(u8 *msg, u16 length);
static void UDS_HandleEcuReset(u8 *msg, u16 length);
static void UDS_HandleSecurityAccess(u8 *msg, u16 length);
static void UDS_HandleTesterPresent(u8 *msg, u16 length);
static void UDS_HandleReadDataById(u8 *msg, u16 length);
static void UDS_HandleWriteDataById(u8 *msg, u16 length);
static void UDS_HandleRoutineControl(u8 *msg, u16 length);
static void UDS_HandleRequestDownload(u8 *msg, u16 length);
static void UDS_HandleTransferData(u8 *msg, u16 length);
static void UDS_HandleRequestTransferExit(u8 *msg, u16 length);
static void UDS_HandleControlDTC(u8 *msg, u16 length);


extern void UDS_ForceReset(void);      /* App_Reset.asm */
void UDS_PerformReset(void);

/*==============================================================================================*/
/* Function: UDS_Init                                                                           */
/*==============================================================================================*/
void UDS_Init(void)
{
    memset(&gUdsState, 0, sizeof(gUdsState));
    gUdsState.currentSession = UDS_SESSION_DEFAULT;
    #if defined(NO_SECURITY)
    gUdsState.securityLevel = UDS_SECURITY_LEVEL_UNLOCKED;
    #else
    gUdsState.securityLevel = UDS_SECURITY_LEVEL_LOCKED;
    #endif

    /* maxNumberOfBlockLength reported in the 0x34 response: SID + blockSequenceCounter + data. */
    /* This is exactly the size of the TP buffer, no more RAM is needed for a bigger block.     */
    gUdsState.blockSize = (u16)CANTP_BUF_SIZE;
}

/*==============================================================================================*/
/* Function: UDS_MainFunction                                                                   */
/* Call as often as possible (every 1 ms recommended, see notes on the scheduler)               */
/*==============================================================================================*/
void UDS_MainFunction(void)
{
    u8  *msg;
    u16  length;

    /* Check for received UDS request */
    if(CANTP_GetRequest(&msg, &length) == CANTP_OK)
    {
        gUdsState.requestSeen = 1u;          
        UDS_ProcessRequest(msg, length);

        /* Every request ends here: if no response was handed to the TP (e.g. suppressed        */
        /* positive response) the buffer is freed now. No-op after CANTP_Transmit().            */
        CANTP_ReleaseRequest();
    }
}

/*==============================================================================================*/
/* Function: UDS_ProcessRequest                                                                 */
/*==============================================================================================*/
static void UDS_ProcessRequest(u8 *msg, u16 length)
{
    if(length < 1)
    {
        return;  /* Invalid request */
    }

    /* Route to appropriate service handler */
    switch(msg[0])
    {
        case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
            UDS_HandleDiagnosticSessionControl(msg, length);
            break;

        case UDS_SID_ECU_RESET:
            UDS_HandleEcuReset(msg, length);
            break;

        case UDS_SID_SECURITY_ACCESS:
            UDS_HandleSecurityAccess(msg, length);
            break;

        case UDS_SID_TESTER_PRESENT:
            UDS_HandleTesterPresent(msg, length);
            break;

        case UDS_SID_READ_DATA_BY_ID:
            UDS_HandleReadDataById(msg, length);
            break;

        case UDS_SID_WRITE_DATA_BY_ID:
            UDS_HandleWriteDataById(msg, length);
            break;

        case UDS_SID_ROUTINE_CONTROL:
            UDS_HandleRoutineControl(msg, length);
            break;

        case UDS_SID_REQUEST_DOWNLOAD:
            UDS_HandleRequestDownload(msg, length);
            break;

        case UDS_SID_TRANSFER_DATA:
            UDS_HandleTransferData(msg, length);
            break;

        case UDS_SID_REQUEST_TRANSFER_EXIT:
            UDS_HandleRequestTransferExit(msg, length);
            break;
        case UDS_SID_CONTROL_DTC:	    
            UDS_HandleControlDTC(msg, length);
            break;

	    default:
            UDS_SendNegativeResponse(msg, msg[0], UDS_NRC_SERVICE_NOT_SUPPORTED);
            break;
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleDiagnosticSessionControl (0x10)                                          */
/*==============================================================================================*/
static void UDS_HandleDiagnosticSessionControl(u8 *msg, u16 length)
{
    u8 sessionType;

    if(length != 2)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_DIAGNOSTIC_SESSION_CONTROL, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    sessionType = msg[1];

    /* Validate session type */
    if(sessionType == UDS_SESSION_DEFAULT ||
       sessionType == UDS_SESSION_PROGRAMMING ||
       sessionType == UDS_SESSION_EXTENDED)
    {
        gUdsState.currentSession = sessionType;
        gUdsState.isSessionActive = 1;

        if(sessionType == UDS_SESSION_PROGRAMMING)
        {
           BootReq_Set();
        }
        /* Send positive response */
        msg[0] = UDS_SID_DIAGNOSTIC_SESSION_CONTROL + UDS_POSITIVE_RESPONSE_OFFSET;
        msg[1] = sessionType;
        msg[2] = 0x00;  /* P2Server_max high byte */
        msg[3] = 0x32;  /* P2Server_max low byte (50ms) */
        msg[4] = 0x01;  /* P2*Server_max high byte */
        msg[5] = 0xF4;  /* P2*Server_max low byte */
        UDS_SendResponse(6);
    }
    else
    {
        UDS_SendNegativeResponse(msg, UDS_SID_DIAGNOSTIC_SESSION_CONTROL, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
}


/*==============================================================================================*/
/* Function: UDS_HandleControlDTC (0x85)                                                        */
/*==============================================================================================*/
static void UDS_HandleControlDTC(u8 *msg, u16 length)
{
    u8 dtcType;

    if(length != 2)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_CONTROL_DTC, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    dtcType = msg[1];

    if(dtcType == 0x01)/*Enable DTC*/
    {
        /* Send positive response */
        msg[0] = UDS_SID_CONTROL_DTC + UDS_POSITIVE_RESPONSE_OFFSET;
        msg[1] = dtcType;
        UDS_SendResponse(2);
    }
    else if(dtcType == 0x02)/*Disable DTC*/
    {
        msg[0] = UDS_SID_CONTROL_DTC + UDS_POSITIVE_RESPONSE_OFFSET;
        msg[1] = dtcType;
        UDS_SendResponse(2);
    }
    else
    {
        UDS_SendNegativeResponse(msg, UDS_SID_CONTROL_DTC, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleEcuReset (0x11)                                                          */
/*==============================================================================================*/
static void UDS_HandleEcuReset(u8 *msg, u16 length)
{
    u32 txDelay = 50000u;
    u8 resetType;

    if(length != 2)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_ECU_RESET, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    resetType = msg[1];

    /* Send positive response first */
    msg[0] = UDS_SID_ECU_RESET + UDS_POSITIVE_RESPONSE_OFFSET;
    if(resetType == 0x01)/*Hard reset*/
    {
       msg[1] = resetType;
       UDS_SendResponse(2);
       while(1);/*WDT reset*/
    }
    else if(resetType == 0x02)/*Key on/off reset*/
    {
       UDS_SendNegativeResponse(msg, UDS_SID_ECU_RESET, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
    else if(resetType == 0x03)/*Soft reset*/
    {
       msg[1] = resetType;
       UDS_SendResponse(2);
       while(txDelay--);
       UDS_PerformReset();
    }
    else 
    {
       UDS_SendNegativeResponse(msg, UDS_SID_ECU_RESET, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
    /* Perform reset after small delay */
    /* In real implementation, you would trigger a watchdog reset or software reset.            */
    /* Wait until CANTP_IsIdle() is TRUE (response frame queued) before resetting.              */

}

/*==============================================================================================*/
/* Function: UDS_HandleSecurityAccess (0x27)                                                    */
/*==============================================================================================*/
static void UDS_HandleSecurityAccess(u8 *msg, u16 length)
{
    u8 subFunction;

    if(length < 2)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_SECURITY_ACCESS, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    subFunction = msg[1];

    if(subFunction == 0x01)  /* Request Seed */
    {
        /* Send seed (simplified - in production use proper security) */
        msg[0] = UDS_SID_SECURITY_ACCESS + UDS_POSITIVE_RESPONSE_OFFSET;
        msg[1] = subFunction;
        msg[2] = 0x12;  /* Seed byte 0 */
        msg[3] = 0x34;  /* Seed byte 1 */
        msg[4] = 0x56;  /* Seed byte 2 */
        msg[5] = 0x78;  /* Seed byte 3 */
        UDS_SendResponse(6);
    }
    else if(subFunction == 0x02)  /* Send Key */
    {
        if(length != 6)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_SECURITY_ACCESS, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
            return;
        }
        else
        {
           /* Verify key (simplified - in production use proper algorithm) */
           /* Expected key = seed XOR 0xFFFFFFFF */
           #if defined(NO_SECURITY)
           if(msg[2] == 0xED && msg[3] == 0xCB &&
              msg[4] == 0xA9 && msg[5] == 0x87)
           #endif
           {
               gUdsState.securityLevel = UDS_SECURITY_LEVEL_UNLOCKED;
               msg[0] = UDS_SID_SECURITY_ACCESS + UDS_POSITIVE_RESPONSE_OFFSET;
               msg[1] = subFunction;
               UDS_SendResponse(2);
           }
           #if defined(NO_SECURITY)
           else
           {
               UDS_SendNegativeResponse(msg, UDS_SID_SECURITY_ACCESS, UDS_NRC_INVALID_KEY);
           }
           #endif
        }
    }
    else
    {
        UDS_SendNegativeResponse(msg, UDS_SID_SECURITY_ACCESS, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleTesterPresent (0x3E)                                                     */
/*==============================================================================================*/
static void UDS_HandleTesterPresent(u8 *msg, u16 length)
{
    u8 subFunction;

    if(length != 2)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TESTER_PRESENT, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    subFunction = msg[1];

    if((subFunction & 0x7Fu) != 0x00u)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TESTER_PRESENT, UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
        return;
    }

    if((subFunction & 0x80u) != 0u)
    {
        return;  /* suppressPosRspMsgIndicationBit: no response, buffer is released by caller */
    }

    /* Send positive response */
    msg[0] = UDS_SID_TESTER_PRESENT + UDS_POSITIVE_RESPONSE_OFFSET;
    msg[1] = 0x00;
    UDS_SendResponse(2);
}

/*==============================================================================================*/
/* Function: UDS_HandleReadDataById (0x22)                                                      */
/*==============================================================================================*/
static void UDS_HandleReadDataById(u8 *msg, u16 length)
{
    u16 did;
    u32 appLen;
    u16 appSum;
    u8 i;

    if(length != 3)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_READ_DATA_BY_ID, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    did = (u16)(((u16)msg[1] << 8) | msg[2]);

    switch(did)
    {
        case UDS_DID_APPLICATION_VERSION:
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            /* msg[1], msg[2] already hold the DID */
            msg[3] = APPLICATION_SW_VERSION_MAJOR;
            msg[4] = APPLICATION_SW_VERSION_MINOR;
            msg[5] = APPLICATION_SW_VERSION_PATCH;
            UDS_SendResponse(6);
            break;

        case UDS_DID_ECU_SERIAL_NUMBER:
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            /*special note: - here the CC780 compiler translate to far access but using memcpy the function do not use far
              so data doesnt get copied or you could use a far pointer and point to address accordingly */
            for (i = 0u; i < 20u; i++)
            {
               msg[3u + i] = AppFblShared.SerialNumber[i];
            }
            UDS_SendResponse(23);
            break;

        case UDS_DID_APPLICATION_STATUS:
            /* state (0 invalid, 1 present, 2 revoked), length:4, 16-bit sum:2 (big endian)     */
            App_GetTrailer(&appLen, &appSum);
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            msg[3] = App_GetState();
            msg[4] = (u8)(appLen >> 24);
            msg[5] = (u8)(appLen >> 16);
            msg[6] = (u8)(appLen >> 8);
            msg[7] = (u8)(appLen);
            msg[8] = (u8)(appSum >> 8);
            msg[9] = (u8)(appSum);
            UDS_SendResponse(10);
            break;

        case UDS_DID_ECU_HARDWARE_NUMBER:
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            msg[3] = AppFblShared.Hw_MajorPartNumber;
            msg[4] = AppFblShared.Hw_MinorPartNumber;
            msg[5] = AppFblShared.Hw_PatchPartNumber;
            UDS_SendResponse(6);
            break;

        default:
            UDS_SendNegativeResponse(msg, UDS_SID_READ_DATA_BY_ID, UDS_NRC_REQUEST_OUT_OF_RANGE);
            break;
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleWriteDataById (0x2E)                                                     */
/*==============================================================================================*/
static void UDS_HandleWriteDataById(u8 *msg, u16 length)
{
    /* Not implemented in basic bootloader */
    (void)length;
    UDS_SendNegativeResponse(msg, UDS_SID_WRITE_DATA_BY_ID, UDS_NRC_SERVICE_NOT_SUPPORTED);
}

/*==============================================================================================*/
/* Function: UDS_HandleRoutineControl (0x31)                                                    */
/*==============================================================================================*/
static void UDS_HandleRoutineControl(u8 *msg, u16 length)
{
   UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_SERVICE_NOT_SUPPORTED);
}

/*==============================================================================================*/
/* Function: UDS_HandleRequestDownload (0x34)                                                   */
/*==============================================================================================*/
static void UDS_HandleRequestDownload(u8 *msg, u16 length)
{
   UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_SERVICE_NOT_SUPPORTED);
}

/*==============================================================================================*/
/* Function: UDS_HandleTransferData (0x36)                                                      */
/* The payload is already in RAM at &msg[2] (no second buffer, no copy).                        */
/*==============================================================================================*/
static void UDS_HandleTransferData(u8 *msg, u16 length)
{
   UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_SERVICE_NOT_SUPPORTED);
}

/*==============================================================================================*/
/* Function: UDS_HandleRequestTransferExit (0x37)                                               */
/*==============================================================================================*/
static void UDS_HandleRequestTransferExit(u8 *msg, u16 length)
{
   UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_TRANSFER_EXIT, UDS_NRC_SERVICE_NOT_SUPPORTED);
}

/*==============================================================================================*/
/* Function: UDS_SendResponse - response (length bytes) is already in the TP buffer             */
/*==============================================================================================*/
static void UDS_SendResponse(u16 length)
{
    (void)CANTP_Transmit(length);
}

/*==============================================================================================*/
/* Function: UDS_SendNegativeResponse                                                           */
/*==============================================================================================*/
static void UDS_SendNegativeResponse(u8 *msg, u8 sid, u8 nrc)
{
    msg[0] = 0x7F;
    msg[1] = sid;
    msg[2] = nrc;
    UDS_SendResponse(3);
}

/*==============================================================================================*/
/* Function: UDS_SendResponsePending - 7F <sid> 78, does not use the shared buffer              */
/* Use before blocking flash operations that may take longer than P2Server (50 ms).             */
/*==============================================================================================*/
void UDS_SendResponsePending(u8 sid)
{
    u8 nr[3];

    nr[0] = 0x7F;
    nr[1] = sid;
    nr[2] = UDS_NRC_RESPONSE_PENDING;
    (void)CANTP_SendImmediate(nr, 3);
}

/*==============================================================================================*/
/* Function: UDS_IsSessionActive                                                                */
/*==============================================================================================*/
u8 UDS_IsSessionActive(void)
{
    return gUdsState.isSessionActive;
}

u8 UDS_WasRequestReceived(void)
{
    return gUdsState.requestSeen;
}

/*==============================================================================================*/
/* Function: UDS_IsProgrammingComplete                                                          */
/*==============================================================================================*/
u8 UDS_IsProgrammingComplete(void)
{
    return gUdsState.isProgrammingComplete;
}

void UDS_PerformReset(void)
{
   u16 bootStartAddress = 0u;

   bootStartAddress = (Flash_ReadU32LE(0u) & 0xFFFFu);
   DI();
   ((void (*)(void))bootStartAddress)();
   for(;;) { }
}


