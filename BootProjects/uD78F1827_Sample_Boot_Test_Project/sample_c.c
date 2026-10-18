/*============================================================================*/
/*                                                                            */
/* Project     = Sample program for flash self-programming library            */
/* Module      = sample_c.c                                                   */
/* Version     = V3.00                                                        */
/* Target CPU  = 78K0R/KE3-L( uPD78F1009 / QB-78K0RKE3L-TB )                  */
/* Environment = PM plus(V6.31)                                               */
/*               RA78K0R(V1.31)                                               */
/*               CC78K0R(V2.10)                                               */
/* Date        = Nov 13, 2009                                                 */
/*                                                                            */
/*============================================================================*/
/*                                COPYRIGHT                                   */
/*============================================================================*/
/*                                                                            */
/*     Copyright (C) NEC Electronics Corporation 2008                         */
/*     NEC ELECTRONICS CONFIDENTIAL AND PROPRIETARY                           */
/*     All rights reserved by NEC Electronics Corporation.                    */
/*     This program must be used solely for the purpose for which             */
/*     it was furnished by NEC Electronics Corporation. No part of this       */
/*     program may be reproduced or disclosed to others, in any               */
/*     form, without the prior written permission of NEC Electronics          */
/*     Corporation. Use of copyright notice dose not evidence                 */
/*     publication of the program.                                            */
/*                                                                            */
/*============================================================================*/

/*----------------------------------------------------------------------------*/
/* Expanded functions                                                         */
/*----------------------------------------------------------------------------*/
#pragma sfr                 /* Describes SFR name in C       */
#pragma DI                  /* interrupt disable instruction */
#pragma EI                  /* interrupt enable instruction  */
#pragma NOP                 /* NOP instruction               */


/*----------------------------------------------------------------------------*/
/* section and switch                                                         */
/*----------------------------------------------------------------------------*/
/****************/
/* section name */
/****************/
/* Segment for code portion */
#ifdef __K0R_SMALL__ 
  #pragma section @@CODE  SAM_CODE
#else
  #pragma section @@CODEL SAM_CODE
#endif 

/* Segment for const variable */
#ifdef __K0R_LARGE__
  #pragma section @@CNSTL SAM_CNST
#else
  #pragma section @@CNST  SAM_CNST
#endif


/* Segment for data area */
/*
#ifdef __K0R_LARGE__
  #pragma section @@DATAL SAM_DATA
#else
  #pragma section @@DATA  SAM_DATA
#endif
*/

/***********************************/
/* sample code                     */
/***********************************/
#if 0
#define __QB_78K0RKE3L_TB__
#endif

/*----------------------------------------------------------------------------*/
/* Include files                                                              */
/*----------------------------------------------------------------------------*/
/* Common library header files */
#include <string.h>             /* string header */

/* flash self-programming library header files */
#include "fsl.h"                /* library header file   */
#include "fsl_user.h"           /* user header file */


/*----------------------------------------------------------------------------*/
/* structure                                                                  */
/*----------------------------------------------------------------------------*/
/* write parameter structure */
typedef struct {
    fsl_u08 *pDataBuffer;       /* Data buffer pointer          */
    fsl_u08  startBlock;        /* Number of Block              */
    fsl_u08 *pWriteData;        /* Write data pointer           */
    fsl_u32  writeDataSize;     /* Write data size              */
    fsl_u08  endOption;         /* Otption code                 */
} SAM_FSL_PARM_T;


/*----------------------------------------------------------------------------*/
/* Constant definitions                                                       */
/*----------------------------------------------------------------------------*/
/**************************************/
/* Definition symbol for sample codes */
/**************************************/
/* error code */
#define SAM_ERR_PARM        0x7F        /* parameter error */
#define SAM_ERR_OPT         0x80        /* option error    */

/* personal data */
#define SAM_BLOCK_SIZE      0x400l      /* Block size         */
#define SAM_WORD_SIZE       4           /* Word size          */
#define SAM_DUMYY_DATA      0xFF        /* Dummy data         */

/* sample option code */
#define SAM_OPT_NON         0x00        /* non                          */
#define SAM_OPT_RST         0x01        /* Reset                        */
#define SAM_OPT_CBOOT       0x02        /* Cheanges boot flag           */
#define SAM_OPT_ACT_SWAP    0x03        /* Cheanges boot flag and reset */
#define SAM_OPT_CBOOT_RST   0x04        /* Cheanges boot flag and reset */
#define SAM_OPT_CBOOT_SUP   0x05        /* Cheanges boot flag and Swap, */

