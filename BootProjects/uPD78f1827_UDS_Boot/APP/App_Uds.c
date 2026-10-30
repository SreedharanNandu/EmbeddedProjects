/*App_Uds.c*/

#include "CG_macrodriver.h"
#include "system_def.h"
#include "App_Can_Tp.h"
#include <string.h>
#include "APP_Uds.h"
#include "App_Flash.h"
#include "HAL_WDT_API.h"
#include "App_Boot.h"




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

    Boot_Task();
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
        case UDS_DID_BOOTLOADER_VERSION:
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            /* msg[1], msg[2] already hold the DID */
            msg[3] = BOOTLOADER_SW_VERSION_MAJOR;
            msg[4] = BOOTLOADER_SW_VERSION_MINOR;
            msg[5] = BOOTLOADER_SW_VERSION_PATCH;
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
 
        case UDS_DID_BOOT_FINGERPRINT:
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            msg[3] = 0u;
            msg[4] = 0u;
            msg[5] = 0u;
            UDS_SendResponse(6);
            break;

        case UDS_DID_ECU_HARDWARE_NUMBER:
            msg[0] = UDS_SID_READ_DATA_BY_ID + UDS_POSITIVE_RESPONSE_OFFSET;
            msg[3] = AppFblShared.Hw_MajorPartNumber;
            msg[4] = AppFblShared.Hw_MinorPartNumber;
            msg[5] = AppFblShared.Hw_PatchPartNumber;
            UDS_SendResponse(6);
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
    u8  subFunction;
    u16 routineId;
    u32 start;
    u32 end;
    u32 addr;
    u8  cnt;
    u8  i;
    u8  alf;
    u32 imgLen;
    u16 expSum;
    u16 sum;
    u32 remaining;
    u32 n;

    if(length < 4u)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    subFunction = msg[1];
    routineId = (u16)(((u16)msg[2] << 8) | msg[3]);

    if((subFunction == 0x01u) && (routineId == 0xFF00u))  /* Erase Memory */
    {
        if(gUdsState.securityLevel != UDS_SECURITY_LEVEL_UNLOCKED)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_SECURITY_ACCESS_DENIED);
            return;
        }

        if(gUdsState.currentSession != UDS_SESSION_PROGRAMMING)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_CONDITIONS_NOT_CORRECT);
            return;
        }

        /* 31 01 FF00                           : erase whole application area                              
           31 01 FF00 <alf:1> <addr:4> <size:4> : erase that range (block aligned)                   
		 */
        if(length == 4u)
        {
            start = APP_START_ADDR;
            end   = CONFIG_START_ADDR;
        }
        else if(length == 13u)
        {
            alf = msg[4];
	        if(alf == 0x44u)
            {
  	           start = 0UL;
               end   = 0UL;
               for(i = 0u; i < 4u; i++)
               {
                  start = (start << 8) | msg[5u + i];
                  end   = (end   << 8) | msg[9u + i];
               }
               end += start;
            }	       
            else
            {
               UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
               return;
            }
        }
        else
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
            return;
        }

        if((start < APP_START_ADDR) ||
           (end <= start) || 
           (end > CONFIG_START_ADDR) ||
           ((start & (FLASH_BLOCK_SIZE - 1UL)) != 0UL) || ((end & (FLASH_BLOCK_SIZE - 1UL)) != 0UL))
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
            return;
        }

        gUdsState.eraseDone = 0u;
        gUdsState.transferInProgress = 0u;

        /* Revoke the application first,  programs the presence mask (no erase). From here on the 
           application can never look valid, whatever happens during the erase (power loss).    
           If the revoke fails, nothing is erased.                                              
		 */
        UDS_SendResponsePending(UDS_SID_ROUTINE_CONTROL);
        if(App_Invalidate() != 0u)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
            return;
        }

        #if 1
        /* Erase takes longer than P2Server: 7F 31 78 now, then every 32 blocks (P2* = 5 s)  */
        UDS_SendResponsePending(UDS_SID_ROUTINE_CONTROL);
        cnt = 0u;
        for(addr = start; addr < end; addr += FLASH_BLOCK_SIZE)
        {
            if(Flash_EraseBlock(addr) != 0u)
            {
                UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
                return;
            }
            WDT_Restart();
            cnt++;
            if(cnt >= 32u)
            {
                cnt = 0u;
                UDS_SendResponsePending(UDS_SID_ROUTINE_CONTROL);
            }
        }
        #else
        UDS_SendResponsePending(UDS_SID_ROUTINE_CONTROL);
        if (Flash_EraseRange(start, end) != 0u)
        {
           UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL,UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
           return;
        }    
        #endif
        gUdsState.eraseStart = start;
        gUdsState.eraseEnd   = end;
        gUdsState.eraseDone  = 1u;

        msg[0] = UDS_SID_ROUTINE_CONTROL + UDS_POSITIVE_RESPONSE_OFFSET;
        msg[1] = subFunction;
        msg[2] = (u8)(routineId >> 8);
        msg[3] = (u8)(routineId & 0xFFu);
        msg[4] = 0x00;  /* Routine status */
        UDS_SendResponse(5);
    }
    else if((subFunction == 0x01u) && (routineId == 0xFF01u))  /* Check memory (16-bit sum) + validate */
    {
        /* 31 01 FF01 <length:4> <sum:2>                                                         
           sum = two's complement of the sum of all 16-bit little-endian words of             
           [APP_START_ADDR, APP_START_ADDR + length).    
           length must be a multiple of 4, the tester fills gaps and padding with 0xFF.       
           Needs a whole-application erase (31 01 FF00) first.                                
           Response: 71 01 FF01 <status> <sum calculated by the boot:2>                         
           status 00 = sum matches, application marked valid (presence pattern written)       
                  01 = sum mismatch, application stays invalid                                
		 */
        if(gUdsState.securityLevel != UDS_SECURITY_LEVEL_UNLOCKED)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_SECURITY_ACCESS_DENIED);
            return;
        }

        if(gUdsState.currentSession != UDS_SESSION_PROGRAMMING)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_CONDITIONS_NOT_CORRECT);
            return;
        }

        if(length != 10u)
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
            return;
        }

        if((gUdsState.eraseDone == 0u) || (gUdsState.transferInProgress != 0u))
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_REQUEST_SEQUENCE_ERROR);
            return;
        }

        /* The presence words can only be written once per erase, so the erase must have covered */
        /* the whole application area including the last block.                                  */
        if((gUdsState.eraseStart != APP_START_ADDR) || (gUdsState.eraseEnd != CONFIG_START_ADDR))
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_CONDITIONS_NOT_CORRECT);
            return;
        }

        imgLen = ((u32)msg[4] << 24) | ((u32)msg[5] << 16) | ((u32)msg[6] << 8) | (u32)msg[7];
        expSum = (u16)(((u16)msg[8] << 8) | (u16)msg[9]);

        if((imgLen == 0UL) || (imgLen > APP_MAX_IMAGE_SIZE) || ((imgLen & 3UL) != 0UL))
        {
            UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
            return;
        }

        /* Read the flash back and add up the 16-bit words. 0x78 before and between the chunks.  */
        UDS_SendResponsePending(UDS_SID_ROUTINE_CONTROL);
        sum       = 0u;
        addr      = APP_START_ADDR;
        remaining = imgLen;
        while(remaining != 0UL)
        {
            n = (remaining > 16384UL) ? 16384UL : remaining;
            sum        = Boot_Sum16Update(sum, addr, n);
            addr      += n;
            remaining -= n;
            WDT_Restart();
            if(remaining != 0UL)
            {
                UDS_SendResponsePending(UDS_SID_ROUTINE_CONTROL);
            }
        }
        sum = Boot_Sum16Final(sum);

        if(sum == expSum)
        {
            if(App_Validate(imgLen, sum) != 0u)
            {
                UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
                return;
            }
            gUdsState.eraseDone = 0u;      /* a further download needs a new erase (= revoke) */
            msg[4] = 0x00;
        }
        else
        {
            msg[4] = 0x01;
        }

        msg[0] = UDS_SID_ROUTINE_CONTROL + UDS_POSITIVE_RESPONSE_OFFSET;
        msg[1] = subFunction;
        msg[2] = (u8)(routineId >> 8);
        msg[3] = (u8)(routineId & 0xFFu);
        msg[5] = (u8)(sum >> 8);
        msg[6] = (u8)(sum);
        UDS_SendResponse(7);
    }
    else
    {
        UDS_SendNegativeResponse(msg, UDS_SID_ROUTINE_CONTROL, UDS_NRC_REQUEST_OUT_OF_RANGE);
    }
}

