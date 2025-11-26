/*==============================================================================================*/
/* Project      = UDS Bootloader for uPD78F1827                                                 */
/* Module       = uds_service.c                                                                 */
/* Description  = UDS (ISO 14229) Service Implementation                                        */
/*==============================================================================================*/

#include "system_def.h"
#include "App_Can_Tp.h"
#include <string.h>
#include "APP_Uds.h"

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
#define UDS_DID_BOOTLOADER_VERSION              0xF180
#define UDS_DID_APPLICATION_VERSION             0xF181
#define UDS_DID_ECU_SERIAL_NUMBER               0xF18C
#define UDS_DID_VIN                             0xF190
#define UDS_DID_PROGRAMMING_DATE                0xF199

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
} UDS_State_t;

static UDS_State_t gUdsState;

/*----------------------------------------------------------------------------------------------*/
/* UDS Response Buffer                                                                          */
/*----------------------------------------------------------------------------------------------*/
static u8 gUdsResponseBuffer[8];
static u16 gUdsResponseLength;


/*----------------------------------------------------------------------------------------------*/
/* Function Prototypes                                                                          */
/*----------------------------------------------------------------------------------------------*/
static void UDS_ProcessRequest(const u8 *pData, u16 length);
static void UDS_SendResponse(const u8 *pData, u16 length);
static void UDS_SendNegativeResponse(u8 sid, u8 nrc);

/* Service Handlers */
static void UDS_HandleDiagnosticSessionControl(const u8 *pData, u16 length);
static void UDS_HandleEcuReset(const u8 *pData, u16 length);
static void UDS_HandleSecurityAccess(const u8 *pData, u16 length);
static void UDS_HandleTesterPresent(const u8 *pData, u16 length);
static void UDS_HandleReadDataById(const u8 *pData, u16 length);
static void UDS_HandleWriteDataById(const u8 *pData, u16 length);
static void UDS_HandleRoutineControl(const u8 *pData, u16 length);
static void UDS_HandleRequestDownload(const u8 *pData, u16 length);
static void UDS_HandleTransferData(const u8 *pData, u16 length);
static void UDS_HandleRequestTransferExit(const u8 *pData, u16 length);

/*==============================================================================================*/
/* Function: UDS_Init                                                                           */
/*==============================================================================================*/
void UDS_Init(void)
{


    memset(&gUdsState, 0, sizeof(gUdsState));
    gUdsState.currentSession = UDS_SESSION_DEFAULT;
    gUdsState.securityLevel = UDS_SECURITY_LEVEL_LOCKED;
    gUdsState.blockSize = 256u;  /* Maximum block size for transfer */


}

/*==============================================================================================*/
/* Function: UDS_MainFunction                                                                   */
/*==============================================================================================*/
void UDS_MainFunction(void)
{
    
    /* Check for received UDS request */
    if(CANTP_ReceiveData() == CANTP_OK)
    {
        /* Process UDS request */
        UDS_ProcessRequest(gCanTpCtx.rxBuffer,gCanTpCtx.rxLength);
    }
}