/* WDT control macro */
#define SAM_WDT_RESET()     WDTE = 0xAC /* Resets WDT */


/****************/
/* Usual symbol */
/****************/
#ifndef __QB_78K0RKE3L_TB__
#define SAM_DATA_SIZE   0x400           /* Size of dummy data(write data)   */
#define SAM_START_BLOCK 0x20            /* write start block                */
#define SAM_SET_OPTION  SAM_OPT_NON     /* write mode option                */

/* LED control macro */
#define SAM_LED1_INIT()
#define SAM_LED1_ON()
#define SAM_LED1_OFF()
#define SAM_LED2_INIT()
#define SAM_LED2_ON()
#define SAM_LED2_OFF()

/******************************/
/* Symbol for QB-78K0RKE3L-TB */
/******************************/
#else
#define SAM_DATA_SIZE   0x400           /* Size of dummy data(write data)   */
#define SAM_START_BLOCK 0x04            /* write start block                */
#define SAM_SET_OPTION  SAM_OPT_CBOOT   /* write mode option                */

/* LED control macro */
#define SAM_LED1_INIT() P7.6 = 1, PM7.6 = 0
                                        /* LED1 initialization              */
#define SAM_LED1_ON()   P7.6 = 0        /* Switches the light on  of LED1   */
#define SAM_LED1_OFF()  P7.6 = 1        /* Switches the light off of LED1   */

#define SAM_LED2_INIT() P7.7 = 1, PM7.7 = 0
                                        /* LED2 initialization              */
#define SAM_LED2_ON()   P7.7 = 0        /* Switches the light on  of LED2   */
#define SAM_LED2_OFF()  P7.7 = 1        /* Switches the light off of LED2   */

#endif


/*----------------------------------------------------------------------------*/
/* external reference parameter                                               */
/*----------------------------------------------------------------------------*/
/* Data buffer */
extern       fsl_u08 fsl_data_buffer[ FSL_DATA_BUFFER_SIZE ];

/* Dummy data table */
extern const fsl_u08 samWriteData   [ SAM_DATA_SIZE        ];


/*----------------------------------------------------------------------------*/
/* prototype declarations                                                     */
/*----------------------------------------------------------------------------*/
/* Target initialization function */
void    samTargetInit( void );                  /* Target initialization function */
void    samTargetEnd ( void );                  /* Target termination function    */

/* Sample program for flash self-programming function */
fsl_u08 samFslBlockWrite( SAM_FSL_PARM_T * );   /* write function */
fsl_u08 samFslStart     ( fsl_u08 * );          /* flash self-programming start function */
void    samFslEnd       ( void );               /* flash self-programming end function   */

/* ==== ADDED TEST BLOCK BEGIN ================================================ */
/* Uses the demo function samFslBlockWrite() with a NEW 1 KB table (samWriteData  */
/* at the end of this file): an 11-byte pin-toggle routine, then AA 55 AA 55 ...  */
/*                                                                                */
/* SAM_TEST_MODE : 0 = nothing                                                    */
/*                 1 = write table into block SAM_TBLK, read back, compare        */
/*                 2 = write table into every application block, read back        */
/* SAM_RUN_PAYLOAD: after mode 1, run the code that was just written to flash:    */
/*                  1 = blink: C code calls the 3 routines (init/on/off) with a   */
/*                      delay between them (visible on an LED), never returns      */
/*                  2 = free-running fast toggle written in the table (for a scope)*/
#define SAM_TEST_MODE         1
#define SAM_RUN_PAYLOAD       1

/* ---- pin driven by the routines (board XX00160_SCH) -------------------------- */
/* Default P1.2 = MCU pin 32 = net "Wake FIDM" = J3 pin 7 (through R20, 0 ohm).    */
/* LED: J3 pin 7 -> 1 kOhm -> LED anode, LED cathode -> J3 pin 10 (GND).           */
#define SAM_PIN_PORT          1      /* port number n of Pn / PMn (0..14)         */
#define SAM_PIN_BIT           2      /* bit 0..7                                   */