/*==============================================================================================*/
/* Function: UDS_HandleRequestDownload (0x34)                                                   */
/*==============================================================================================*/
static void UDS_HandleRequestDownload(u8 *msg, u16 length)
{
    u8 addressLength;
    u8 sizeLength;
    u32 address = 0;
    u32 size = 0;
    u8 i;

    if(length < 3)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    /* Check security and session */
    if(gUdsState.securityLevel != UDS_SECURITY_LEVEL_UNLOCKED)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_SECURITY_ACCESS_DENIED);
        return;
    }

    if(gUdsState.currentSession != UDS_SESSION_PROGRAMMING)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_CONDITIONS_NOT_CORRECT);
        return;
    }

    /* Flash must have been erased (31 01 FF00) before a download is accepted */
    if(gUdsState.eraseDone == 0u)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }

    /* msg[1] = dataFormatIdentifier (not used) */
    addressLength = (u8)((msg[2] >> 4) & 0x0F);
    sizeLength = (u8)(msg[2] & 0x0F);

    if((addressLength == 0u) || (addressLength > 4u) || (sizeLength == 0u) || (sizeLength > 4u))
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_REQUEST_OUT_OF_RANGE);
        return;
    }

    if(length != (u16)(3u + addressLength + sizeLength))
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    /* Extract address */
    for(i = 0; i < addressLength; i++)
    {
        address = (address << 8) | msg[3 + i];
    }

    /* Extract size */
    for(i = 0; i < sizeLength; i++)
    {
        size = (size << 8) | msg[3 + addressLength + i];
    }

    /* Validate address range: whole image must lie inside the application area 
       Note that the last 16 bytes of the area (APP_TRAILER_ADDR..) belong to the presence pattern.       
	 */
    if((address < gUdsState.eraseStart) || (address >= gUdsState.eraseEnd) ||
       (address >= APP_IMAGE_END_ADDR) ||
       (size == 0UL) || (size > (gUdsState.eraseEnd - address)) ||
       (size > (APP_IMAGE_END_ADDR - address)) ||
       ((address & 3UL) != 0UL))
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_DOWNLOAD, UDS_NRC_REQUEST_OUT_OF_RANGE);
        return;
    }

    /* Store download parameters */
    gUdsState.downloadAddress = address;
    gUdsState.downloadSize = size;
    gUdsState.bytesReceived = 0;
    gUdsState.blockSequenceCounter = 1;
    gUdsState.transferInProgress = 1;

    /* Send positive response: lengthFormatIdentifier 0x20 = 2 byte maxNumberOfBlockLength      */
    msg[0] = UDS_SID_REQUEST_DOWNLOAD + UDS_POSITIVE_RESPONSE_OFFSET;
    msg[1] = 0x20;
    msg[2] = (u8)(gUdsState.blockSize >> 8);
    msg[3] = (u8)(gUdsState.blockSize & 0xFF);
    UDS_SendResponse(4);
}