/*==============================================================================================*/
/* Function: UDS_ProcessRequest                                                                 */
/*==============================================================================================*/
static void UDS_ProcessRequest(const u8 *pData, u16 length)
{
    u8 sid;
    
    if(length < 1)
    {
        return;  /* Invalid request */
    }
    
    sid = pData[0];
    
    /* Route to appropriate service handler */
    switch(sid)
    {
        case UDS_SID_DIAGNOSTIC_SESSION_CONTROL:
            UDS_HandleDiagnosticSessionControl(pData, length);
            break;
            
        case UDS_SID_ECU_RESET:
            UDS_HandleEcuReset(pData, length);
            break;
            
        case UDS_SID_SECURITY_ACCESS:
            UDS_HandleSecurityAccess(pData, length);
            break;
            
        case UDS_SID_TESTER_PRESENT:
            UDS_HandleTesterPresent(pData, length);
            break;
            
        case UDS_SID_READ_DATA_BY_ID:
            UDS_HandleReadDataById(pData, length);
            break;
            
        case UDS_SID_WRITE_DATA_BY_ID:
            UDS_HandleWriteDataById(pData, length);
            break;
            
        case UDS_SID_ROUTINE_CONTROL:
            UDS_HandleRoutineControl(pData, length);
            break;
            
        case UDS_SID_REQUEST_DOWNLOAD:
            UDS_HandleRequestDownload(pData, length);
            break;
            
        case UDS_SID_TRANSFER_DATA:
            UDS_HandleTransferData(pData, length);
            break;
            
        case UDS_SID_REQUEST_TRANSFER_EXIT:
            UDS_HandleRequestTransferExit(pData, length);
            break;
            
        default:
            UDS_SendNegativeResponse(sid, UDS_NRC_SERVICE_NOT_SUPPORTED);
            break;
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleDiagnosticSessionControl (0x10)                                          */
/*==============================================================================================*/
static void UDS_HandleDiagnosticSessionControl(const u8 *pData, u16 length)
{
    u8 sessionType;
    
    if(length != 2)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    sessionType = pData[1];
    
    /* Validate session type */
    if(sessionType == UDS_SESSION_DEFAULT || 
       sessionType == UDS_SESSION_PROGRAMMING ||
       sessionType == UDS_SESSION_EXTENDED)
    {
        gUdsState.currentSession = sessionType;
        gUdsState.isSessionActive = 1;
        
        /* Send positive response */
        gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
        gUdsResponseBuffer[1] = sessionType;
        gUdsResponseBuffer[2] = 0x00;  /* P2Server_max high byte */
        gUdsResponseBuffer[3] = 0x32;  /* P2Server_max low byte (50ms) */
        gUdsResponseBuffer[4] = 0x01;  /* P2*Server_max high byte */
        gUdsResponseBuffer[5] = 0xF4;  /* P2*Server_max low byte (500ms) */
        UDS_SendResponse(gUdsResponseBuffer, 6);
    }
    else
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleEcuReset (0x11)                                                          */
/*==============================================================================================*/
static void UDS_HandleEcuReset(const u8 *pData, u16 length)
{
    u8 resetType;
    
    if(length != 2)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    resetType = pData[1];
    
    /* Send positive response first */
    gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
    gUdsResponseBuffer[1] = resetType;
    UDS_SendResponse(gUdsResponseBuffer, 2);
    
    /* Perform reset after small delay */
    /* In real implementation, you would trigger a watchdog reset or software reset */

}

/*==============================================================================================*/
/* Function: UDS_HandleSecurityAccess (0x27)                                                    */
/*==============================================================================================*/
static void UDS_HandleSecurityAccess(const u8 *pData, u16 length)
{
    u8 subFunction;
    
    if(length < 2)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    subFunction = pData[1];
    
    if(subFunction == 0x01)  /* Request Seed */
    {
        /* Send seed (simplified - in production use proper security) */
        gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
        gUdsResponseBuffer[1] = subFunction;
        gUdsResponseBuffer[2] = 0x12;  /* Seed byte 0 */
        gUdsResponseBuffer[3] = 0x34;  /* Seed byte 1 */
        gUdsResponseBuffer[4] = 0x56;  /* Seed byte 2 */
        gUdsResponseBuffer[5] = 0x78;  /* Seed byte 3 */
        UDS_SendResponse(gUdsResponseBuffer, 6);
    }
    else if(subFunction == 0x02)  /* Send Key */
    {
        if(length != 6)
        {
            UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
            return;
        }
        else
        {
           /* Verify key (simplified - in production use proper algorithm) */
           /* Expected key = seed XOR 0xFFFFFFFF */
           #if defined(NO_SECURITY)
           if(pData[2] == 0xED && pData[3] == 0xCB && 
              pData[4] == 0xA9 && pData[5] == 0x87)
           #endif
           {
               gUdsState.securityLevel = UDS_SECURITY_LEVEL_UNLOCKED;
               gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
               gUdsResponseBuffer[1] = subFunction;
               UDS_SendResponse(gUdsResponseBuffer, 2);
           }
           #if defined(NO_SECURITY)
           else
           {
               UDS_SendNegativeResponse(pData[0], UDS_NRC_INVALID_KEY);
           }
           #endif
        }
    }
    else
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_SUB_FUNCTION_NOT_SUPPORTED);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleTesterPresent (0x3E)                                                     */
/*==============================================================================================*/
static void UDS_HandleTesterPresent(const u8 *pData, u16 length)
{
    if(length != 2)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    /* Send positive response */
    gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
    gUdsResponseBuffer[1] = pData[1];
    UDS_SendResponse(gUdsResponseBuffer, 2);
}

/*==============================================================================================*/
/* Function: UDS_HandleReadDataById (0x22)                                                      */
/*==============================================================================================*/
static void UDS_HandleReadDataById(const u8 *pData, u16 length)
{
    u16 did;
    
    if(length != 3)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    did = (u16)(((u16)pData[1] << 8) | pData[2]);
    
    gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
    gUdsResponseBuffer[1] = pData[1];
    gUdsResponseBuffer[2] = pData[2];
    
    switch(did)
    {
        case UDS_DID_BOOTLOADER_VERSION:
            gUdsResponseBuffer[3] = 0x01;  /* Major */
            gUdsResponseBuffer[4] = 0x00;  /* Minor */
            gUdsResponseBuffer[5] = 0x00;  /* Patch */
            UDS_SendResponse(gUdsResponseBuffer, 6);
            break;
            
        case UDS_DID_ECU_SERIAL_NUMBER:
            memcpy(&gUdsResponseBuffer[3], "BL78F1827001", 12);
            UDS_SendResponse(gUdsResponseBuffer, 15);
            break;
            
        default:
            UDS_SendNegativeResponse(pData[0], UDS_NRC_REQUEST_OUT_OF_RANGE);
            break;
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleWriteDataById (0x2E)                                                     */
/*==============================================================================================*/
static void UDS_HandleWriteDataById(const u8 *pData, u16 length)
{
    /* Not implemented in basic bootloader */
    UDS_SendNegativeResponse(pData[0], UDS_NRC_SERVICE_NOT_SUPPORTED);
}

/*==============================================================================================*/
/* Function: UDS_HandleRoutineControl (0x31)                                                    */
/*==============================================================================================*/
static void UDS_HandleRoutineControl(const u8 *pData, u16 length)
{
    u8 subFunction;
    u16 routineId;
    u8 result;
    u16 block;


    
    if(length < 4u)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    subFunction = pData[1];
    routineId = (u16)(((u16)pData[2] << 8) | pData[3]);
    
    if((subFunction == 0x01u) && (routineId == 0xFF00u))  /* Erase Memory */
    {
        /* Check security access */
        if(gUdsState.securityLevel != UDS_SECURITY_LEVEL_UNLOCKED)
        {
            UDS_SendNegativeResponse(pData[0], UDS_NRC_SECURITY_ACCESS_DENIED);
            return;
        }

        #if 0
        
        /* flash self-programming */
        fslRetCode = FslBlockErase( &typFslData );
        
        /* Erase application area (blocks) */
        if(fslRetCode == FSL_OK)
        {
            gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
            gUdsResponseBuffer[1] = subFunction;
            gUdsResponseBuffer[2] = pData[2];
            gUdsResponseBuffer[3] = pData[3];
            gUdsResponseBuffer[4] = 0x00;  /* Routine status */
            UDS_SendResponse(gUdsResponseBuffer, 5);
        }
        else
	#endif
	
        {
            UDS_SendNegativeResponse(pData[0], UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
        }
    }
    else
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_REQUEST_OUT_OF_RANGE);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleRequestDownload (0x34)                                                   */
/*==============================================================================================*/
static void UDS_HandleRequestDownload(const u8 *pData, u16 length)
{
    u8 dataFormatId;
    u8 addressAndLengthFormatId;
    u8 addressLength;
    u8 sizeLength;
    u32 address = 0;
    u32 size = 0;
    u8 i;
    
    if(length < 3)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    /* Check security and session */
    if(gUdsState.securityLevel != UDS_SECURITY_LEVEL_UNLOCKED)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_SECURITY_ACCESS_DENIED);
        return;
    }
    
    if(gUdsState.currentSession != UDS_SESSION_PROGRAMMING)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_CONDITIONS_NOT_CORRECT);
        return;
    }
    
    dataFormatId = pData[1];
    addressAndLengthFormatId = pData[2];
    addressLength = (addressAndLengthFormatId >> 4) & 0x0F;
    sizeLength = addressAndLengthFormatId & 0x0F;
    
    /* Extract address */
    for(i = 0; i < addressLength; i++)
    {
        address = (address << 8) | pData[3 + i];
    }
    
    /* Extract size */
    for(i = 0; i < sizeLength; i++)
    {
        size = (size << 8) | pData[3 + addressLength + i];
    }
    
    /* Validate address range */
    if(address < 0x04000UL || address >= 0x3F000UL)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_REQUEST_OUT_OF_RANGE);
        return;
    }
    
    /* Store download parameters */
    gUdsState.downloadAddress = address;
    gUdsState.downloadSize = size;
    gUdsState.bytesReceived = 0;
    gUdsState.blockSequenceCounter = 1;
    gUdsState.transferInProgress = 1;
    
    /* Send positive response */
    gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
    gUdsResponseBuffer[1] = 0x20;  /* maxNumberOfBlockLength format */
    gUdsResponseBuffer[2] = (u8)(gUdsState.blockSize >> 8);
    gUdsResponseBuffer[3] = (u8)(gUdsState.blockSize & 0xFF);
    UDS_SendResponse(gUdsResponseBuffer, 4);
}