/* uPD78F1827: flash 0000H-17FFFH = blocks 0x00-0x5F (1 KB each)                  */
#define SAM_TBLK              0x20   /* mode 1: 8000H-83FFH                       */
#define SAM_APP_FIRST_BLOCK   0x08   /* 2000H : first block after boot cluster 0  */
#define SAM_APP_LAST_BLOCK    0x5E   /* 17BFFH: block 0x5F = debugger area        */
#define SAM_CODE_FIRST_BLOCK  0x10   /* 4000H : sample code, library, table       */
#define SAM_CODE_LAST_BLOCK   0x14   /* 53FFH   (see sample_c.dr)                 */

/* flash read pointer (CPU reads flash directly) */
#ifndef SAM_FLASH_PTR
#define SAM_FLASH_PTR(a)      ( (const __far fsl_u08 *)(a) )
#endif

fsl_u08  gTestStage;     /* 1 running, 9 finished                                 */
fsl_u08  gTestRet;       /* last return code of samFslBlockWrite, 0 = OK          */
fsl_u08  gTestBlk;       /* block being written                                   */
fsl_u08  gTestCnt;       /* number of blocks written and compared                 */
fsl_u32  gTestBad;       /* total mismatching bytes found on read-back            */
fsl_u32  gTestFirst;     /* address of first mismatch, FFFFFFFFH = none           */

/* write the table at block 'blk' using the demo function */
fsl_u08 samWriteTableAt( fsl_u08 blk )
{
    SAM_FSL_PARM_T typ;

    typ.pDataBuffer   = fsl_data_buffer;
    typ.startBlock    = blk;
    typ.pWriteData    = (fsl_u08 *)samWriteData;
    typ.writeDataSize = SAM_DATA_SIZE;
    typ.endOption     = SAM_OPT_NON;
    return samFslBlockWrite( &typ );
}

/* compare block 'blk' (read directly from flash) with the table */
fsl_u16 samCompareTable( fsl_u08 blk )
{
    const __far fsl_u08 *pf;
    fsl_u32 base;
    fsl_u16 i;
    fsl_u16 bad;

    bad  = 0;
    base = (fsl_u32)blk * SAM_BLOCK_SIZE;
    pf   = SAM_FLASH_PTR( base );
    for( i = 0; i < SAM_DATA_SIZE; i++ )
    {
        if( pf[ i ] != samWriteData[ i ] )
        {
            if( gTestFirst == 0xFFFFFFFFUL ) { gTestFirst = base + i; }
            bad++;
        }
    }
    return bad;
}

void samTestDemoTable( void )
{
    gTestBlk = SAM_TBLK;
    gTestRet = samWriteTableAt( SAM_TBLK );
    gTestBad += samCompareTable( SAM_TBLK );
    gTestCnt = 1;
}

void samTestWholeArea( void )
{
    fsl_u08 blk;

    for( blk = SAM_APP_FIRST_BLOCK; blk <= SAM_APP_LAST_BLOCK; blk++ )
    {
        /* skip the blocks that hold the running code */
        if( ( blk >= SAM_CODE_FIRST_BLOCK ) && ( blk <= SAM_CODE_LAST_BLOCK ) )
        {
            continue;
        }
        SAM_WDT_RESET();
        gTestBlk = blk;
        gTestRet = samWriteTableAt( blk );          /* blank check, erase, write, verify */
        if( gTestRet != FSL_OK ) { break; }
        gTestBad += samCompareTable( blk );         /* read back and compare             */
        gTestCnt++;
    }
}
/* ---- code that was programmed into flash (see the table at the end of file) --- */
typedef void ( *SAM_FN )( void );
#define SAM_PAYLOAD_ADDR   ( (fsl_u16)( (fsl_u16)SAM_TBLK << 10 ) )   /* 8000H for block 0x20 */
#define SAM_FN_INIT        ( (SAM_FN)( SAM_PAYLOAD_ADDR +  0U ) )      /* CLR1 PMn.b ; RET     */
#define SAM_FN_ON          ( (SAM_FN)( SAM_PAYLOAD_ADDR +  4U ) )      /* SET1 Pn.b  ; RET     */
#define SAM_FN_OFF         ( (SAM_FN)( SAM_PAYLOAD_ADDR +  8U ) )      /* CLR1 Pn.b  ; RET     */
#define SAM_FN_FAST        ( (SAM_FN)( SAM_PAYLOAD_ADDR + 12U ) )      /* SET1/CLR1/BR forever */
#define SAM_BLINK_LOOPS    150000UL     /* about 0.2-0.3 s at 16 MHz; adjust if needed */

