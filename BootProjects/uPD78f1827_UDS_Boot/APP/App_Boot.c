
#pragma section @@CNSTL APPFBLSH

/* App_Boot.c : boot manager (see App_Boot.h for the flash layout and the start-up rules) */
#include "CG_macrodriver.h"         
#include "system_def.h"
#include "App_Boot.h"
#include "App_Flash.h"
#include "HAL_WDT_API.h"

 
extern u8  AppFblId;
extern u8  AppResetVec[2];

/*SPECIAL NOTE:-
   A short (near) instruction has only 16 address bits, and the CPU always fills the top 4 bits with F. 
   That is fixed behaviour and has nothing to do with PMC.
   Instruction fetch: Always real flash. Code is never fetched from the mirror.
   Near constant reads: These use the mirror. The short instruction forms an F....H address, and the mirrored copy returns the flash byte. 
   This is the default for const data.
   Far constant reads: These skip the mirror. The long instruction supplies the upper bits through ES, so it reads the real flash address directly.
   THIS IS A REAL issue:- be careful i.e any code constant can be placed in the mirror range (F1000 - FE6FF) without any keyword far or near, but for any other 
   range ,you need to specify the far keyword, but if you speficy a memory read out of above mirror range, there are chances it will read from SFR (i.e say 00D8
   would read from F00D8 ,so be careul before placing constants in memory ranges accordingly)
   use _far keyword only for constants that end up at 0x0000–0x0FFF or above 0xE6FF,(look at the mirror area)becuase the 0000-fff and e6ff will use mirror area
*/ 
const __far AppFblShared_T AppFblShared  = {BOOTLOADER_HW_VERSION_MAJOR,
                                           BOOTLOADER_HW_VERSION_MINOR,
                                           BOOTLOADER_HW_VERSION_PATCH,
                                           "SN0123456789ABCDEF00",0};

/*==============================================================================================*/
/* Reprogramming request flag                 */
/* Two RAM words (magic and its inverse) defined in BootFlag.c, which is placed at the same     */
/* fixed address in the boot and in the application project and must not be cleared by the      */
/* start-up code.                                                                               */
/*==============================================================================================*/
extern volatile u32 gBootReqMagic;
extern volatile u32 gBootReqInv;



void BootReq_Set(void)
{
   gBootReqMagic = BOOT_REQUEST_MAGIC;
   gBootReqInv   = (u32)(~BOOT_REQUEST_MAGIC);
}

u8 BootReq_IsSet(void)
{
   return (u8)((gBootReqMagic == BOOT_REQUEST_MAGIC) && (gBootReqInv == (u32)(~BOOT_REQUEST_MAGIC)));
}

void BootReq_Clear(void)
{
   gBootReqMagic = 0UL;
   gBootReqInv   = 0UL;
}

/*==============================================================================================*/
/* Flash reading. Flash above 0xFFFF needs a far pointer in the small memory model.             */
/* CHECK the far keyword spelling (__far / far) for your CA78K0R version.                        */
/*==============================================================================================*/
#ifndef FLASH_READ8
#define FLASH_READ8(a)   (*(const __far u8 *)(u32)(a))
#endif
#ifndef FLASH_READ16
#define FLASH_READ16(a)  (*(const __far u16 *)(u32)(a))     /* a must be even */
#endif

u32 Flash_ReadU32LE(u32 addr)
{
   u32 v;
   v  = (u32)FLASH_READ8(addr);
   v |= (u32)FLASH_READ8(addr + 1UL) << 8;
   v |= (u32)FLASH_READ8(addr + 2UL) << 16;
   v |= (u32)FLASH_READ8(addr + 3UL) << 24;
   return v;
}

static void PutU32LE(u8 *p, u32 v)
{
   p[0] = (u8)(v);
   p[1] = (u8)(v >> 8);
   p[2] = (u8)(v >> 16);
   p[3] = (u8)(v >> 24);
}

/*==============================================================================================*/
/* 16-bit sum                                                                                   */
/*==============================================================================================*/
u16 Boot_Sum16Update(u16 sum, u32 addr, u32 len)
{
   u8 wd = 0u;

   while(len >= 2UL)
   {
      sum  = (u16)(sum + FLASH_READ16(addr));
      addr += 2UL;
      len  -= 2UL;
      if(++wd == 0u)              /* every 256 words */
      {
         WDT_Restart();
      }
   }
   return sum;
}

u16 Boot_Sum16Final(u16 sum)
{
   return (u16)(0u - sum);
}

/*==============================================================================================*/
/* Application presence pattern / mask   */
/*==============================================================================================*/
void App_GetTrailer(u32 *length, u16 *sum)
{
   *sum    = (u16)Flash_ReadU32LE(APP_TRAILER_ADDR);
   *length = Flash_ReadU32LE(APP_TRAILER_ADDR + 4UL);
}

u8 App_GetState(void)
{
   u32 pattern = Flash_ReadU32LE(APP_PATTERN_ADDR);
   u32 mask    = Flash_ReadU32LE(APP_MASK_ADDR);
   u32 length  = Flash_ReadU32LE(APP_TRAILER_ADDR + 4UL);

   if((pattern == APP_VALID_MAGIC) && (mask == APP_MASK_ERASED) &&
      (length != 0UL) && (length <= APP_MAX_IMAGE_SIZE))
   {
      return APP_STATE_PRESENT;
   }
   if(mask == APP_MASK_REVOKED)
   {
      return APP_STATE_REVOKED;
   }
   return APP_STATE_INVALID;
}