/*==============================================================================================*/
/* Function: UDS_HandleTransferData (0x36)                                                      */
/*==============================================================================================*/
static void UDS_HandleTransferData(const u8 *pData, u16 length)
{
    u8 blockSequenceCounter;
    u16 dataLength;
    u32 writeAddress;
    u8 wordCount;
    u8 result;

    
    if(length < 2)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }
    
    if(!gUdsState.transferInProgress)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }
    
    blockSequenceCounter = pData[1];
    
    /* Verify block sequence counter */
    if(blockSequenceCounter != gUdsState.blockSequenceCounter)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER);
        return;
    }
    
    dataLength = length - 2;
    #if 0
    /* Copy data to FSL buffer */
    memcpy(fsl_data_buffer, &pData[2], dataLength);
    
    /* Pad to word boundary if needed */
    while(dataLength % 4 != 0)
    {
        fsl_data_buffer[dataLength++] = 0xFF;
    }
    
    wordCount = (u8)(dataLength / 4);
    writeAddress = gUdsState.downloadAddress + gUdsState.bytesReceived;
    
    /* Sets parameter */
    typFslData.pDataBuffer   = fsl_data_buffer;     /* Data buffer pointer  */
    typFslData.startBlock    = SAM_START_BLOCK;     /* Write block          */
    typFslData.pWriteData    = samWriteData;        /* Write data pointer   */
    typFslData.writeDataSize = SAM_DATA_SIZE;       /* Write data size      */
    typFslData.endOption     = SAM_SET_OPTION;      /* Option code          */

    /* Write data to flash */
    result = FslBlockWrite(&typFslData);

    if(result == FSL_OK)
    {
        gUdsState.bytesReceived += (length - 2);
        gUdsState.blockSequenceCounter++;
        if(gUdsState.blockSequenceCounter > 0xFF)
        {
            gUdsState.blockSequenceCounter = 0;
        }
        
        /* Send positive response */
        gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
        gUdsResponseBuffer[1] = blockSequenceCounter;
        UDS_SendResponse(gUdsResponseBuffer, 2);
    }
    else
    #endif
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleRequestTransferExit (0x37)                                               */
/*==============================================================================================*/
static void UDS_HandleRequestTransferExit(const u8 *pData, u16 length)
{
    if(!gUdsState.transferInProgress)
    {
        UDS_SendNegativeResponse(pData[0], UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }
    
    gUdsState.transferInProgress = 0;
    gUdsState.isProgrammingComplete = 1;
    
    /* Send positive response */
    gUdsResponseBuffer[0] = pData[0] + UDS_POSITIVE_RESPONSE_OFFSET;
    UDS_SendResponse(gUdsResponseBuffer, 1);
}

/*==============================================================================================*/
/* Function: UDS_SendResponse                                                                   */
/*==============================================================================================*/
static void UDS_SendResponse(const u8 *pData, u16 length)
{
    CANTP_SendData(pData, length);
}

/*==============================================================================================*/
/* Function: UDS_SendNegativeResponse                                                           */
/*==============================================================================================*/
static void UDS_SendNegativeResponse(u8 sid, u8 nrc)
{
    gUdsResponseBuffer[0] = 0x7F;
    gUdsResponseBuffer[1] = sid;
    gUdsResponseBuffer[2] = nrc;
    UDS_SendResponse(gUdsResponseBuffer, 3);
}

/*==============================================================================================*/
/* Function: UDS_IsSessionActive                                                                */
/*==============================================================================================*/
u8 UDS_IsSessionActive(void)
{
    return gUdsState.isSessionActive;
}

/*==============================================================================================*/
/* Function: UDS_IsProgrammingComplete                                                          */
/*==============================================================================================*/
u8 UDS_IsProgrammingComplete(void)
{
    return gUdsState.isProgrammingComplete;
}