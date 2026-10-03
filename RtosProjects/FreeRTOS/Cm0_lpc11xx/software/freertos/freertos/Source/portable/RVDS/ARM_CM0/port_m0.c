/*
    FreeRTOS V7.3.0 - Copyright (C) 2012 Real Time Engineers Ltd.

    FEATURES AND PORTS ARE ADDED TO FREERTOS ALL THE TIME.  PLEASE VISIT 
    http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest versions, license 
    and contact details.  
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM0 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL      ( ( volatile unsigned long *) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD      ( ( volatile unsigned long *) 0xe000e014 )
#define portNVIC_INT_CTRL         ( ( volatile unsigned long *) 0xe000ed04 )
#define portNVIC_SYSPRI2         ( ( volatile unsigned long *) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK      0x00000004
#define portNVIC_SYSTICK_INT      0x00000002
#define portNVIC_SYSTICK_ENABLE      0x00000001
#define portNVIC_PENDSVSET         0x10000000
#define portMIN_INTERRUPT_PRIORITY   ( 255UL )
#define portNVIC_PENDSV_PRI         ( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI      ( portMIN_INTERRUPT_PRIORITY << 24UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR         ( 0x01000000 )

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static unsigned portBASE_TYPE uxCriticalNesting = 0xaaaaaaaa;

#if configUSE_CUSTOM_TICK == 0
/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void );
#endif

#ifdef __GNUC__
/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ (( naked ));
void xPortSysTickHandler( void );
void vPortSVCHandler( void ) __attribute__ (( naked ));
/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void vPortStartFirstTask( void ) __attribute__ (( naked ));
#endif

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
   /* Simulate the stack frame as it would be created by a context switch
   interrupt. */
   pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
   *pxTopOfStack = portINITIAL_XPSR;   /* xPSR */
   pxTopOfStack--;
   *pxTopOfStack = ( portSTACK_TYPE ) pxCode;   /* PC */
   pxTopOfStack -= 6;   /* LR, R12, R3..R1 */
   *pxTopOfStack = ( portSTACK_TYPE ) pvParameters;   /* R0 */
   pxTopOfStack -= 8; /* R11..R4. */

   return pxTopOfStack;
}
/*-----------------------------------------------------------*/
#if defined ( __CC_ARM   )
/*
[ TASK 1 TCB Structure in RAM ]
+--------------------------------------------------------+
¦ pxTopOfStack = 0x3FFF50 -----------------------------+ ¦
+------------------------------------------------------+-+
                                                       ¦
  +----------------------------------------------------+
  V
[ TASK 1 STACK RAM AREA ]
Memory Address | Stored Register  | Who Pushed It?     | Pointer Locations
---------------+------------------+--------------------+---------------------------------------
0x3FFF8C       | xPSR             | +                  | 
0x3FFF88       | PC (vLEDTask1)   | ¦                  | 
0x3FFF84       | LR               | ¦                  | 
0x3FFF80       | R12              | +- AUTOMATICALLY   | 
0x3FFF7C       | R3               | ¦  pushed by the   | 
0x3FFF78       | R2               | ¦  ARM Hardware    | 
0x3FFF74       | R1               | ¦                  | 
0x3FFF70       | R0               | +                  | <-- Hardware PSP stopped here.          here <-BX R1 happens 
---------------+------------------+--------------------+---------------------------------------  here <--MSR PSP,R0 i.e PSP = R0
0x3FFF6C       | R11              | +                  | //first R0 is used to store R11 to R8 , then R0 is subtracted by 32 becuase 
0x3FFF68       | R10              | ¦                  | //ldmia stores R toR7 and increments to next 16bytes so R0 is now at 0x3fff70
0x3FFF64       | R9               | +- MANUALLY        | //so now 0x3fff70 -32 is 0x3fff50 ,now store R4 to R7
0x3FFF60       | R8               | ¦  pushed by our   | 
0x3FFF5C       | R7               | ¦  Assembly via    | 
0x3FFF58       | R6               | ¦  stmia           | 
0x3FFF54       | R5               | ¦                  |     //they use R0 to copy the ToS pointer until contents of R4-R11 is copied
0x3FFF50       | R4               | +                  | <-- pxTopOfStack , this will never change (dont confuse with PSP) (this is given by malloc when task gets created)

*/__asm void vPortSVCHandler( void )
{
   extern pxCurrentTCB;

   PRESERVE8

   ldr   r3, =pxCurrentTCB      /* Restore the context(R3 pointer) */
   ldr r1, [r3]            /* Get the pxCurrentTCB address. */
   ldr r0, [r1]            /* The first item in pxCurrentTCB is the task top of stack(pxTopOfStack) */
   adds r0, r0, #16         /* Move to the high registers.i.e skip all 16bytes(4 words) */
   ldmia r0!, {r4-r7}  //copy R8 - R11 of task RAM stack to CPU R4 - R7 regs
   /*
     pxTopOfStack --> [  SAVED TASK STACK FRAME IN RAM  ]   <---dont get confused these are task RAM stack (not CPU stack)
                      +----------------------------------+
     Address (r0) --> ¦ Low Registers: R4, R5, R6, R7    ¦
                      +----------------------------------¦
       (r0 + #16) --> ¦ High Registers: R8, R9, R10, R11 ¦
                      +----------------------------------¦
         r0-> PSP --> ¦ Hardware Auto-Pop Area:          ¦
                      ¦ R0, R1, R2, R3, R12,             ¦
                      ¦ LR, PC (Task Code), xPSR         ¦
                      +----------------------------------+

     and move those RAM stack R4-R7 to CPU R8-R11
   */
   
   mov r8,  r4
   mov r9,  r5
   mov r10, r6
   mov r11, r7
   
   msr psp, r0               /* PSP = R0 (High Registers location+4 becuase of ! in ldmia before) Remember the new top of stack for the task. just a copy of R0 in PSP*/
   subs r0, r0, #32         /* Go back for the low registers that are not automatically restored. */
   ldmia r0!, {r4-r7}         /* Pop low registers.  */
    /*  and move those task RAM stack R4-R7 to CPU R4-R7  */

   mov r1, r14     //R14 holds the EXC_RETURN
   movs r0, #0xd  // see here the CPU needs to be told manually ,so add last 4 bits to make 0xFFFFFFFD (return to thread mode and use PSP)
   orrs r1, r0    //bitwiseOR to add 'd' and move 0xFFFFFFFD to R1
   bx r1  //on this the CPU will reload Stackpointer pointed my PSP which is remember R0 saved before (Auto pop Area)
}


