#pragma sfr
/* fal.c - flash abstraction layer manager: 2 logical blocks, 2 redundant copies, immediate + background write
 * See nvm.h for the storage model. C89 style for CA78K0R.
 */
#include "system_def.h"
#include "nvm.h"      /* also pulls in nvm_cfg.h and fdl.h */


/*============================================================================*/
/* FAL access                                                                 */
/*============================================================================*/
void Fal_Issue(fal_command_t cmd, u16 idx, u32 data)
{
    g_req.index_u16   = idx;
    g_req.data_u32    = data;
    g_req.command_enu = cmd;
    g_rej = 0U;
    FAL_Execute(&g_req);
}

/* returns FAL_BUSY while the command is running */
fal_status_t Fal_Poll(void)
{
    if (g_req.status_enu == FAL_BUSY)
    {
        FAL_Handler();
    }
    if (g_req.status_enu == FAL_ERR_REJECTED)
    {
        if (g_rej < NVM_REJECT_RETRIES)
        {
            g_rej++;
            FAL_Execute(&g_req);
            return FAL_BUSY;
        }
    }
    return g_req.status_enu;
}

fal_status_t Fal_Run(fal_command_t cmd, u16 idx, u32 data)
{
    fal_status_t st;

    Fal_Issue(cmd, idx, data);
    do
    {
        NVM_KICK_WDT();
        st = Fal_Poll();
    } while (st == FAL_BUSY);
    return st;
}

u8 Rd(u16 idx, u32 *v)
{
    if (Fal_Run(FAL_CMD_READ_WORD, idx, 0UL) != FAL_OK)
    {
        return 0U;
    }
    *v = g_req.data_u32;
    return 1U;
}

/*============================================================================*/
/* slot image helpers                                                         */
/*============================================================================*/
u16 Word_Base(u8 b, u8 c, u16 slot)
{
    return (u16)((u16)nvm_cfg[b].fdl_block[c] * NVM_WORDS_PER_ERASE_BLOCK
                     + slot * g_blk[b].sw);
}

u32 Hdr_Word(u8 b)
{
    return ((u32)nvm_cfg[b].id)
         | ((u32)nvm_cfg[b].rev  << 8)
         | ((u32)nvm_cfg[b].size << 16);
}

/* CRC16-CCITT, one 32-bit word at a time, least significant byte first */
u16 Crc16_Word(u16 crc, u32 v)
{
    u8 n;
    u8 k;

    for (n = 0U; n < 4U; n++)
    {
        crc = (u16)(crc ^ (u16)((v & 0xFFUL) << 8));
        v >>= 8;
        for (k = 0U; k < 8U; k++)
        {
            if ((crc & 0x8000U) != 0U)
            {
                crc = (u16)(((crc << 1) ^ 0x1021U) & 0xFFFFU);
            }
            else
            {
                crc = (u16)((crc << 1) & 0xFFFFU);
            }
        }
    }
    return crc;
}

u32 Crc_Word(u16 crc)
{
    return (u32)crc | ((u32)((u16)(~crc)) << 16);
}

/* number of 32-bit data words of block b (data is stored zero padded to 4 bytes) */
u16 Data_Words(u8 b)
{
    return (u16)((nvm_cfg[b].size + 3U) / 4U);
}

/* data word i taken straight from the RAM shadow, least significant byte first */
u32 Data_Word(u8 b, u16 i)
{
    const nvm_block_cfg_t *cfg = &nvm_cfg[b];
    u32 v = 0UL;
    u16 idx = (u16)(i * 4U);
    u8 n;

    for (n = 0U; n < 4U; n++)
    {
        if ((u16)(idx + n) < cfg->size)
        {
            v |= ((u32)cfg->ram[idx + n]) << (n * 8U);
        }
    }
    return v;
}

/* CRC over [count][id|rev|len][data words] of the current shadow */
u16 Shadow_Crc(u8 b, u32 count)
{
    u16 dw = Data_Words(b);
    u16 i;
    u16 crc;

    crc = Crc16_Word(0xFFFFU, count);
    crc = Crc16_Word(crc, Hdr_Word(b));
    for (i = 0U; i < dw; i++)
    {
        crc = Crc16_Word(crc, Data_Word(b, i));
    }
    return crc;
}

