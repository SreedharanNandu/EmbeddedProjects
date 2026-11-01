/* nvm_cfg.c - block table. Edit defaults / FDL block assignment here. */
#include "nvm.h"

/* RAM mirrors (the application reads/writes these through the NVM API) */
static u8 nvm_ram_slow[NVM_SLOW_DATA_SIZE];
static u8 nvm_ram_fast[NVM_FAST_DATA_SIZE];

/* Default contents used when a block has no valid copy (first power-up, both copies lost).
 * TODO: put your real default values here. */
static const u8 nvm_def_slow[NVM_SLOW_DATA_SIZE] = { 0 };
static const u8 nvm_def_fast[NVM_FAST_DATA_SIZE] = { 0 };

/* Every copy owns one whole 2 KB erase block and is filled slot by slot (append log),
 * so one erase is needed only every (512 / slot words) writes.
 * Copy A and copy B of one block MUST be different FDL blocks, and no FDL block may be
 * shared between logical blocks (checked in NVM_Init).
 * Blocks 4..7 are free: give the FAST block more erase blocks later if you need more endurance. */
const nvm_block_cfg_t nvm_cfg[NVM_BLK_COUNT] = {
    /* id    rev  size                 copyA copyB  defaults       ram           */
    { 0xA1U, 1U,  NVM_SLOW_DATA_SIZE,  { 0U,   1U }, nvm_def_slow, nvm_ram_slow },
    { 0xB2U, 1U,  NVM_FAST_DATA_SIZE,  { 2U,   3U }, nvm_def_fast, nvm_ram_fast }
};