void samDelay( void )
{
    volatile fsl_u32 d;

    for( d = 0; d < SAM_BLINK_LOOPS; d++ )
    {
        NOP();
    }
    SAM_WDT_RESET();
}

void samRunPayload( void )
{
#if ( SAM_RUN_PAYLOAD == 1 )
    SAM_FN_INIT();                       /* run flash code: pin becomes an output */
    while( 1 )
    {
        SAM_FN_ON();                     /* run flash code: pin = 1 */
        samDelay();
        SAM_FN_OFF();                    /* run flash code: pin = 0 */
        samDelay();
    }
#elif ( SAM_RUN_PAYLOAD == 2 )
    SAM_FN_INIT();
    SAM_FN_FAST();                       /* never returns */
#endif
}
/* ==== ADDED TEST BLOCK END ================================================== */


/*----------------------------------------------------------------------------*/
/* functions                                                                  */
/*----------------------------------------------------------------------------*/
/*============================================================================*/
/* Function name : main                                                       */
/* IN            : void                                                       */
/* OUT           : void                                                       */
/* Description   : Sample program for flash self-programming                  */
/* Date          : 2009/04/17                                                 */
/*============================================================================*/
void main( void )
{
    /* Target initialization */
    samTargetInit();

    gTestStage = 1;
    gTestRet   = 0xEE;                  /* 0xEE = test not run                    */
    gTestCnt   = 0;
    gTestBad   = 0;
    gTestFirst = 0xFFFFFFFFUL;

#if ( SAM_TEST_MODE == 1 )
    samTestDemoTable();
#elif ( SAM_TEST_MODE == 2 )
    samTestWholeArea();
#endif

    gTestStage = 9;                     /* <== put the breakpoint on this line    */

#if ( ( SAM_TEST_MODE == 1 ) && ( SAM_RUN_PAYLOAD != 0 ) )
    samRunPayload();                    /* runs the code just programmed into flash; never returns */
#endif

    /* Target termination */
    samTargetEnd();
    /* Resets WDT   */
    SAM_WDT_RESET();
    /* End loop     */
    while( 1 )
    {
        NOP();
        NOP();
    }
}


