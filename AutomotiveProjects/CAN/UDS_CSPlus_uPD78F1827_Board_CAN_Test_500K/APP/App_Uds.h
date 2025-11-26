#ifndef APP_UDS_H
#define APP_UDS_H


#include <stdio.h>
#include <ctype.h> 
#include <stdlib.h>
#include <string.h>
#include "system_def.h"

#define NO_SECURITY 1


#define BOOTLOADER_VERSION_MAJOR    1
#define BOOTLOADER_VERSION_MINOR    0
#define BOOTLOADER_VERSION_PATCH    0



/* Memory Map Configuration */
#define BOOT_START_ADDR         0x00000UL
#define BOOT_END_ADDR           0x03FFFUL
#define APP_START_ADDR          0x04000UL
#define APP_END_ADDR            0x3EFFFUL
#define CONFIG_START_ADDR       0x3F000UL
#define CONFIG_END_ADDR         0x3FFFFUL

/* Bootloader Configuration */
#define BOOT_TIMEOUT_MS         5000
#define BOOT_REQUEST_MAGIC      0xBAADBEEFUL
#define APP_VALID_MAGIC         0xAA55AA55UL



void UDS_Init(void);
void UDS_MainFunction(void);
u8 UDS_IsSessionActive(void);
u8 UDS_IsProgrammingComplete(void);



#endif 