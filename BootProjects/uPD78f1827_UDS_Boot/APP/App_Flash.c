/* Thin glue between UDS and the NEC/Renesas Flash Self-programming Library (FSL V1.20).        */
/* Only this file needs to know the FSL API. fsl.lib + fsl_user.c must be part of the project.  */

#include "system_def.h"
#include "App_Flash.h"
#include "App_Uds.h"            /* APP_START_ADDR / CONFIG_START_ADDR                          */
#include "HAL_WDT_API.h"
#include "fsl.h"
#include "fsl_user.h"

extern fsl_u08 fsl_data_buffer[FSL_DATA_BUFFER_SIZE];      /* defined in fsl_user.c            */

#define FLASH_WORD_BYTES    4u
#define FLASH_MAX_WORDS     (FSL_DATA_BUFFER_SIZE / FLASH_WORD_BYTES)

static u8 Flash_WriteChunk(u32 addr, const u8 *pData, u8 wordCount);

/*------------------------------------------------------------------------------------------*/
/* Open an FSL session. FLMD0 stays HIGH (and interrupts stay masked, FSL_INT_BACKUP) until    */
/* Flash_Session_Close(). Always call Close, even if this returns an error.                    */
/*------------------------------------------------------------------------------------------*/
static fsl_u08 Flash_Session_Open(void)
{
    fsl_u08 fslRetCode;

    /*
       NOTE:- On the 78K0R the stack cannot live inside the forbidden 0xFFE20–0xFFEFF range (check section 24.10) when self programming is enabled.
    */    
    FSL_Open();
    fslRetCode = FSL_Init(fsl_data_buffer);          /* NOTE: FSL_Init changes fsl_data_buffer[0]       */
    while(fslRetCode == FSL_ERR_INTERRUPTION)
    {
        fslRetCode = FSL_Init_cont(fsl_data_buffer);
    }
    if(fslRetCode == FSL_OK)
    {
        fslRetCode = FSL_ModeCheck();                /* FSL_ERR_FLMD0 if FLMD0 did not go HIGH          */
    }
    return fslRetCode;
}

static void Flash_Session_Close(void)
{
    FSL_Close();
}

/*------------------------------------------------------------------------------------------*/
u8 Flash_EraseBlock(u32 blockStartAddr)
{
    fsl_u08 r;
    fsl_u16 block;

    /* Only the application area may be erased; never boot block or the reserved top 1 KB      */
    if((blockStartAddr < APP_START_ADDR) ||
       ((blockStartAddr + FLASH_BLOCK_SIZE) > CONFIG_START_ADDR) ||
       ((blockStartAddr & (FLASH_BLOCK_SIZE - 1UL)) != 0UL))
    {
        return FLASH_ERR_PARAM;
    }

    block = (fsl_u16)(blockStartAddr / FLASH_BLOCK_SIZE);

    WDT_Restart();
    r = Flash_Session_Open();
    if(r == FSL_OK)
    {
        do 
        { 
           r = FSL_BlankCheck(block); 
        }while(r == FSL_ERR_INTERRUPTION);

        if(r == FSL_ERR_BLANKCHECK)         /* not blank -> erase (blank blocks are skipped)   */
        {
            do 
            { 
               r = FSL_Erase(block); 
            } while(r == FSL_ERR_INTERRUPTION);
        }
    }
    Flash_Session_Close();
    WDT_Restart();

    return (u8)r;
}