#elif defined   (  __GNUC__  )
void vPortSVCHandler( void )
{
   __asm volatile (
               "   ldr   r3, pxCurrentTCBConst2      \n" /* Restore the context. */
               "   ldr r1, [r3]               \n" /* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
               "   ldr r0, [r1]               \n" /* The first item in pxCurrentTCB is the task top of stack. */
               "   add r0, r0, #16               \n" /* Move to the high registers. */
               "   ldmia r0!, {r4-r7}            \n" /* Pop the high registers. */
               "    mov r8, r4                  \n"
               "    mov r9, r5                  \n"
               "    mov r10, r6                  \n"
               "    mov r11, r7                  \n"
               "                           \n"
               "   msr psp, r0                  \n" /* Remember the new top of stack for the task. */
               "                           \n"
               "   sub r0, r0, #32               \n" /* Go back for the low registers that are not automatically restored. */
               "    ldmia r0!, {r4-r7}              \n" /* Pop low registers.  */
               "   mov r1, r14                  \n" /* OR R14 with 0x0d. */
               "   movs r0, #0x0d               \n"
               "   orr r1, r0                  \n"
               "   bx r1                     \n"
               "                           \n"
               "   .align 2                  \n"
               "pxCurrentTCBConst2: .word pxCurrentTCB   \n"
            );
}
#else
#error vPortSVCHandler not defined for this compiler!
#endif
/*-----------------------------------------------------------*/
/*
[ MAIN STACK RAM AREA (MSP) ]
Address    | Stored Contents / Variables               | Current CPU Pointer State
-----------+-------------------------------------------+------------------------------------------------
0x20008000 | [Stack Top Baseline Anchor]               | 
0x20007FFC | main() local variables                    |
0x20007FF8 | xTaskCreate(Task 1) allocations           |
0x20007FF4 | xTaskCreate(Task 2) allocations           |
0x20007FF0 | vTaskStartScheduler() temporary frames    | <-- MSP is actively here during main() execution
*/
#if defined ( __CC_ARM   )
__asm void vPortStartFirstTask( void )
{
   PRESERVE8  /*make SP 8 byte aligned*/

   movs r0, #0x00       /* Locate the top of stack. *///MSP is sitting at address 0
   ldr r0, [r0]        /* Read the contents of memory pointed by R0 into R0*/
   msr msp, r0         /* MSP = R0 Set the msp back to the start of the stack. */
   cpsie i            /* Globally enable interrupts. */
   svc 0            /* System call to start first task. */
   nop                 /* Triggers vPortSVCHandler */
   //from here the interrupt uses MSP
/* Note:- ISB and DSB instructions not needed . The Cortex-M0 features a basic, highly 
   predictable 3-stage pipeline. It does not feature out-of-order execution, it does not 
   perform speculative data prefetching, and it does not have an advanced data cache
*/
}

