/* main_nvm_test.c - hardware test / demo of the NVM module (nvm.c, nvm_cfg.c) for CS+ / CA78K0R.
 * Replace main.c with this file (it carries samTargetInit from your FDL test) and add
 * nvm.c, nvm_cfg.c and your existing FDL files (fdl_wrap.c, fdl_glue.asm, fdl_blob, descriptor).
 *
 * WARNING: uses data-flash blocks 0..3 (slow = 0/1, fast = 2/3). Use a development board.
 *
 * What it does
 *  1. NVM_Init(). First run on erased flash: both blocks report NVM_INIT_DEFAULTED.
 *  2. Increments a boot counter stored in the FAST block (background write) and writes a
 *     pattern into the SLOW block (immediate write, once).
 *  3. Main loop: kicks the watchdog and calls NVM_MainFunction().
 *
 */
#pragma sfr
#pragma DI
#pragma EI
#pragma NOP

#include "nvm.h"

#define WDT_KICK()   (WDTE = 0xACU)

typedef struct 
{
    u8 spare[NVM_FAST_DATA_SIZE - 4U];
} fast_data_t;

typedef struct
{
   u8 r_init;
   u8 init_slow;
   u8 init_fast;
   u8 health_slow;
   u8 health_fast;
   u8 err_slow;
   u8 err_fast;
}nvm_status_T;

nvm_status_T nvm_status;

static fast_data_t fast;
static u8     slow_buf[NVM_SLOW_DATA_SIZE];
static u8     slow_rd[NVM_SLOW_DATA_SIZE];

void samTargetInit( void );

void main(void)
{
    u16 i;

    samTargetInit();

    nvm_status.r_init    = NVM_Init();
    nvm_status.init_slow = NVM_GetInitStatus(NVM_BLK_SLOW);
    nvm_status.init_fast = NVM_GetInitStatus(NVM_BLK_FAST);

    /* boot counter in the FAST block, written in the background */
    NVM_ReadBlock(NVM_BLK_FAST, &fast);
    NVM_WriteBlock(NVM_BLK_FAST, &fast);

    /* SLOW block: write a pattern once, immediately (blocking), then read back */
    for (i = 0U; i < NVM_SLOW_DATA_SIZE; i++)
    {
        slow_buf[i] = (u8)(i + 0x20U);
    }
    NVM_WriteBlock(NVM_BLK_SLOW, slow_buf);
    NVM_ReadBlock(NVM_BLK_SLOW, slow_rd);

    while (NVM_IsBusy() != 0U)          /* let the background write finish */
    {
        WDT_KICK();
        NVM_MainFunction();
    }
    nvm_status.health_slow = NVM_GetCopyHealth(NVM_BLK_SLOW);
    nvm_status.health_fast = NVM_GetCopyHealth(NVM_BLK_FAST);
    nvm_status.err_slow    = NVM_GetErrorStatus(NVM_BLK_SLOW);
    nvm_status.err_fast    = NVM_GetErrorStatus(NVM_BLK_FAST);

    while (1)
    {
        WDT_KICK();
        NVM_MainFunction();
    }
}

/*============================================================================*/
/* Function name : samTargetInit                                              */
/* IN            : void                                                       */
/* OUT           : void                                                       */
/* Description   : Sample program for flash self-programming                  */
/* Date          : 2009/02/03                                                 */
/*============================================================================*/
void samTargetInit( void )
{
    DI();                   /* Disabled interrupt */
    
    /******************************************************/
    /* System clock control register (CKC) initialization */
    /******************************************************/
    /* System clock control register */
    CKC = 0x00;             /* Selects the internal high-speed oscillation clock */
                            
    
    /* Operation speed mode control register */
    OSMC =0x01;             /* Operates at a frequency higher than 10 MHz */
    
    
    /************************/
    /* Changes of frequency */
    /************************/
    /* Clock operation mode control register (CMC)  */
    CMC = 0x41;             /* Selects X1 oscillation mode (10 MHz < fMX < 20 MHz) */
    
    /* Oscillation stabilization time select register (OSTS) */
    OSTS = 0x07;            /* Sets stabilization time (07H) */
    
    /* High-speed system clock operation control    */
    GUARD |= 0x01;          /* Fx3: enable writes to CSC (added) */
    MSTOP = 0;              /* X1 oscillator operating (X1 clock starts) */
    
    /* Oscillation Stabilization Time Counter Status Register (OSTC) */
    while( ( OSTC & 0xFF ) != 0xFF );    /* Waits the oscillation stability time for X1 clock */
    
    /* Main system clock operation control (MCM0)   */
    MCM0 = 1;               /* Selects high-Speed system clock   */
    while( MCS == 0 );
    GUARD &= (unsigned char)~0x01;   /* lock CSC again (added) */      /* Waits the High-speed system clock */
    
    
    /*************************/
    /* Low-voltage detection */
    /*************************/
    LVIM = 0x00;            /* Clears Low-voltage detection register              */
    LVIS = 0x00;            /* Clears Low-voltage detection level select register */
    

    
    EI();                   /* Enabled interrupt */
}
