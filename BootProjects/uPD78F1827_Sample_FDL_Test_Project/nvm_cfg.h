/* nvm_cfg.h - configuration of the NVM module (2 logical blocks, 2 copies each)
 *
 * Built on the FAL/FDL port in fdl_ca.h (32-bit word access, 8 x 2 KB data-flash blocks).
 * Change the sizes / FDL block assignment here and in nvm_cfg.c only; nvm.c needs no edit.
 */
#ifndef NVM_CFG_H
#define NVM_CFG_H


#include "system_def.h"

/* ---- logical blocks ------------------------------------------------------------ */
#define NVM_BLK_SLOW        0U      /* rarely written  (calibration, config ...)    */
#define NVM_BLK_FAST        1U      /* frequently written (counters, logs ...)      */
#define NVM_BLK_COUNT       2U

/* user data size of each block in bytes (any size, stored padded to 4 bytes) */
#define NVM_SLOW_DATA_SIZE  64U
#define NVM_FAST_DATA_SIZE  32U

#define NVM_MAX_DATA_SIZE   ((NVM_SLOW_DATA_SIZE > NVM_FAST_DATA_SIZE) ? NVM_SLOW_DATA_SIZE : NVM_FAST_DATA_SIZE)

/* ---- FDL geometry (from fdl_descriptor_raw.c: 8 blocks x 2048 bytes) ----------- */
#define NVM_FDL_BLOCK_COUNT         8U
#define NVM_WORDS_PER_ERASE_BLOCK   512U

/* ---- slot layout: [count][id|rev|len] [data ...] [crc word][valid word] --------- */
#define NVM_SLOT_WORDS(size)        (2U + (((size) + 3U) / 4U) + 2U)
#define NVM_MAX_SLOT_WORDS          NVM_SLOT_WORDS(NVM_MAX_DATA_SIZE)

/* ---- behaviour ------------------------------------------------------------------ */
#define NVM_WRITE_RETRIES   2U      /* extra slots tried after a write/verify error */

/* Watchdog restart*/
#define NVM_KICK_WDT()      (WDTE = 0xACU)

/* ---- per-block configuration ---------------------------------------------------- */
typedef struct {
    u8         id;             /* block ID stored in every header              */
    u8         rev;            /* layout revision; mismatch => record ignored  */
    u16         size;           /* user data bytes                              */
    u8         fdl_block[2];   /* FDL erase block used by copy A and copy B    */
    const u8  *defaults;       /* used when no valid copy exists               */
    u8        *ram;            /* RAM mirror                                   */
} nvm_block_cfg_t;

extern const nvm_block_cfg_t nvm_cfg[NVM_BLK_COUNT];

#endif