/* word w of the slot being written by the current job (generated on the fly) */
u32 Slot_Word(u8 b, u16 w)
{
    u16 dw = Data_Words(b);

    if (w == 0U)                    { return g_job.count; }
    if (w == 1U)                    { return Hdr_Word(b); }
    if (w < (u16)(2U + dw))         { return Data_Word(b, (u16)(w - 2U)); }
    if (w == (u16)(2U + dw))        { return Crc_Word(g_job.crc); }
    return NVM_VALID32;
}

/* validate a slot word by word (no buffer); 1 = valid committed record, *count = its write count */
u8 Slot_Check(u8 b, u8 c, u16 slot, u32 *count)
{
    u16 base = Word_Base(b, c, slot);
    u16 sw   = g_blk[b].sw;
    u16 crc  = 0xFFFFU;
    u16 w;
    u32 v;

    for (w = 0U; w < sw; w++)
    {
        if (Rd((u16)(base + w), &v) == 0U)
        {
            return 0U;
        }
        if (w == 0U)
        {
            if ((v == 0UL) || (v == NVM_BLANK32))
            {
                return 0U;
            }
            *count = v;
        }
        else if (w == 1U)
        {
            if (v != Hdr_Word(b))
            {
                return 0U;
            }
        }
        if (w < (u16)(sw - 2U))
        {
            crc = Crc16_Word(crc, v);
        }
        else if (w == (u16)(sw - 2U))
        {
            if (v != Crc_Word(crc))
            {
                return 0U;
            }
        }
        else
        {
            if (v != NVM_VALID32)
            {
                return 0U;
            }
        }
    }
    return 1U;
}

/* copy the data of an (already validated) slot into the RAM shadow */
u8 Slot_Load(u8 b, u8 c, u16 slot)
{
    const nvm_block_cfg_t *cfg = &nvm_cfg[b];
    u16 base = Word_Base(b, c, slot);
    u16 dw = Data_Words(b);
    u16 w;
    u16 idx;
    u8 n;
    u32 v;

    for (w = 0U; w < dw; w++)
    {
        if (Rd((u16)(base + 2U + w), &v) == 0U)
        {
            return 0U;
        }
        for (n = 0U; n < 4U; n++)
        {
            idx = (u16)(w * 4U + n);
            if (idx < cfg->size)
            {
                cfg->ram[idx] = (u8)((v >> (n * 8U)) & 0xFFUL);
            }
        }
    }
    return 1U;
}

/*============================================================================*/
/* write job (state machine, one FAL command per call)                        */
/*============================================================================*/
void Job_Start(u8 b, u32 count, u8 mask)
{
    g_job.active  = 1U;
    g_job.blk     = b;
    g_job.mask    = mask;
    g_job.copy    = 0U;
    g_job.fail    = 0U;
    g_job.phase   = NVM_FSM_NEXT_COPY;
    g_job.pend    = 0U;
    g_job.retries = 0U;
    g_job.count   = count;
    g_job.crc     = Shadow_Crc(b, count);   /* expected CRC of header + data in the shadow */
    g_blk[b].result = NVM_REQ_PENDING;
}

void Job_Finish(void)
{
    nvm_blk_t *k = &g_blk[g_job.blk];

    k->result   = (g_job.fail != 0U) ? NVM_REQ_NOT_OK : NVM_REQ_OK;
    g_job.active = 0U;
}

/* the shadow changed while this job was writing it: drop the half-written slot
 * (it has no valid flag) and queue a fresh write of the new data */
void Job_Abort(void)
{
    nvm_blk_t *k = &g_blk[g_job.blk];

    k->cp[g_job.copy & 1U].next_free = (u16)(g_job.slot + 1U);
    k->pending   = 1U;
    k->result    = NVM_REQ_PENDING;
    g_job.active = 0U;
}

void Job_Copy_Fail(void)
{
    g_blk[g_job.blk].health &= (u8)(~(1U << g_job.copy));
    g_job.fail |= (u8)(1U << g_job.copy);
    g_job.copy++;
    g_job.phase = NVM_FSM_NEXT_COPY;
}

