#ifndef RESET_H
#define RESET_H


#include "io_map.h"
#include "types.h"

#define TRAPR_RESET    RCONbits.TRAPR
#define IOPUWR_RESET   RCONbits.IOPUWR
#define CM_RESET       RCONbits.CM
#define EXTR_RESET     RCONbits.EXTR
#define SWR_RESET      RCONbits.SWR
#define WDTO_RESET     RCONbits.WDTO
#define BOR_RESET      RCONbits.BOR
#define POR_RESET      RCONbits.POR

#define SLEEP_WAKEUP   RCONbits.SLEEP
#define IDLE_WAKEUP    RCONbits.IDLE

typedef enum
{
    POR,          // Power-on Reset
    MCLR,          // Pin Reset
    SWR,          // RESET Instruction
    WDT,          // Watchdog Timer Reset
    BOR,          // Brown-out Reset
    CM,          // Configuration Mismatch Reset
    TRAPR,       // Trap Conflict Reset
    IOPUWR,      // Illegal Opcode Reset
    UWR,         // Uninitialized W Register Reset
    INVALID
}RESET_t ;

typedef enum
{
   IDLE,
   SLEEP
}WAKEUP_t;

// Functions 
extern RESET_t  Read_MCU_RCON_Reg(void);
extern WAKEUP_t Read_Wakeup_Status(void);
#endif


// EOF



