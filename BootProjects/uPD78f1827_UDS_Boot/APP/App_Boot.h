
/* App_Boot.h : 
 *   - application presence pattern + mask (present / revoked / invalid)
 *   - 16-bit word sum over the flash 
 *   - reprogramming request flag (RAM, survives a reset) "
 *   - start-up decision, optional stay-in-boot window, jump to the application
 */
#ifndef APP_BOOT_H
#define APP_BOOT_H

#include "system_def.h"
#include "App_Uds.h"      /* APP_START_ADDR, CONFIG_START_ADDR, APP_VALID_MAGIC, BOOT_REQUEST_MAGIC */
#include "App_Flash.h"    /* FLASH_BLOCK_SIZE, FLASH_ERR_PARAM */

/*----------------------------------------------------------------------------------------------*
 * Layout of the last 16 bytes of the application area (end of the last 1 KB erase block)       *
 *                                                                                              *
 *   0xXXX0  sum       16-bit sum in the low half, upper half stays FFFF   \ written first,    *
 *   0xXXX4  length    image length in bytes, little endian                 / one FSL write    *
 *   0xXXX8  PATTERN   APP_VALID_MAGIC                                       written LAST      *
 *   0xXXXC  MASK      erased (FFFFFFFF) while valid or blank; programmed with the inverse     *
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

#define APP_ERR_STATE         0xFEu     /* flash write returned OK but the state is not as expected */

/*----------------------------------------------------------------------------------------------*/
/* Start-up behaviour                        */
/*   0 : a valid application is started immediately    */
/*   1 : a valid application is started after BOOT_START_DELAY_MS, unless a tester sends any    */
/*       UDS request in that window.  Every ms of it delays    */
/*       the start of the application after each reset: keep it as short as the tester allows. */
/*----------------------------------------------------------------------------------------------*/
#ifndef BOOT_ENABLE_STAY_IN_BOOT
#define BOOT_ENABLE_STAY_IN_BOOT   0//1
#endif
#ifndef BOOT_START_DELAY_MS
#define BOOT_START_DELAY_MS        BOOT_TIMEOUT_MS          /* from App_Uds.h */
#endif

/* Number of entries in the vector table   */
#define APPLVECT_SIZE        64u          

#define BOOT_ID          0xA5u        /* this code tells that all relocated interrupt vector table is default pointed to boot reset vector*/
#define APPL_ID          0x5Au        /* this code tells that all relocated interrupt vector table is pointed to apps real reset vector*/


/* 1 = re-check the stored sum over the whole image at every start-up (a few tens of ms)        */
#ifndef BOOT_VERIFY_SUM_AT_STARTUP
#define BOOT_VERIFY_SUM_AT_STARTUP 0
#endif

#define BOOTLOADER_HW_VERSION_MAJOR    1
#define BOOTLOADER_HW_VERSION_MINOR    0
#define BOOTLOADER_HW_VERSION_PATCH    0

#define BOOTLOADER_SW_VERSION_MAJOR    1
#define BOOTLOADER_SW_VERSION_MINOR    0
#define BOOTLOADER_SW_VERSION_PATCH    0


typedef struct
{
   u8 Hw_MajorPartNumber;
   u8 Hw_MinorPartNumber;
   u8 Hw_PatchPartNumber;
   u8 SerialNumber[20];
   u8 Reserved;

}AppFblShared_T;


/*----------------------------------------------------------------------------------------------*/
/* Reprogramming request (RAM, survives a reset). Set by the application, consumed by the boot.  */
/*----------------------------------------------------------------------------------------------*/
void BootReq_Set(void);
u8   BootReq_IsSet(void);
void BootReq_Clear(void);
u32 Flash_ReadU32LE(u32 addr);

/*----------------------------------------------------------------------------------------------*/
/* Application presence                                                                         */
/*----------------------------------------------------------------------------------------------*/
u8   App_GetState(void);                       /* APP_STATE_xxx                                  */
u8   App_IsValid(void);                        /* 1 = PRESENT                                    */
u8   App_Invalidate(void);                     /* 0 = OK. Programs the mask, no erase            */
u8   App_Validate(u32 length, u16 sum);        /* 0 = OK. Needs an erased trailer block          */
void App_GetTrailer(u32 *length, u16 *sum);    /* raw trailer values (0xFFFF / 0xFFFFFFFF if blank) */

/*----------------------------------------------------------------------------------------------*/
/* 16-bit checksum over flash                                                                   */
/*   sum = 0; sum = Boot_Sum16Update(sum, addr, n) ...; result = Boot_Sum16Final(sum);          */
/* addr and n must be even. The 16-bit words are little endian (the way the 78K0R reads them).  */
/* All words of the image plus the result add up to 0 (mod 65536).                              */
/*----------------------------------------------------------------------------------------------*/
u16  Boot_Sum16Update(u16 sum, u32 addr, u32 len);
u16  Boot_Sum16Final(u16 sum);

/*----------------------------------------------------------------------------------------------*/
/* Start-up                                                                                     */
/*   call Boot_Startup() right after reset, BEFORE CAN / timer / UDS are initialised:           */
/*     - programming request flag set  -> returns, boot stays (flag is cleared)                 */
/*     - no valid application          -> returns, boot stays                                   */
/*     - valid application, window off -> jumps to the application, never returns               */
/*     - valid application, window on  -> returns, window armed                                 */
/*   Boot_Task() every 1 ms from the main loop: closes the window or jumps to the application.  */
/*----------------------------------------------------------------------------------------------*/
void Boot_Startup(void);
void Boot_Task(void);
void Boot_JumpToApp(void);
void Boot_DeinitHook(void);                    /* stop what the boot started: 1 ms timer, CAN ... */

#endif