/*============================================================================*/
/* Function name : samFslBlockWrite                                           */
/* IN            : SAM_FSL_PARM_T / Write parameter                           */
/* OUT           : fsl_u08        / Return code                               */
/* Description   : Sample program for flash self-programming                  */
/* Date          : 2009/04/17                                                 */
/*============================================================================*/
fsl_u08 samFslBlockWrite( SAM_FSL_PARM_T *pTypFslData  )
{
    fsl_u08 fslRetCode;             /* Return code           */
    fsl_u32 fslWriteBlockNum;       /* Write blcok number    */
    fsl_u16 fslWriteBufSize;        /* Write bffer size      */
    
    /**************************************/
    /* Parameter check and initialization */
    /**************************************/
    if( ( pTypFslData->writeDataSize == 0 ) ||
        ( pTypFslData->pDataBuffer   == 0 ) )
    {
        fslRetCode = SAM_ERR_PARM;  /* Parameter error */
    }
    else {
        /*  */
        /*  */
        fslWriteBufSize = FSL_DATA_BUFFER_SIZE - FSL_DATA_BUFFER_SIZE % SAM_WORD_SIZE;
        
        /* flash self-programming start function */
        fslRetCode = samFslStart( pTypFslData->pDataBuffer );
    }
    
    /*******************/
    /* Writing of data */
    /*******************/
    /* write loop */
    for( fslWriteBlockNum = ( ( pTypFslData->writeDataSize - 1 ) / SAM_BLOCK_SIZE ) + 1 ;
         ( fslWriteBlockNum != 0 ) && ( fslRetCode == FSL_OK )                          ;
         fslWriteBlockNum-- ,pTypFslData->startBlock++                                  )
    {
        /* Performs a blank check of a specified block */
        do {
            fslRetCode = FSL_BlankCheck( pTypFslData->startBlock );
            
            /* When an interrupt error occurred, it carry it out again. */
        } while( fslRetCode == FSL_ERR_INTERRUPTION );
        
        /* Blank check error */
        if( fslRetCode == FSL_ERR_BLANKCHECK )
        {
            /* Erases a specified block */
            do {
                fslRetCode = FSL_Erase( pTypFslData->startBlock );
                
                /* When an interrupt error occurred, it carry it out again. */
            } while( fslRetCode == FSL_ERR_INTERRUPTION );
        }
        
        /* Write program */
        if( fslRetCode == FSL_OK )
        {
            fsl_u32 fslRemDataSize;     /* Remaining data size */
            fsl_u32 fslWriteAddress;    /* Write address       */
            fsl_u32 fslOneWriteSize;    /* Write size          */
            
            /* Calculation of write address */
            fslWriteAddress = pTypFslData->startBlock * SAM_BLOCK_SIZE;
            
            /* When write block remains */
            if( fslWriteBlockNum > 1 )
            {
                /* Write size is set to the size of block */
                fslRemDataSize = SAM_BLOCK_SIZE;
            }
            else {
                /* Data of the remainder is set to the writing size */
                fslRemDataSize = pTypFslData->writeDataSize;
            }
            
            /* Block write Loop */
            for( ; ( fslRemDataSize != 0 ) && ( fslRetCode == FSL_OK ) ;
                   fslWriteAddress += fslWriteBufSize                  )
            {
                /* When the data of the remainder is larger than that of the buffer size */
                if( fslRemDataSize >= fslWriteBufSize )
                {   
                    /* Write size is set of the buffer size */
                    fslOneWriteSize = fslWriteBufSize;
                }
                else {
                    /* Write size is set to the buffer size of remainder */
                    fslOneWriteSize = fslRemDataSize;
                }
                
                /* It copies it onto data Buffer */
                memcpy( pTypFslData->pDataBuffer        ,
                        pTypFslData->pWriteData         ,
                        (unsigned int)fslOneWriteSize   );
                
                /* Size of remainder is calculated */
                fslRemDataSize             -= fslOneWriteSize;
                pTypFslData->writeDataSize -= fslOneWriteSize;
                pTypFslData->pWriteData    += fslOneWriteSize;
                
                /* Write size adjustment          */
                /* It adjusts it every four bytes */
                for( ; ( fslOneWriteSize % SAM_WORD_SIZE ) != 0 ; fslOneWriteSize++ )
                {
                    pTypFslData->pDataBuffer[ fslOneWriteSize ] = SAM_DUMYY_DATA;
                }
                
                /* Writes 1- to 64-word data to a specified address */
                do {
                    fslRetCode = FSL_Write( fslWriteAddress                              ,
                                            (fsl_u08)( fslOneWriteSize / SAM_WORD_SIZE ) );
                    
                    /* When an interrupt error occurred, it carry it out again. */
                } while( fslRetCode == FSL_ERR_INTERRUPTION );
            }
        }
        /* Normal end */
        if( fslRetCode == FSL_OK )
        {
            /* Verifies a specified block */
            do {
                fslRetCode = FSL_IVerify( pTypFslData->startBlock );
                
                /* When an interrupt error occurred, it carry it out again. */
            } while( fslRetCode == FSL_ERR_INTERRUPTION );
        }
    }
    
    /* After executing fsl */
    if( fslRetCode != SAM_ERR_PARM )
    {
        /* Normal end */
        if( fslRetCode == FSL_OK )
        {
            /************************************************************************************************************/
            /* Notes when boot swap is used                                                                             */
            /* Please write the setting necessary for the movement of the object microcomputer of an optional byte etc. */
            /* In area (1000H-1FFFH) before it swaps when you execute the boot swap.                                    */
            /* It becomes impossible the normal movement of the object microcomputer when there is no setting.          */
            /************************************************************************************************************/
            switch( pTypFslData->endOption )
            {
                /* Resets the microcontroller that is being used. */
                case SAM_OPT_RST:
                    /* Resets the microcontroller that is being used. */
                    /* note:If this function is executed when using MINICUBE or IECUBE,a break occurs and processing stops. */
                    /*      Manually execute a reset, because normal operation cannot be performed after the break occurs.  */
                    FSL_ForceReset();
                    break;
                    
                /* Inverts the current boot flag value. */
                case SAM_OPT_CBOOT:
                    /* Inverts the current boot flag value.                                              */
                    /* note:The address of a program arranged in BCL0,BCL1 is changed.                   */
                    /*      Therefore, As for the program how an address is arranged to less than 2000H, */
                    /*      confirmation becomes difficult.                                              */
                    fslRetCode = FSL_InvertBootFlag();
                    
                    break;
                    
                /* Inverts the current boot flag value and executes boot swapping. */
                case SAM_OPT_ACT_SWAP:
                    
                    do {
                        /* Inverts the current boot flag value and executes boot swapping.                    */
                        /* note1:Please arrange the program at the address since 2000H.                       */
                        /*       When the program is arranged at the address of less than 2000H,              */
                        /*       it is not possible to operate normally.                                      */
                        /* note2:The address of a program arranged in BCL0,BCL1 is changed.                   */
                        /*       Therefore, As for the program how an address is arranged to less than 2000H, */
                        /*       confirmation becomes difficult.                                              */
                        /*                                                                                    */
                        /*                                                                                    */
                        fslRetCode = FSL_SwapActiveBootCluster();
                        
                    /* When an interrupt error occurred, it carry it out again. */
                    } while( fslRetCode == FSL_ERR_INTERRUPTION );
                    
                    break;
                    
                /* Inverts the current boot flag value. And resets the microcontroller that is being used. */
                case SAM_OPT_CBOOT_RST:
                    /* Inverts the current boot flag value.                                              */
                    /* note:The address of a program arranged in BCL0,BCL1 is changed.                   */
                    /*      Therefore, As for the program how an address is arranged to less than 2000H, */
                    /*      confirmation becomes difficult.                                              */
                    fslRetCode = FSL_InvertBootFlag();
                    
                    if( fslRetCode == FSL_OK )
                    {
                        /* Resets the microcontroller that is being used. */
                        /* note:If this function is executed when using MINICUBE or IECUBE,a break occurs and processing stops. */
                        /*      Manually execute a reset, because normal operation cannot be performed after the break occurs.  */
                        FSL_ForceReset();
                    }
                    break;
                    
                /* Inverts the current boot flag value. And executes boot swapping and then executes processing from the address registered to the reset vector of area after boot swapping. */
                case SAM_OPT_CBOOT_SUP:
                    /* Inverts the current boot flag value.                                              */
                    /* note:The address of a program arranged in BCL0,BCL1 is changed.                   */
                    /*      Therefore, As for the program how an address is arranged to less than 2000H, */
                    /*      confirmation becomes difficult.                                              */
                    fslRetCode = FSL_InvertBootFlag();
                    
                    if( fslRetCode == FSL_OK )
                    {
                        /* Executes boot swapping and then executes processing from the address registered to the reset vector of area after boot swapping. */
                        /* note:The address of a program arranged in BCL0,BCL1 is changed.                   */
                        /*      Therefore, As for the program how an address is arranged to less than 2000H, */
                        /*      confirmation becomes difficult.                                              */
                        FSL_SwapBootCluster();
                    }
                    break;
                    
                /* etc... */
                default:
                    break;
            }
            
            /* otion error */
            if( fslRetCode != FSL_OK )
            {
                fslRetCode |= SAM_ERR_OPT;
            }
        }
        
        /* flash self-programming end function */
        samFslEnd();
    }
    
    return fslRetCode;
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
    
    /************************/
    /* WDT                  */
    /************************/
    SAM_WDT_RESET();        /* Resets WDT */
    
    /**********************/
    /* LED initialization */
    /**********************/
    SAM_LED1_INIT();        /* LED1 initialization           */
    SAM_LED2_INIT();        /* LED2 initialization           */
    SAM_LED1_ON();          /* Switches the light on of LED1 */
    SAM_LED2_ON();          /* Switches the light on of LED2 */
    
    EI();                   /* Enabled interrupt */
}