/*==============================================================================================*/
/* Function: UDS_HandleTransferData (0x36)                                                      */
/* The payload is already in RAM at &msg[2] (no second buffer, no copy).                        */
/*==============================================================================================*/
static void UDS_HandleTransferData(u8 *msg, u16 length)
{
    u8  blockSequenceCounter;
    u16 dataLength;
    u16 paddedLength;

    if(length < 2)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    if(!gUdsState.transferInProgress)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }

    blockSequenceCounter = msg[1];

    if(blockSequenceCounter != gUdsState.blockSequenceCounter)
    {
        /* Tester repeats the previous block (our response was lost): acknowledge, no re-write */
        if((gUdsState.bytesReceived != 0UL) &&
           (blockSequenceCounter == (u8)(gUdsState.blockSequenceCounter - 1u)))
        {
            msg[0] = UDS_SID_TRANSFER_DATA + UDS_POSITIVE_RESPONSE_OFFSET;
            UDS_SendResponse(2);
            return;
        }
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_WRONG_BLOCK_SEQUENCE_COUNTER);
        return;
    }

    dataLength = (u16)(length - 2);

    if(dataLength == 0u)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    if(((u32)gUdsState.bytesReceived + dataLength) > gUdsState.downloadSize)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_REQUEST_OUT_OF_RANGE);
        return;
    }

    /* Only the last block may be shorter than a multiple of 4, else the next address is misaligned */
    if((((u32)gUdsState.bytesReceived + dataLength) != gUdsState.downloadSize) && ((dataLength & 3u) != 0u))
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_INCORRECT_MESSAGE_LENGTH);
        return;
    }

    /* Pad to flash word boundary IN PLACE (CANTP_BUF_PAD bytes of slack exist behind msg) */
    paddedLength = dataLength;
    while((paddedLength & 3u) != 0u)
    {
        msg[2u + paddedLength] = 0xFFu;
        paddedLength++;
    }

    /*
    Remark The FSL_Write function writes data in word units (1 word = 4 bytes, maximum 64 words (256 bytes)).
    */
    #if 0
    if(Flash_WriteWords(gUdsState.downloadAddress + gUdsState.bytesReceived,
                        &msg[2], (u8)(paddedLength >> 2)) != 0u)
    #else
    if(Flash_WriteWords(gUdsState.downloadAddress + gUdsState.bytesReceived,
                        &msg[2], (paddedLength >> 2)) != 0u)
    #endif
    {
        UDS_SendNegativeResponse(msg, UDS_SID_TRANSFER_DATA, UDS_NRC_GENERAL_PROGRAMMING_FAILURE);
        return;
    }

    gUdsState.bytesReceived += dataLength;
    gUdsState.blockSequenceCounter++;           /* u8: wraps 0xFF -> 0x00 as required */

    msg[0] = UDS_SID_TRANSFER_DATA + UDS_POSITIVE_RESPONSE_OFFSET;
    msg[1] = blockSequenceCounter;
    UDS_SendResponse(2);
}

/*==============================================================================================*/
/* Function: UDS_HandleRequestTransferExit (0x37)                                               */
/*==============================================================================================*/
static void UDS_HandleRequestTransferExit(u8 *msg, u16 length)
{
    (void)length;

    if(!gUdsState.transferInProgress)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_TRANSFER_EXIT, UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }

    if(gUdsState.bytesReceived != gUdsState.downloadSize)
    {
        UDS_SendNegativeResponse(msg, UDS_SID_REQUEST_TRANSFER_EXIT, UDS_NRC_REQUEST_SEQUENCE_ERROR);
        return;
    }

    gUdsState.transferInProgress = 0;
    gUdsState.isProgrammingComplete = 1;

    /* Send positive response */
    msg[0] = UDS_SID_REQUEST_TRANSFER_EXIT + UDS_POSITIVE_RESPONSE_OFFSET;
    UDS_SendResponse(1);
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