u8 App_IsValid(void)
{
   u8 state = FALSE;
   if((App_GetState() == APP_STATE_PRESENT) && /*magic flag pattern and mask erased*/
      (APPL_ID == AppFblId))/*App Vector boot exist */
   {
     state = TRUE;
   }
   return state;
}

/* Revoke the application by programming the mask word. No erase. Does nothing if the            */
/* application is not PRESENT (blank, torn or already revoked: it is not valid anyway).         */
u8 App_Invalidate(void)
{
   u8 buf[4];
   u8 r;

   if(App_GetState() != APP_STATE_PRESENT)
   {
      return 0u;
   }
   PutU32LE(buf, APP_MASK_REVOKED);
   r = Flash_WriteWords(APP_MASK_ADDR, buf, 1u);
   if(r != 0u)
   {
      return r;
   }
   return (u8)((App_GetState() == APP_STATE_PRESENT) ? APP_ERR_STATE : 0u);
}

/* sum + length first (one FSL write), the pattern word LAST (own FSL write).                    */
/* The words must still be erased, so this only works after the last block was erased.          */
u8 App_Validate(u32 length, u16 sum)
{
   u8 buf[8];
   u8 r;

   if((length == 0UL) || (length > APP_MAX_IMAGE_SIZE))
   {
      return FLASH_ERR_PARAM;
   }

   PutU32LE(&buf[0], 0xFFFF0000UL | (u32)sum);
   PutU32LE(&buf[4], length);
   r = Flash_WriteWords(APP_TRAILER_ADDR, buf, 2u);
   if(r != 0u)
   {
      return r;
   }

   PutU32LE(&buf[0], APP_VALID_MAGIC);
   r = Flash_WriteWords(APP_PATTERN_ADDR, buf, 1u);
   if(r != 0u)
   {
      return r;
   }
   return (u8)((App_GetState() == APP_STATE_PRESENT) ? 0u : APP_ERR_STATE);
}

/*==============================================================================================*/
/* Start-up 
/*==============================================================================================*/
static u16 gBootTimer;                /* 0 = no stay-in-boot window running */

#if (BOOT_VERIFY_SUM_AT_STARTUP == 1)
static u8 App_SumMatches(void)
{
   u32 length;
   u16 stored;
   u16 sum = 0u;
   u32 addr = APP_START_ADDR;
   u32 n;

   App_GetTrailer(&length, &stored);
   while(length != 0UL)
   {
      n = (length > 16384UL) ? 16384UL : length;
      sum     = Boot_Sum16Update(sum, addr, n);
      addr   += n;
      length -= n;
   }
   return (u8)(Boot_Sum16Final(sum) == stored);
}
#endif

void Boot_Startup(void)
{
   u8 powerOnRead ;
   u8 powerOnResetRead ;

   powerOnRead = ((POCRES & 0x01u) == 0u);
   powerOnResetRead = RESF;

   #if (BOOT_ENABLE_STAY_IN_BOOT == 1)
   gBootTimer = 0u;
   #endif
   /*Read Reset and check*/
   if(0u == powerOnRead)/*Power on Reset*/
   {
      POCRES = 0x01u;//clear
      BootReq_Clear();
   }

   if(BootReq_IsSet())  /* reprogramming request from the application: stay in boot*/
   {
      BootReq_Clear();/*empty MAGIC_FLAG */
      return;
   }
   else /* jump to app ;no stay request (empty MAGIC_FLAG)*/
   {
      #if (BOOT_ENABLE_STAY_IN_BOOT == 1)
         BootReq_Set();
      #endif
   }
   if(App_IsValid() == 0u)  /* nothing to start: stay in boot*/
   {
      return;
   }
#if (BOOT_VERIFY_SUM_AT_STARTUP == 1)
   if(App_SumMatches() == 0u)           /* image damaged since it was validated */
   {
      (void)App_Invalidate();
      return;
   }
#endif

#if (BOOT_ENABLE_STAY_IN_BOOT == 1)
   gBootTimer = (u16)BOOT_START_DELAY_MS;      /* wait for a tester, see Boot_Task() */
#else
   Boot_JumpToApp();                              /* never returns */
#endif
}

void Boot_Task(void)                     /* every 1 ms */
{
#if (BOOT_ENABLE_STAY_IN_BOOT == 1)
   if(gBootTimer != 0u)
   {
      if(UDS_WasRequestReceived() != 0u)         /* tester talked to us: stay in the boot */
      {
         gBootTimer = 0u;
      }
      else if(--gBootTimer == 0u)
      {
         Boot_JumpToApp();
      }
   }
#endif
}

/* Add the calls that stop what the boot started (the 1 ms timer interrupt, CAN, ...).          */
void Boot_DeinitHook(void)
{
}

#ifndef BOOT_CALL_APP
#define BOOT_CALL_APP()   ((void (*)(void))APP_START_ADDR)()
#endif

void Boot_JumpToApp(void)
{
   u16 applStartAddress;

   applStartAddress =  (u16)AppResetVec[0] | ((u16)AppResetVec[1] << 8);

   DI();
   WDT_Restart();                       
   Boot_DeinitHook();
   /* The application start-up code */
   ((void (*)(void))applStartAddress)();

   for(;;) { }
}



