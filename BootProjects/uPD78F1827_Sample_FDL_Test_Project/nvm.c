#pragma sfr
/* nvm.c - NVM manager: 2 logical blocks, 2 redundant copies, immediate + background write
 * See nvm.h for the storage model. C89 style for CA78K0R.
 */
#include "nvm.h"
#include "fal.h"
#include "nvm_cfg.h"



nvm_blk_t     g_blk[NVM_BLK_COUNT];
nvm_job_t     g_job;
fal_request_t g_req;
u8       g_rej;
u8       g_rr;                          /* round-robin start for pending  */
u8       g_ready;

/*============================================================================*/
/* public API                                                                 */
/*============================================================================*/
nvm_state_t NVM_Init(void)
{
    u8 b;
    u8 o;
    u8 bad = 0U;
    u8 *p;
    u16 i;

    g_ready = 0U;
    p = (u8 *)&g_blk[0];
    for (i = 0U; i < (u16)sizeof(g_blk); i++) { p[i] = 0U; }
    p = (u8 *)&g_job;
    for (i = 0U; i < (u16)sizeof(g_job); i++) { p[i] = 0U; }
    g_rr = 0U;

    /* configuration sanity: copies in different, valid FDL blocks, no sharing */
    for (b = 0U; b < NVM_BLK_COUNT; b++)
    {
        g_blk[b].init   = NVM_INIT_FAILED;
        g_blk[b].result = NVM_REQ_NOT_OK;
        g_blk[b].sw     = (u16)NVM_SLOT_WORDS(nvm_cfg[b].size);
        g_blk[b].slots  = (u16)(NVM_WORDS_PER_ERASE_BLOCK / g_blk[b].sw);

        if ((nvm_cfg[b].fdl_block[0] == nvm_cfg[b].fdl_block[1])
         || (nvm_cfg[b].fdl_block[0] >= NVM_FDL_BLOCK_COUNT)
         || (nvm_cfg[b].fdl_block[1] >= NVM_FDL_BLOCK_COUNT)
         || (g_blk[b].slots == 0U))
        {
            bad = 1U;
        }
        for (o = (u8)(b + 1U); o < NVM_BLK_COUNT; o++)
        {
            if ((nvm_cfg[b].fdl_block[0] == nvm_cfg[o].fdl_block[0])
             || (nvm_cfg[b].fdl_block[0] == nvm_cfg[o].fdl_block[1])
             || (nvm_cfg[b].fdl_block[1] == nvm_cfg[o].fdl_block[0])
             || (nvm_cfg[b].fdl_block[1] == nvm_cfg[o].fdl_block[1]))
            {
                bad = 1U;
            }
        }
    }
    if (bad != 0U)
    {
        return NVM_E_NOT_OK;
    }

    if (FAL_Init((const fal_descriptor_t __far *)&fal_descriptor_str) != FAL_OK)
    {
        return NVM_E_NOT_OK;
    }
    FAL_Open();

    for (b = 0U; b < NVM_BLK_COUNT; b++)
    {
        g_blk[b].result = NVM_REQ_OK;
        Init_Block(b);
    }
    g_ready = 1U;
    return NVM_E_OK;
}

void NVM_MainFunction(void)
{
    u8 i;
    u8 b;

    if (g_ready == 0U)
    {
        return;
    }
    if (g_job.active != 0U)
    {
        (void)Job_Step();
        return;
    }
    for (i = 0U; i < NVM_BLK_COUNT; i++)
    {
        b = (u8)((g_rr + i) % NVM_BLK_COUNT);
        if (g_blk[b].pending != 0U)
        {
            g_blk[b].pending = 0U;
            g_rr = (u8)((b + 1U) % NVM_BLK_COUNT);
            g_blk[b].count++;
            Job_Start(b, g_blk[b].count, 0x03U);
            (void)Job_Step();
            return;
        }
    }
}

void NVM_Ram_MirrorStore(u8 b, const void *src)
{
    const u8 *s = (const u8 *)src;
    u16 i;

    if (s != 0)
    {
        for (i = 0U; i < nvm_cfg[b].size; i++)
        {
            nvm_cfg[b].ram[i] = s[i];
        }
    }
}

nvm_state_t NVM_ReadBlock(u8 block, void *dst)
{
    u8 *d = (u8 *)dst;
    u16 i;

    if ((block >= NVM_BLK_COUNT) || (g_ready == 0U) || (d == 0))
    {
        return NVM_E_NOT_OK;
    }
    for (i = 0U; i < nvm_cfg[block].size; i++)
    {
        d[i] = nvm_cfg[block].ram[i];
    }
    return NVM_E_OK;
}

nvm_state_t NVM_WriteBlock(u8 block, const void *src)
{
    if ((block >= NVM_BLK_COUNT) || (g_ready == 0U))
    {
        return NVM_E_NOT_OK;
    }
    NVM_Ram_MirrorStore(block, src);
    g_blk[block].pending = 1U;
    g_blk[block].result  = NVM_REQ_PENDING;
    return NVM_E_OK;
}

nvm_state_t NVM_WriteBlockImmediate(u8 block, const void *src)
{
    nvm_blk_t *k;
    u8 tries = 0U;

    if ((block >= NVM_BLK_COUNT) || (g_ready == 0U))
    {
        return NVM_E_NOT_OK;
    }
    k = &g_blk[block];

    Job_Run_Blocking();                 /* let a running background job finish first */
    NVM_Ram_MirrorStore(block, src);
    do
    {
        k->pending = 0U;
        k->count++;
        Job_Start(block, k->count, 0x03U);
        Job_Run_Blocking();
        tries++;
    } while ((k->pending != 0U) && (tries <= NVM_WRITE_RETRIES));  /* pending again = shadow changed during the write */
    return (k->result == NVM_REQ_OK) ? NVM_E_OK : NVM_E_NOT_OK;
}

u8 NVM_Check_PendingWrite(void)
{
    u8 b;

    for (b = 0U; b < NVM_BLK_COUNT; b++)
    {
        if (g_blk[b].pending != 0U)
        {
            return 1U;
        }
    }
    return 0U;
}

void NVM_FlushPending(void)
{
    while ((g_job.active != 0U) || (NVM_Check_PendingWrite() != 0U))
    {
        NVM_KICK_WDT();
        NVM_MainFunction();
    }
}

u8 *NVM_GetRamPtr(u8 block)
{
    return (block < NVM_BLK_COUNT) ? nvm_cfg[block].ram : (u8 *)0;
}

nvm_req_t NVM_GetErrorStatus(u8 block)
{
    return (block < NVM_BLK_COUNT) ? g_blk[block].result : NVM_REQ_NOT_OK;
}

nvm_init_t NVM_GetInitStatus(u8 block)
{
    return (block < NVM_BLK_COUNT) ? g_blk[block].init : NVM_INIT_FAILED;
}

u8 NVM_GetCopyHealth(u8 block)
{
    return (block < NVM_BLK_COUNT) ? g_blk[block].health : 0U;
}

u32 NVM_GetWriteCount(u8 block)
{
    return (block < NVM_BLK_COUNT) ? g_blk[block].count : 0UL;
}

u8 NVM_IsBusy(void)
{
    return (u8)((g_job.active != 0U) || (NVM_Check_PendingWrite() != 0U));
}

/* while a write of this block is queued or running: avoid changing its shadow meanwhile */
u8 NVM_IsBlockBusy(u8 block)
{
    if (block >= NVM_BLK_COUNT)
    {
        return 0U;
    }
    return (u8)(((g_job.active != 0U) && (g_job.blk == block)) || (g_blk[block].pending != 0U));
}