/* write or verify error: skip the partly written slot and try the next one */
void Job_Slot_Fail(void)
{
    nvm_copy_t *cp = &g_blk[g_job.blk].cp[g_job.copy];

    cp->next_free = (u16)(g_job.slot + 1U);
    g_job.retries++;
    if (g_job.retries > NVM_WRITE_RETRIES)
    {
        Job_Copy_Fail();
    }
    else
    {
        g_job.phase = NVM_FSM_COPY_BEGIN;
    }
}

/* returns 1 while the job is still running, 0 when idle/finished */
u8 Job_Step(void)
{
    nvm_blk_t *k;
    nvm_copy_t *cp;
    fal_status_t st;
    u16 dw;

    if (g_job.active == 0U)
    {
        return 0U;
    }
    k = &g_blk[g_job.blk];

    for (;;)
    {
        cp = &k->cp[g_job.copy & 1U];

        switch (g_job.phase)
        {
        case NVM_FSM_NEXT_COPY:
            while ((g_job.copy < 2U) && (((g_job.mask >> g_job.copy) & 1U) == 0U))
            {
                g_job.copy++;
            }
            if (g_job.copy >= 2U)
            {
                Job_Finish();
                return 0U;
            }
            g_job.retries = 0U;
            g_job.phase   = NVM_FSM_COPY_BEGIN;
            break;

        case NVM_FSM_COPY_BEGIN:
            g_job.pend = 0U;
            cp = &k->cp[g_job.copy];
            if (cp->next_free >= k->slots)
            {
                g_job.phase = NVM_FSM_ERASE;
            }
            else
            {
                g_job.slot  = cp->next_free;
                g_job.w     = 0U;
                g_job.phase = NVM_FSM_WRITE;
            }
            break;

        case NVM_FSM_ERASE:
            cp = &k->cp[g_job.copy];
            if (g_job.pend == 0U)
            {
                cp->valid = 0U;             /* old records in this block are about to go */
                k->health &= (u8)(~(1U << g_job.copy));
                Fal_Issue(FAL_CMD_ERASE_BLOCK, nvm_cfg[g_job.blk].fdl_block[g_job.copy], 0UL);
                g_job.pend = 1U;
                return 1U;
            }
            st = Fal_Poll();
            if (st == FAL_BUSY)
            {
                return 1U;
            }
            g_job.pend = 0U;
            if (st != FAL_OK)
            {
                Job_Copy_Fail();
                break;
            }
            cp->next_free = 0U;
            g_job.slot  = 0U;
            g_job.w     = 0U;
            g_job.phase = NVM_FSM_WRITE;
            break;

        case NVM_FSM_WRITE:
            if (g_job.pend == 0U)
            {
                dw = Data_Words(g_job.blk);
                if (g_job.w == 0U)
                {
                    g_job.run = 0xFFFFU;
                }
                g_job.word = Slot_Word(g_job.blk, g_job.w);
                if (g_job.w < (u16)(2U + dw))
                {
                    g_job.run = Crc16_Word(g_job.run, g_job.word);
                }
                else if (g_job.w == (u16)(2U + dw))
                {
                    if (g_job.run != g_job.crc)         /* shadow changed during the write */
                    {
                        Job_Abort();
                        return 0U;
                    }
                }
                Fal_Issue(FAL_CMD_WRITE_WORD,
                          (u16)(Word_Base(g_job.blk, g_job.copy, g_job.slot) + g_job.w),
                          g_job.word);
                g_job.pend = 1U;
                return 1U;
            }
            st = Fal_Poll();
            if (st == FAL_BUSY)
            {
                return 1U;
            }
            g_job.pend = 0U;
            if (st != FAL_OK)
            {
                Job_Slot_Fail();
                break;
            }
            g_job.phase = NVM_FSM_VERIFY;
            break;

        case NVM_FSM_VERIFY:
            if (g_job.pend == 0U)
            {
                Fal_Issue(FAL_CMD_IVERIFY_WORD,
                          (u16)(Word_Base(g_job.blk, g_job.copy, g_job.slot) + g_job.w),
                          g_job.word);
                g_job.pend = 1U;
                return 1U;
            }
            st = Fal_Poll();
            if (st == FAL_BUSY)
            {
                return 1U;
            }
            g_job.pend = 0U;
            if (st != FAL_OK)
            {
                Job_Slot_Fail();
                break;
            }
            g_job.w++;
            if (g_job.w >= k->sw)
            {
                /* slot committed (valid flag was the last word) */
                cp->slot      = g_job.slot;
                cp->count     = g_job.count;
                cp->valid     = 1U;
                cp->next_free = (u16)(g_job.slot + 1U);
                k->health    |= (u8)(1U << g_job.copy);
                g_job.copy++;
                g_job.phase = NVM_FSM_NEXT_COPY;
            }
            else
            {
                g_job.phase = NVM_FSM_WRITE;
            }
            break;

        default:
            g_job.active = 0U;
            return 0U;
        }
    }
}

