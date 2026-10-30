/*App_Uds.h*/

#ifndef APP_UDS_H
#define APP_UDS_H


#include <stdio.h>
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>
#include "system_def.h"

#define NO_SECURITY 1


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
#define UDS_DID_BOOTLOADER_VERSION              0xF180
#define UDS_DID_BOOT_FINGERPRINT                0xF183
#define UDS_DID_ECU_SERIAL_NUMBER               0xF18C
#define UDS_DID_ECU_HARDWARE_NUMBER             0xF191
#define UDS_DID_APPLICATION_STATUS              0xF1A0   


/* Memory Map Configuration */
#define BOOT_START_ADDR         0x00000UL
#define BOOT_END_ADDR           0x03FFFUL

#define APP_START_ADDR          0x04000UL
#define APP_END_ADDR            0x17BFFUL

#define CONFIG_START_ADDR       0x17C00UL   /* top 1 KB: reserved (link option -go..17C00h,1024) */
#define CONFIG_END_ADDR         0x17FFFUL   /* end of 96 KB flash */

/* Bootloader Configuration */
#define BOOT_TIMEOUT_MS         5000
#define BOOT_REQUEST_MAGIC      0xBAADBEEFUL
#define APP_VALID_MAGIC         0xAA55AA55UL



void UDS_Init(void);
void UDS_MainFunction(void);
u8 UDS_IsSessionActive(void);
u8 UDS_WasRequestReceived(void);   /* 1 once any UDS request has arrived since UDS_Init() */
u8 UDS_IsProgrammingComplete(void);



#endif 