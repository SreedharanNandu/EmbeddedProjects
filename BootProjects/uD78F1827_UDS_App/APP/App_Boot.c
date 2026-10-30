#pragma section @@CNSTL APPFBLSH


/* App_Boot.c : boot manager (see App_Boot.h for the flash layout and the start-up rules) */
#include "CG_macrodriver.h"       
#include "system_def.h"
#include "App_Boot.h"
#include "HAL_WDT_API.h"

/*==============================================================================================*/
/* Reprogramming request flag                                                                   */
/* Two RAM words (magic and its inverse) defined in BootFlag.c, which is placed at the same     */
/* fixed address in the boot and in the application project and must not be cleared by the      */
/* start-up code.                                                                               */
/*==============================================================================================*/
extern volatile u32 gBootReqMagic;
extern volatile u32 gBootReqInv;


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
const __far AppFblShared_T AppFblShared;


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

void Init_Boot_App(void)
{
   if(BootReq_IsSet())
   {
      BootReq_Clear();
   }
}

/*==============================================================================================*/
/* Flash reading. Flash above 0xFFFF needs a far pointer in the small memory model.             */
/* CHECK the far keyword spelling (__far / far) for your CA78K0R version.                        */
/*==============================================================================================*/
u32 Flash_ReadU32LE(u32 addr)
{
   u32 v;
   v  = (u32)FLASH_READ8(addr);
   v |= (u32)FLASH_READ8(addr + 1UL) << 8;
   v |= (u32)FLASH_READ8(addr + 2UL) << 16;
   v |= (u32)FLASH_READ8(addr + 3UL) << 24;
   return v;
}



/*==============================================================================================
 Application presence pattern / mask 
==============================================================================================*/
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








