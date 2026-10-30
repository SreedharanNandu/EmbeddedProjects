#ifndef APP_FLASH_H
#define APP_FLASH_H

#include "system_def.h"

/* Erase unit of the code flash (FSL block = 1 KB). CHECK against the 78F1827 user's manual / FSL docs. */
/*Figure 3-5. Memory Map in CPU architecture chapter 3*/
#define FLASH_BLOCK_SIZE    0x400UL

/* Flash glue error code for rejected parameters (FSL codes 0x01..0x1F are passed through as-is). */
#define FLASH_ERR_PARAM     0xFFu

/* Both return 0 = OK, non-zero = failure (FSL status code, or FLASH_ERR_PARAM).                  */
/* Each call is a complete FSL session: FSL_Open -> FSL_Init -> FSL_ModeCheck -> command -> FSL_Close. */
u8 Flash_EraseBlock(u32 blockStartAddr);                          /* erase 1 block (blank-checked first) */
#if 0
u8 Flash_WriteWords(u32 addr, const u8 *pData, u8 wordCount);     /* 4-byte words, FSL-verified          */
#else
u8 Flash_WriteWords(u32 addr, const u8 *pData, u16 wordCount);     /* 4-byte words, FSL-verified          */
#endif
u8 Flash_EraseRange(u32 start, u32 end);
#endif