void Job_Run_Blocking(void)
{
    while (Job_Step() != 0U)
    {
        NVM_KICK_WDT();
    }
}

/*============================================================================*/
/* init: scan, select newest, repair / defaults                               */
/*============================================================================*/
void Scan_Copy(u8 b, u8 c)
{
    nvm_blk_t  *k;
    nvm_copy_t *cp;
    u16 base;
    u16 used;
    u16 w;
    u16 j;
    u32 cnt;

    k  = &g_blk[b];
    cp = &k->cp[c];
    base = Word_Base(b, c, 0U);

    cp->valid = 0U;
    cp->count = 0UL;
    cp->slot  = 0U;

    /* first blank slot (slots fill sequentially) */
    for (used = 0U; used < k->slots; used++)
    {
        if (Fal_Run(FAL_CMD_BLANKCHECK_WORD, (u16)(base + used * k->sw), 0UL) == FAL_OK)
        {
            break;
        }
    }
    cp->next_free = used;

    /* the rest of the slot area must really be blank (an interrupted erase leaves garbage);
     * if not, force an erase before the next write into this copy */
    for (w = (u16)(used * k->sw); w < (u16)(k->slots * k->sw); w++)
    {
        if (Fal_Run(FAL_CMD_BLANKCHECK_WORD, (u16)(base + w), 0UL) != FAL_OK)
        {
            cp->next_free = k->slots;
            break;
        }
    }

    /* newest valid record = last valid slot before the first blank one */
    for (j = used; j > 0U; j--)
    {
        if (Slot_Check(b, c, (u16)(j - 1U), &cnt) != 0U)
        {
            cp->valid = 1U;
            cp->slot  = (u16)(j - 1U);
            cp->count = cnt;
            break;
        }
    }
}

void Init_Block(u8 b)
{
    const nvm_block_cfg_t *cfg = &nvm_cfg[b];
    nvm_blk_t *k = &g_blk[b];
    u8 src;
    u8 mask;
    u16 i;

    Scan_Copy(b, 0U);
    Scan_Copy(b, 1U);

    k->health = (u8)((k->cp[0].valid ? NVM_COPY_A_OK : 0U)
                        | (k->cp[1].valid ? NVM_COPY_B_OK : 0U));

    if ((k->cp[0].valid != 0U) || (k->cp[1].valid != 0U))
    {
        if (k->cp[0].valid == 0U)       { src = 1U; mask = 0x01U; }
        else if (k->cp[1].valid == 0U)  { src = 0U; mask = 0x02U; }
        else if (k->cp[0].count == k->cp[1].count) { src = 0U; mask = 0x00U; }
        else if (k->cp[0].count >  k->cp[1].count) { src = 0U; mask = 0x02U; }
        else                                       { src = 1U; mask = 0x01U; }

        if (Slot_Load(b, src, k->cp[src].slot) == 0U)       /* newest record -> RAM shadow */
        {
            k->init   = NVM_INIT_FAILED;
            k->result = NVM_REQ_NOT_OK;
            return;
        }
        k->count = k->cp[src].count;
        k->init  = (mask == 0U) ? NVM_INIT_OK : NVM_INIT_RECOVERED;
        if (mask != 0U)
        {
            Job_Start(b, k->count, mask);                   /* repair the other copy */
            Job_Run_Blocking();
        }
    }
    else
    {
        for (i = 0U; i < cfg->size; i++)
        {
            cfg->ram[i] = cfg->defaults[i];
        }
        k->count = 0UL;
        k->init  = NVM_INIT_DEFAULTED;
        k->count++;
        Job_Start(b, k->count, 0x03U);                      /* write defaults to both */
        Job_Run_Blocking();
    }
}