/*============================================================================*/
/* Function name : samTargetEnd                                               */
/* IN            : void                                                       */
/* OUT           : void                                                       */
/* Description   : Sample program for flash self-programming                  */
/* Date          : 2009/02/03                                                 */
/*============================================================================*/
void samTargetEnd( void )
{
    SAM_LED1_OFF();         /* Switches the light off of LED1 */
    SAM_LED2_OFF();         /* Switches the light off of LED2 */
}

/*============================================================================*/
/* Function name : samFslStart                                                */
/* IN            : fsl_u08* /  Buffer pointer                                 */
/* OUT           : fsl_u08  /  Return code                                    */
/* Description   : Sample program for flash self-programming                  */
/* Date          : 2009/02/03                                                 */
/*============================================================================*/
fsl_u08 samFslStart( fsl_u08 *pDataBuffer )
{
    fsl_u08 fslRetCode;
    
    /* Declares the start of flash self programming */
    FSL_Open();
    
    /* Initializes the flash self programming environment */
    fslRetCode = FSL_Init( pDataBuffer );
    
    /* Interrupt error */
    while( fslRetCode == FSL_ERR_INTERRUPTION )
    {
        /* Continues initialization after interrupt */
        fslRetCode = FSL_Init_cont( pDataBuffer );
    }
    
    /* Normal end */
    if( fslRetCode == FSL_OK )
    {
        
        
        /* Checks the voltage level of the FLMD0 pin */
        fslRetCode = FSL_ModeCheck();
        
    }
    
    return fslRetCode;
}


