/* nvm.h - NVM manager API (AUTOSAR NvM style) on top of the FAL/FDL port
 *
 * Model
 *  - 2 logical blocks (SLOW, FAST), each kept in RAM and stored as 2 redundant copies in
 *    data flash. A copy is an append log of fixed-size slots inside one erase block.
 *  - Slot = header [write count][id|rev|len]  data  trailer [checksum][valid flag].
 *    Words are programmed in that order, the valid flag last, so a slot is either fully
 *    committed or ignored. A write goes to copy A first, is verified, and only then to copy B.
 *  - NVM_Init() reads both copies, takes the newest valid one, repairs the other copy, or
 *    loads defaults if nothing valid exists.
 *
 * Writing
 *  - NVM_WriteBlock()          : copy to RAM mirror, flash write happens in NVM_MainFunction()
 *  - NVM_WriteBlockImmediate() : copy to RAM mirror and write both copies now (blocking)
 *
 * Call NVM_MainFunction() periodically from the main loop (e.g. every 1..5 ms).
 * Do not call any NVM function from an interrupt while the main loop is inside NVM code.
 */
#ifndef NVM_H
#define NVM_H


#include "nvm_cfg.h"
#include "fal.h"

/* copy health bits (NVM_GetCopyHealth): 1 = copy holds the current data */
#define NVM_COPY_A_OK   0x01U
#define NVM_COPY_B_OK   0x02U

#define NVM_BLANK32         0xFFFFFFFFUL
#define NVM_VALID32         0xA5A5A5A5UL
#define NVM_REJECT_RETRIES  100U

/* write job phases */
typedef enum
{
   NVM_FSM_NEXT_COPY  =  0U,
   NVM_FSM_COPY_BEGIN =  1U,
   NVM_FSM_ERASE      =  2U,
   NVM_FSM_WRITE      =  3U,
   NVM_FSM_VERIFY     =  4U
}nvm_fsm_state_t;
/* compile-time checks */
typedef char nvm_chk_slow [(NVM_SLOW_DATA_SIZE <= NVM_MAX_DATA_SIZE) ? 1 : -1];
typedef char nvm_chk_fast [(NVM_FAST_DATA_SIZE <= NVM_MAX_DATA_SIZE) ? 1 : -1];
typedef char nvm_chk_slot [(NVM_MAX_SLOT_WORDS <= NVM_WORDS_PER_ERASE_BLOCK) ? 1 : -1];

/* function return codes */
typedef enum
{
   NVM_E_OK=0U,
   NVM_E_NOT_OK=1U
}nvm_state_t;
/* request result (NVM_GetErrorStatus) */
typedef enum
{
   NVM_REQ_OK=0U,
   NVM_REQ_PENDING=1U,
   NVM_REQ_NOT_OK=2U    /* last flash write failed on at least one copy */
}nvm_req_t;

/* NVM_Init result per block (NVM_GetInitStatus) */
typedef enum
{
   NVM_INIT_OK=0U,  /* both copies valid and identical               */
   NVM_INIT_RECOVERED=1U,  /* one copy was bad/old and was rewritten        */
   NVM_INIT_DEFAULTED=2U,  /* no valid copy: defaults loaded and written    */
   NVM_INIT_FAILED=3U  /* FAL init/config error, block not usable       */
}nvm_init_t;

typedef struct 
{
    u16 next_free;          /* next slot to write; == slots means erase first */
    u16 slot;               /* slot of the newest valid record                */
    u32 count;              /* write count of the newest valid record         */
    u8 valid;              /* copy holds a valid record                      */
} nvm_copy_t;

typedef struct 
{
    nvm_copy_t cp[2];
    u32 count;              /* last write count issued for this block         */
    u16 sw;                 /* slot size in words                             */
    u16 slots;              /* slots per erase block                          */
    u8 pending;            /* background write requested                     */
    nvm_req_t result;           /* NVM_REQ_xxx                                    */
    nvm_init_t init;            /* NVM_INIT_xxx                                   */
    u8 health;             /* NVM_COPY_x_OK bits                             */
} nvm_blk_t;

typedef struct 
{
    u8 active;
    u8 blk;
    u8 mask;               /* copies to write: bit0 = A, bit1 = B            */
    u8 copy;               /* copy being written                             */
    u8 fail;               /* copies that failed                             */
    nvm_fsm_state_t phase;
    u8 pend;               /* a FAL command is in flight                     */
    u8 retries;
    u16 slot;
    u16 w;                  /* word being written                             */
    u32 count;
    u32 word;               /* word currently written / verified              */
    u16 crc;                /* expected CRC of header + data (from the shadow) */
    u16 run;                /* CRC of the words actually written so far       */
} nvm_job_t;




extern nvm_blk_t     g_blk[NVM_BLK_COUNT];
extern nvm_job_t     g_job;
extern u8       g_rej;
extern u8       g_rr;                          /* round-robin start for pending  */
extern u8       g_ready;


nvm_state_t NVM_Init(void);
void NVM_MainFunction(void);

nvm_state_t NVM_ReadBlock(u8 block, void *dst);                   /* RAM mirror -> dst   */
nvm_state_t NVM_WriteBlock(u8 block, const void *src);            /* background write    */
nvm_state_t NVM_WriteBlockImmediate(u8 block, const void *src);   /* blocking write      */
u8 *NVM_GetRamPtr(u8 block);                              /* edit in place, then
                                                                       call NVM_WriteBlock(block, 0) */
void NVM_FlushPending(void);                                    /* finish all background
                                                                       writes (power-down)  */

nvm_req_t NVM_GetErrorStatus(u8 block);
nvm_init_t NVM_GetInitStatus(u8 block);
u8 NVM_GetCopyHealth(u8 block);
u32 NVM_GetWriteCount(u8 block);
u8 NVM_IsBusy(void);
u8 NVM_IsBlockBusy(u8 block);

u8 NVM_Check_PendingWrite(void);
void NVM_Ram_MirrorStore(u8 b, const void *src);

#endif