#if 0
/*------------------------------------------------------------------------------------------*/
u8 Flash_WriteWords(u32 addr, const u8 *pData, u8 wordCount)
{
    fsl_u08 r;
    u16 i;
    u16 nBytes;

    if((wordCount == 0u) || (wordCount > FLASH_MAX_WORDS) ||
       ((addr & 3UL) != 0UL) ||
       (addr < APP_START_ADDR) ||
       ((addr + ((u32)wordCount * FLASH_WORD_BYTES)) > CONFIG_START_ADDR))
    {
        return FLASH_ERR_PARAM;
    }

    nBytes = (u16)wordCount * FLASH_WORD_BYTES;

    WDT_Restart();
    r = Flash_Session_Open();
    if(r == FSL_OK)
    {
        /* Fill the FSL buffer AFTER FSL_Init (Init overwrites byte 0) */
        for(i = 0u; i < nBytes; i++)
        {
            fsl_data_buffer[i] = pData[i];
        }

        /* FSL_Write = blank check + write + internal verify of wordCount words */
        do { r = FSL_Write(addr, (fsl_u08)wordCount); } while(r == FSL_ERR_INTERRUPTION);
    }
    Flash_Session_Close();
    WDT_Restart();

    return (u8)r;
}
#else 
static u8 Flash_WriteChunk(u32 addr, const u8 *pData, u8 wordCount)
{
    fsl_u08 r;
    u16 i;
    u16 nBytes;

    nBytes = (u16)wordCount * FLASH_WORD_BYTES;

    WDT_Restart();
    r = Flash_Session_Open();
    if(r == FSL_OK)
    {
        /* Fill the FSL buffer AFTER FSL_Init (Init overwrites byte 0) */
        for(i = 0u; i < nBytes; i++)
        {
            fsl_data_buffer[i] = pData[i];
        }

        /* FSL_Write = blank check + write + internal verify of wordCount words */
        do { r = FSL_Write(addr, (fsl_u08)wordCount); } while(r == FSL_ERR_INTERRUPTION);
    }
    Flash_Session_Close();
    WDT_Restart();

    return (u8)r;
}

/*------------------------------------------------------------------------------------------*/
/* Any length: split into chunks of FLASH_MAX_WORDS. Interrupts run between the chunks.        */
/*------------------------------------------------------------------------------------------*/
u8 Flash_WriteWords(u32 addr, const u8 *pData, u16 wordCount)
{
    u8  r;
    u8  chunk;

    if((wordCount == 0u) ||
       ((addr & 3UL) != 0UL) ||
       (addr < APP_START_ADDR) ||
       ((addr + ((u32)wordCount * FLASH_WORD_BYTES)) > CONFIG_START_ADDR))
    {
        return FLASH_ERR_PARAM;
    }

    while(wordCount != 0u)
    {
        chunk = (wordCount > FLASH_MAX_WORDS) ? (u8)FLASH_MAX_WORDS : (u8)wordCount;

        r = Flash_WriteChunk(addr, pData, chunk);
        if(r != 0u)
        {
            return r;
        }

        addr      += ((u32)chunk * FLASH_WORD_BYTES);
        pData     += ((u16)chunk * FLASH_WORD_BYTES);
        wordCount -= chunk;
    }

    return 0u;
}
#endif




/* Erase a contiguous, block-aligned range [start, end) in ONE FSL session. */
u8 Flash_EraseRange(u32 start, u32 end)
{
    fsl_u08 r;
    fsl_u16 block;
    fsl_u16 first;
    fsl_u16 last;

    /* Same safety checks you already have */
    if ((start < APP_START_ADDR) ||
        (end <= start) ||
        (end > CONFIG_START_ADDR) ||
        ((start & (FLASH_BLOCK_SIZE - 1UL)) != 0UL) ||
        ((end   & (FLASH_BLOCK_SIZE - 1UL)) != 0UL))
    {
        return FLASH_ERR_PARAM;
    }

    first = (fsl_u16)(start / FLASH_BLOCK_SIZE);
    last  = (fsl_u16)((end - 1UL) / FLASH_BLOCK_SIZE);

    r = Flash_Session_Open();          /* Open + Init + ModeCheck  ONCE */
    if (r != FSL_OK)
    {
        Flash_Session_Close();
        return (u8)r;
    }

    for (block = first; block <= last; block++)
    {
        /* Blank check */
        do {
            r = FSL_BlankCheck(block);
        } while (r == FSL_ERR_INTERRUPTION);

        if (r == FSL_ERR_BLANKCHECK)   /* not blank ? erase */
        {
            do {
                r = FSL_Erase(block);
            } while (r == FSL_ERR_INTERRUPTION);
        }

        if ((r != FSL_OK) && (r != FSL_ERR_BLANKCHECK))
        {
            break;                     /* real error ? stop */
        }
    }

    Flash_Session_Close();             /* Close ONCE */
    return (u8)r;
}





