/* App_Boot.h : 
 *   - application presence pattern + mask (present / revoked / invalid)
 *   - 16-bit word sum over the flash 
 *   - reprogramming request flag (RAM, survives a reset)   
 *   - start-up decision, optional stay-in-boot window, jump to the application
 */
#ifndef APP_BOOT_H
#define APP_BOOT_H

#include "system_def.h"
#include "App_Uds.h"      /* APP_START_ADDR, CONFIG_START_ADDR, APP_VALID_MAGIC, BOOT_REQUEST_MAGIC */


/*----------------------------------------------------------------------------------------------*
 * Layout of the last 16 bytes of the application area (end of the last 1 KB erase block)       *
 *                                                                                              *
 *   0x17BF0  sum       16-bit sum in the low half, upper half stays FFFF   \ written first,    *
 *   0x17BF4  length    image length in bytes, little endian                 / one FSL write    *
 *   0x17BF8  PATTERN   APP_VALID_MAGIC                                       written LAST      *
 *   0x17BFC  MASK      erased (FFFFFFFF) while valid or blank; programmed with the inverse     *
 *                      of the magic to REVOKE the application (no erase needed)                *
 *                                                                                              *
 *   state PRESENT : pattern == magic and mask erased and length plausible                      *
 *   state REVOKED : mask == inverse of magic                                                   *
 *   state INVALID : anything else (blank, torn write after power loss, ...)                    *
 *                                                                                              *
 * Every change of state only clears bits (1 -> 0), so a torn write can only move the state     *
 * towards INVALID, never towards PRESENT. The whole area is cleared by the next erase of the   *
 * last block, which is part of every whole-application erase.                                  *
 *----------------------------------------------------------------------------------------------*/
#define APP_TRAILER_ADDR      (CONFIG_START_ADDR - 16UL)   /* sum + length        */
#define APP_PATTERN_ADDR      (CONFIG_START_ADDR - 8UL)    /* presence pattern    */
#define APP_MASK_ADDR         (CONFIG_START_ADDR - 4UL)    /* presence mask       */
#define APP_IMAGE_END_ADDR    APP_TRAILER_ADDR             /* image must end below it */
#define APP_MAX_IMAGE_SIZE    (APP_IMAGE_END_ADDR - APP_START_ADDR)

#define APP_MASK_ERASED       0xFFFFFFFFUL
#define APP_MASK_REVOKED      ((u32)(~APP_VALID_MAGIC))

#define APP_STATE_INVALID     0u
#define APP_STATE_PRESENT     1u
#define APP_STATE_REVOKED     2u

#ifndef FLASH_READ8
#define FLASH_READ8(a)   (*(const __far u8 *)(u32)(a))
#endif
#ifndef FLASH_READ16
#define FLASH_READ16(a)  (*(const __far u16 *)(u32)(a))     /* a must be even */
#endif

#define APPLICATION_SW_VERSION_MAJOR    1
#define APPLICATION_SW_VERSION_MINOR    0
#define APPLICATION_SW_VERSION_PATCH    0


typedef struct
{
   u8 Hw_MajorPartNumber;
   u8 Hw_MinorPartNumber;
   u8 Hw_PatchPartNumber;
   u8 SerialNumber[20];
   u8 Reserved;

}AppFblShared_T;


u32 Flash_ReadU32LE(u32 addr);

/*----------------------------------------------------------------------------------------------*/
/* Reprogramming request (RAM, survives a reset). Set by the application, consumed by the boot.  */
/*----------------------------------------------------------------------------------------------*/
void BootReq_Set(void);
u8   BootReq_IsSet(void);
void BootReq_Clear(void);
void Init_Boot_App(void);

/*----------------------------------------------------------------------------------------------*/
/* Application presence                                                                         */
/*----------------------------------------------------------------------------------------------*/
u8   App_GetState(void);                       /* APP_STATE_xxx                                  */
u8   App_IsValid(void);                        /* 1 = PRESENT                                    */
u8   App_Invalidate(void);                     /* 0 = OK. Programs the mask, no erase            */
u8   App_Validate(u32 length, u16 sum);        /* 0 = OK. Needs an erased trailer block          */
void App_GetTrailer(u32 *length, u16 *sum);    /* raw trailer values (0xFFFF / 0xFFFFFFFF if blank) */


#endif