/*============================================================================*/
/* Function name : samFslEnd                                                  */
/* IN            : void                                                       */
/* OUT           : void                                                       */
/* Description   : Sample program for flash self-programming                  */
/* Date          : 2009/02/03                                                 */
/*============================================================================*/
void samFslEnd( void )
{
    /* Declares the end of flash self programming */
    FSL_Close();
}


/************************************************************************************/
/*                                                                                  */
/*  LED lighting program. ( QB-78K0RKE3L-TB only )                                  */
/*                                                                                  */
/*    Two LED lights alternately.                                                   */
/*    SW1 is pushed, Boot-swap is done.                                             */
/*                                                                                  */
/*    EPlease set it as follows when you use the program on QB-78K0RKE3L-TB.       */
/*      EThere's switch from "if 0" to "if 1" in the 70th line from the top.       */
/*        The LED lighting program starts when boot swapping with this program      */
/*        written in boot cluster 1.                                                */
/*                                                                                  */
/*                                                                                  */
/*                                                                                  */
/*                                                                                  */
/*                                                                                  */
/*                                                                                  */
/*==================================================================================*/
/*                                                                                  */
/*      Please do not reset it by the debugger at this time.                        */
/*      It doesn't operate normally.                                                */
/*                                                                                  */
/*==================================================================================*/
/*                                                                                  */
/*      Blinking LED changes if SW1 is pushed and after a few seconds,              */
/*      reset is generated. Then, the boot swap is executed and it returns to       */
/*      former program.                                                             */
/*                                                                                  */
/*      Note:This execution program is a sample made to confirm the boot-swap       */
/*           simply. It is likely not to operate normally according to the          */
/*           setting of the project and the connection of QB-78K0RKE3L-TB.          */
/*                                                                                  */
/************************************************************************************/
const  fsl_u08 samWriteData[ SAM_DATA_SIZE ] = 
{
    /* --- 3 small routines, position independent, each ends with RET (D7) --- */
    0x71, ( 0x0B | ( SAM_PIN_BIT << 4 ) ), ( 0x20 + SAM_PIN_PORT ), 0xD7,   /* 00: CLR1 PMn.b ; RET  pin = output */
    0x71, ( 0x0A | ( SAM_PIN_BIT << 4 ) ),   SAM_PIN_PORT,           0xD7,   /* 04: SET1 Pn.b  ; RET  pin = 1      */
    0x71, ( 0x0B | ( SAM_PIN_BIT << 4 ) ),   SAM_PIN_PORT,           0xD7,   /* 08: CLR1 Pn.b  ; RET  pin = 0      */
    /* --- free-running toggle, never returns --- */
    0x71, ( 0x0A | ( SAM_PIN_BIT << 4 ) ),   SAM_PIN_PORT,                   /* 0C: SET1 Pn.b  <-+                 */
    0x71, ( 0x0B | ( SAM_PIN_BIT << 4 ) ),   SAM_PIN_PORT,                   /* 0F: CLR1 Pn.b    |                 */
    0xEF, 0xF8,                                                              /* 12: BR $-8 ------+                 */
    /* --- pattern from offset 0x14: AA 55 AA 55 ... (every bit toggles) --- */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 014 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 024 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 034 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 044 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 054 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 064 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 074 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 084 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 094 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 0A4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 0B4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 0C4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 0D4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 0E4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 0F4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 104 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 114 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 124 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 134 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 144 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 154 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 164 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 174 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 184 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 194 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 1A4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 1B4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 1C4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 1D4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 1E4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 1F4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 204 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 214 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 224 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 234 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 244 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 254 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 264 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 274 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 284 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 294 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 2A4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 2B4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 2C4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 2D4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 2E4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 2F4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 304 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 314 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 324 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 334 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 344 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 354 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 364 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 374 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 384 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 394 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 3A4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 3B4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 3C4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 3D4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55,   /* 3E4 */
    0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55   /* 3F4 */
};