#elif defined   (  __GNUC__  )
void vPortStartFirstTask( void )
{
   __asm volatile(
               " movs r0, #0x00    \n" /* Locate the top of stack. */
               " ldr r0, [r0]       \n"
               " msr msp, r0      \n" /* Set the msp back to the start of the stack. */
               " cpsie i         \n" /* Globally enable interrupts. */
               " svc 0            \n" /* System call to start first task. */
               " nop            \n"
            );
}
#endif
/*-----------------------------------------------------------*/

/*
 * See header file for description.
   [ HARDWARE RESET ]
            ¦
            +-- 1. CPU defaults to using the MSP register.
            +-- 2. Silicon reads address 0x00000000 --? Sets MSP = 0x20008000
            +-- 3. Silicon reads address 0x00000004 --? Sets PC = Reset_Handler
            ¦
            v Jumps to
  [ C Startup / main() ]  <---- RUNNING ON MSP!
            ¦
            +-- 1. Initializes clocks and peripherals.
            +-- 2. Calls xTaskCreate(Task 1) --> Creates fake R4-xPSR block in RAM (TCB1 = 0x3FFF50)
            +-- 3. Calls xTaskCreate(Task 2) --> Creates fake R4-xPSR block in RAM (TCB2 = 0x3FFE80)
            +-- 4. Calls vTaskStartScheduler()
            ¦
            v Calls
  [ vPortStartFirstTask ] <---- RUNNING ON MSP!
            ¦
            +-- 1. Re-reads address 0x00000000.
            +-- 2. Rewinds the physical MSP back to 0x20008000 (Cleans out main() history to save RAM).
            +-- 3. Fires the "svc 0" instruction.
            ¦
            v Triggers Hardware Exception Routing
  [ vPortSVCHandler ]     <---- RUNNING ON MSP (Interrupt State)!
            ¦
            +-- 1. Looks up Task 1's TCB bookmark (0x3FFF50).
            +-- 2. Restores fake registers R4-R11.
            +-- 3. Executes: msr psp, r0 --? INJECTS Task 1's stack (0x3FFF70) into physical PSP register!
            +-- 4. Switches CPU CONTROL register to force task mode to use PSP stack.
            +-- 5. Executes: bx r14
            ¦
            v Exception Exit Takeover
  [ Task 1 Running Live ] <---- RUNNING ON PSP (PSP = 0x3FFF90)!
            ¦
            +-- 1. Hardware auto-pops Task 1's fake R0-xPSR block off the stack.
            +-- 2. Task 1 runs loops, calls Board_LED_Set(), then hits vTaskDelay() to sleep.
            ¦
            V Triggers Scheduler Context Switch
  [ xPortPendSVHandler ]  <---- SWAPS MODE BACK TO MSP (Interrupt State)!
            ¦
            +-- 1. Saves Task 1's state down to 0x3FFF4C and bookmarks it inside TCB 1.
            +-- 2. C scheduler shifts global pointer variable: pxCurrentTCB = &xTask2TCB.
            +-- 3. Reads Task 2's bookmark (0x3FFE80).
            +-- 4. Executes: msr psp, r0 --? INJECTS Task 2's stack (0x3FFE90) into physical PSP register!
            +-- 5. Executes: bx r3
            ¦
            V Exception Exit Takeover
  [ Task 2 Running Live ] <---- RUNNING ON PSP (PSP = 0x3FFEBC)!

*/
portBASE_TYPE xPortStartScheduler( void )
{
   /* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
   *(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;//pensSV priotiry as 3
   *(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;//systick priority as 3

#if configUSE_CUSTOM_TICK == 0
   /* Start the timer that generates the tick ISR.  Interrupts are disabled
   here already. */
   prvSetupTimerInterrupt();//configure systick for 1ms interrupt
#endif   

   /* Initialise the critical nesting count ready for the first task. */
   uxCriticalNesting = 0;

   /* Start the first task. */
   vPortStartFirstTask();

   /* Should not get here! */
   return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
  /* It is unlikely that the CM0 port will require this function as there
    is nothing to return to.  */
}
/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
   /* Set a PendSV to request a context switch. */
   *( portNVIC_INT_CTRL ) = portNVIC_PENDSVSET;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    portDISABLE_INTERRUPTS();
    uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
    uxCriticalNesting--;
    if( uxCriticalNesting == 0 )
    {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

#if defined ( __CC_ARM   )
/*
[ TASK 1 TCB Structure in RAM ]
+--------------------------------------------------------+
¦ pxTopOfStack = 0x3FFF50 -----------------------------+ ¦
+------------------------------------------------------+-+
                                                       ¦
  +----------------------------------------------------+
  V
[ TASK 1 STACK RAM AREA ]
Memory Address | Stored Register  | Who Pushed It?     | Pointer Locations
---------------+------------------+--------------------+---------------------------------------
0x3FFF8C       | xPSR             | +                  | 
0x3FFF88       | PC (vLEDTask1)   | ¦                  | 
0x3FFF84       | LR               | ¦                  | 
0x3FFF80       | R12              | +- AUTOMATICALLY   | 
0x3FFF7C       | R3               | ¦  pushed by the   | 
0x3FFF78       | R2               | ¦  ARM Hardware    | 
0x3FFF74       | R1               | ¦                  | 
0x3FFF70       | R0               | +                  | <-- Hardware PSP stopped here.
---------------+------------------+--------------------+---------------------------------------
0x3FFF6C       | R11              | +                  | 
0x3FFF68       | R10              | ¦                  | 
0x3FFF64       | R9               | +- MANUALLY        | 
0x3FFF60       | R8               | ¦  pushed by our   | 
0x3FFF5C       | R7               | ¦  Assembly via    | 
0x3FFF58       | R6               | ¦  stmia           | 
0x3FFF54       | R5               | ¦                  | 
0x3FFF50       | R4               | +                  | <-- pxTopOfStack bookmarks THIS line!(this is given by malloc when task gets created)

[ IDLE TASK TCB Structure in RAM ]
+--------------------------------------------------------+
¦ pxTopOfStack = 0x3FFE20 -----------------------------+ ¦
+------------------------------------------------------+-+
                                                       ¦
  +----------------------------------------------------+
  V
[ IDLE TASK STACK RAM AREA ]
Memory Address | Stored Register  | Who Restores It?   | Pointer Tracking
---------------+------------------+--------------------+---------------------------------------
0x3FFE5C       | xPSR             | +                  | 
0x3FFE58       | PC (prvIdleTask) | ¦                  | 
0x3FFE54       | LR               | ¦                  | 
0x3FFE50       | R12              | +- AUTOMATICALLY   | 
0x3FFE4C       | R3               | ¦  restored by the | 
0x3FFE48       | R2               | ¦  ARM hardware    | 
0x3FFE44       | R1               | ¦  during "bx r3"  | 
0x3FFE40       | R0               | +                  | <-- [Line 11] msr psp, r0 sets PSP here!
---------------+------------------+--------------------+---------------------------------------
0x3FFE3C       | R11              | +                  | 
0x3FFE38       | R10              | ¦  MANUALLY        | 
0x3FFE34       | R9               | +- unpacked by our | <-- [Line 4] adds r0, #16 jumps here first
0x3FFE30       | R8               | ¦  Assembly lines  | 
---------------+------------------+--------------------+---------------------------------------
0x3FFE2C       | R7               | ¦                  | 
0x3FFE28       | R6               | ¦                  | 
0x3FFE24       | R5               | ¦                  | 
0x3FFE20       | R4               | +                  | <-- [Line 3] ldr r0, [r1] starts here!

[ TASK2 STACK RAM AREA ]

Memory Address | Stored Value / Register | Block Type      | Tracking Pointers & Actions
---------------+-------------------------+-----------------+---------------------------------------------
0x3FFEBC       | Mock xPSR (0x01000000)  | +               | 
0x3FFEBA       | PC (vLEDTask2)          | ¦               | 
0x3FFEB6       | Mock LR (0x00000000)    | ¦               | 
0x3FFEB2       | Mock R12                | +- HARDWARE     |  
0x3FFEAE       | Mock R3                 | ¦  BLOCK        |    
0x3FFEAA       | Mock R2                 | ¦               |     The ARM hardware will auto-pop them
0x3FFE66       | Mock R1                 | ¦               |     during the final "bx r3" exit.
0x3FFE90       | pvParameters (Mock R0)  | +               | <-- PSP location , [Step 3] R0 stops here after ldmia (!).
---------------+-------------------------+-----------------+---------------------------------------------
0x3FFE8C       | Mock R11                | +               | 
0x3FFE88       | Mock R10                | ¦               | 
0x3FFE84       | Mock R9                 | +- MANUAL       | 
0x3FFE80       | Mock R8                 | ¦  HIGH REGS    | <-- [Step 2] "adds r0, #16" jumps R0 straight here!
---------------+-------------------------+-----------------+---------------------------------------------
0x3FFE7C       | Mock R7                 | +               | 
0x3FFE78       | Mock R6                 | ¦  MANUAL       | 
0x3FFE74       | Mock R5                 | +- LOW REGS     | 
0x3FFE70       | Mock R4                 | +               | <-- pxTopOfStack, [Step 1] "ldr r0, [r1]" starts R0 exactly here!


*/__asm void xPortPendSVHandler( void )
{
   extern pxCurrentTCB;
   extern vTaskSwitchContext;

   PRESERVE8

   //R0 = PSP (after all defaul regs pushed by CORE)
   mrs r0, psp                     /* read PSP and store in R0, becuase  PSP already contains xPSR,PC,R12,R3-R0,we need to store R11-R4 also pointed by PSP ,so that we store all together*/
   ldr   r3, =pxCurrentTCB         /* Get the location of the current TCB. */
   ldr   r2, [r3]                    /* read at the location and store the data read in R2, say for ex:- task1 's 0x4000 i.r R2 = 4000*/
   //make way for 8 words
   subs r0, r0, #32            /* Make space for the remaining low registers. R0 = R0-32*/
    //store the PSP's 8 word offset address into task's pxTopOfStack
   str r0, [r2]               /* Save the new top of stack,so R2(which is 0x4000) is read so pxTopOfStack = R0 (which is PSP - 32)*/
    //now store R4 to R7 into R0 which is same as  pxTopOfStack
   stmia r0!, {r4-r7}            /* Store the low registers that are not saved automatically. */
    //now copy R8 to R11 into R4-R7 for rest store
   mov r4, r8                  /* Store the high registers. */
   mov r5, r9
   mov r6, r10
   mov r7, r11
    //store R8 to R11 as well
   stmia r0!, {r4-r7}
   //very important that R3 and R14 also must be saved in
    //NOTE that this is in handler mode and not thread mode so MSP is used to store R3 and R14(LR)    
   push {r3, r14} //remember that R3 = pxCurrentTCB
   //this is the critical section, like Do not disturb (DND)
   cpsid i   //Do not disturb,no more interruts allowed
   bl vTaskSwitchContext
   cpsie i   //interrupts allowed
    //R2 = R3 = new pxCurrentTCB (not old one, its very important because we switched context above) ,R3 = LR(R14)
   pop {r2, r3}               /* LR goes in r3. r2 now holds tcb pointer. */
   ldr r1, [r2]                    /* Read from pxCurrentTCB */
   ldr r0, [r1]               /* The first item in pxCurrentTCB is the task top of stack. */
   adds r0, r0, #16            /* Move to the high registers. */
   ldmia r0!, {r4-r7}            /* Pop the high registers. */
   mov r8, r4
   mov r9, r5
   mov r10, r6
   mov r11, r7
   msr psp, r0                  /* Remember the new top of stack for the task. */
   subs r0, r0, #32            /* Go back for the low registers that are not automatically restored. */
   ldmia r0!, {r4-r7}              /* Pop low registers.  */
   bx r3
}

#elif defined   (  __GNUC__  )
void xPortPendSVHandler( void )
{
   /* This is a naked function. */

   __asm volatile
   (
   "   mrs r0, psp                     \n"
   "                              \n"
   "   ldr   r3, pxCurrentTCBConst         \n" /* Get the location of the current TCB. */
   "   ldr   r2, [r3]                  \n"
   "                              \n"
   "   sub r0, r0, #32                  \n" /* Make space for the remaining low registers. */
   "   str r0, [r2]                  \n" /* Save the new top of stack. */
   "   stmia r0!, {r4-r7}               \n" /* Store the low registers that are not saved automatically. */
   "    mov r4, r8                     \n" /* Store the high registers. */
   "    mov r5, r9                     \n"
   "    mov r6, r10                     \n"
   "    mov r7, r11                     \n"
   "    stmia r0!, {r4-r7}                 \n"
   "                              \n"
   "   push {r3, r14}                  \n"
   "   cpsid i                        \n"
   "   bl vTaskSwitchContext            \n"
   "   cpsie i                        \n"
   "   pop {r2, r3}                  \n" /* lr goes in r3. r2 now holds tcb pointer. */
   "                              \n"
   "   ldr r1, [r2]                  \n"
   "   ldr r0, [r1]                  \n" /* The first item in pxCurrentTCB is the task top of stack. */
   "   add r0, r0, #16                  \n" /* Move to the high registers. */
   "   ldmia r0!, {r4-r7}               \n" /* Pop the high registers. */
   "    mov r8, r4                     \n"
   "    mov r9, r5                     \n"
   "    mov r10, r6                     \n"
   "    mov r11, r7                     \n"
   "                              \n"
   "   msr psp, r0                     \n" /* Remember the new top of stack for the task. */
   "                              \n"
   "   sub r0, r0, #32                  \n" /* Go back for the low registers that are not automatically restored. */
   "    ldmia r0!, {r4-r7}                 \n" /* Pop low registers.  */
   "                              \n"
   "   bx r3                        \n"
   "                              \n"
   "   .align 2                     \n"
   "pxCurrentTCBConst: .word pxCurrentTCB     "
   );
}
#endif
/*-----------------------------------------------------------*/

#if configUSE_CUSTOM_TICK == 0
void xPortSysTickHandler( void )
{
unsigned long ulDummy;

   /* If using preemption, also force a context switch. */
   #if configUSE_PREEMPTION == 1
      *(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
   #endif

   ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
   {
      vTaskIncrementTick();
   }
   portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
   /* Configure SysTick to interrupt at the requested rate. */
   *(portNVIC_SYSTICK_LOAD) = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
   *(portNVIC_SYSTICK_CTRL) = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}
#endif
/*-----------------------------------------------------------*/

