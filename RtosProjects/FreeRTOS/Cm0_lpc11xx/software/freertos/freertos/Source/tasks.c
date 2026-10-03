/*
    FreeRTOS V7.4.2 - Copyright (C) 2013 Real Time Engineers Ltd.

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

    >>>>>>NOTE<<<<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
    details. You should have received a copy of the GNU General Public License
    and the FreeRTOS license exception along with FreeRTOS; if not it can be
    viewed here: http://www.freertos.org/a00114.html and also obtained by
    writing to Real Time Engineers Ltd., contact details for whom are available
    on the FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, and our new
    fully thread aware and reentrant UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems, who sell the code with commercial support,
    indemnification and middleware, under the OpenRTOS brand.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.
*/

/* Standard includes. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "StackMacros.h"

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Sanity check the configuration. */
#if configUSE_TICKLESS_IDLE != 0
	#if INCLUDE_vTaskSuspend != 1
		#error INCLUDE_vTaskSuspend must be set to 1 if configUSE_TICKLESS_IDLE is not set to 0
	#endif /* INCLUDE_vTaskSuspend */
#endif /* configUSE_TICKLESS_IDLE */

/*
 * Defines the size, in words, of the stack allocated to the idle task.
 */
#define tskIDLE_STACK_SIZE	configMINIMAL_STACK_SIZE

/*
 * Task control block.  A task control block (TCB) is allocated for each task,
 * and stores task state information, including a pointer to the task's context
 * (the task's run time environment, including register values)
 */
typedef struct tskTaskControlBlock
{
	volatile unsigned long	*pxTopOfStack;		/* long pointer < Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE TCB STRUCT. */
                                                /*  top of stack points to memory address where R4- R11 is saved (initially all these are junk but later they are stored in PendSV handler, 
                                                    please note that it does not store R0-R3,R12,LR,Pc,xPSR which is saved by CORE itself*/
   #if ( portUSING_MPU_WRAPPERS == 1 )   //defined in mcu_wrappers.h
      xMPU_SETTINGS xMPUSettings;            /*< The MPU settings are defined as part of the port layer.  THIS MUST BE THE SECOND MEMBER OF THE TCB STRUCT. */
   #endif

   xListItem            xGenericListItem;   /*< (used for main timeout)The list that the state list item of a task is reference from denotes the state of that task (Ready, Blocked, Suspended ). */
   xListItem            xEventListItem;      /*< (used for event triggers liek semaphone,mutex,event groups)Used to reference a task from an event list. */
   unsigned long           uxPriority;         /*< The priority of the task.  0 is the lowest priority. */
   unsigned long         *pxStack;         /*< Points to the start of the stack. */
   signed char            pcTaskName[ configMAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */

   #if ( portSTACK_GROWTH > 0 )
      unsigned long *pxEndOfStack;         /*< Points to the end of the stack on architectures where the stack grows up from low memory. */
   #endif

   #if ( portCRITICAL_NESTING_IN_TCB == 1 )
      unsigned long uxCriticalNesting; /*< Holds the critical section nesting depth for ports that do not maintain their own count in the port layer. */
   #endif

   #if ( configUSE_TRACE_FACILITY == 1 )
      unsigned long   uxTCBNumber;   /*< Stores a number that increments each time a TCB is created.  It allows debuggers to determine when a task has been deleted and then recreated. */
      unsigned long  uxTaskNumber;   /*< Stores a number specifically for use by third party trace code. */
   #endif

   #if ( configUSE_MUTEXES == 1 )
      unsigned long uxBasePriority;   /*< The priority last assigned to the task - used by the priority inheritance mechanism. */
   #endif

   #if ( configUSE_APPLICATION_TASK_TAG == 1 )
      pdTASK_HOOK_CODE pxTaskTag;
   #endif

   #if ( configGENERATE_RUN_TIME_STATS == 1 )
      unsigned long ulRunTimeCounter;         /*< Stores the amount of time the task has spent in the Running state. */
   #endif

} tskTCB;


/*
 * Some kernel aware debuggers require the data the debugger needs access to to
 * be global, rather than file scope.
 */
#ifdef portREMOVE_STATIC_QUALIFIER
   #define static
#endif

/*lint -e956 */
PRIVILEGED_DATA tskTCB * volatile pxCurrentTCB = NULL;

/* Lists for ready and blocked tasks. --------------------*/
PRIVILEGED_DATA static xList pxReadyTasksLists[ configMAX_PRIORITIES ];   /*< Prioritised ready tasks. */
PRIVILEGED_DATA static xList xDelayedTaskList1;                     /*< Delayed tasks. */
PRIVILEGED_DATA static xList xDelayedTaskList2;                     /*< Delayed tasks (two lists are used - one for delays that have overflowed the current tick count. */
PRIVILEGED_DATA static xList * volatile pxDelayedTaskList ;            /*< Points to the delayed task list currently being used. */
PRIVILEGED_DATA static xList * volatile pxOverflowDelayedTaskList;      /*< Points to the delayed task list currently being used to hold tasks that have overflowed the current tick count. */
PRIVILEGED_DATA static xList xPendingReadyList;                     /*< Tasks that have been readied while the scheduler was suspended.  They will be moved to the ready queue when the scheduler is resumed. */

#if ( INCLUDE_vTaskDelete == 1 )

   PRIVILEGED_DATA static xList xTasksWaitingTermination;            /*< Tasks that have been deleted - but the their memory not yet freed. */
   PRIVILEGED_DATA static volatile unsigned long uxTasksDeleted = ( unsigned long ) 0U;

#endif

#if ( INCLUDE_vTaskSuspend == 1 )

   PRIVILEGED_DATA static xList xSuspendedTaskList;               /*< Tasks that are currently suspended. */

#endif

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )

   PRIVILEGED_DATA static xTaskHandle xIdleTaskHandle = NULL;         /*< Holds the handle of the idle task.  The idle task is created automatically when the scheduler is started. */

#endif

/* File private variables. --------------------------------*/
PRIVILEGED_DATA static volatile unsigned long uxCurrentNumberOfTasks    = ( unsigned long ) 0U;
PRIVILEGED_DATA static volatile portTickType xTickCount                   = ( portTickType ) 0U;
PRIVILEGED_DATA static unsigned long uxTopUsedPriority                = tskIDLE_PRIORITY;
PRIVILEGED_DATA static volatile unsigned long uxTopReadyPriority       = tskIDLE_PRIORITY;
PRIVILEGED_DATA static volatile signed long xSchedulerRunning          = pdFALSE;
PRIVILEGED_DATA static volatile unsigned long uxSchedulerSuspended       = ( unsigned long ) pdFALSE;
PRIVILEGED_DATA static volatile unsigned long uxMissedTicks          = ( unsigned long ) 0U;
PRIVILEGED_DATA static volatile long xMissedYield                   = ( long ) pdFALSE;
PRIVILEGED_DATA static volatile long xNumOfOverflows                = ( long ) 0;
PRIVILEGED_DATA static unsigned long uxTaskNumber                   = ( unsigned long ) 0U;
PRIVILEGED_DATA static volatile portTickType xNextTaskUnblockTime            = ( portTickType ) portMAX_DELAY;//0xffffffff

#if ( configGENERATE_RUN_TIME_STATS == 1 )

   PRIVILEGED_DATA static char pcStatsString[ 50 ] ;
   PRIVILEGED_DATA static unsigned long ulTaskSwitchedInTime = 0UL;   /*< Holds the value of a timer/counter the last time a task was switched in. */
   PRIVILEGED_DATA static unsigned long ulTotalRunTime = 0UL;            /*< Holds the total amount of execution time as defined by the run time counter clock. */
   static void prvGenerateRunTimeStatsForTasksInList( const signed char *pcWriteBuffer, xList *pxList, unsigned long ulTotalRunTimeDiv100 ) PRIVILEGED_FUNCTION;

#endif

/* Debugging and trace facilities private variables and macros. ------------*/

/*
 * The value used to fill the stack of a task when the task is created.  This
 * is used purely for checking the high water mark for tasks.
 */
#define tskSTACK_FILL_BYTE   ( 0xa5U )

/*
 * Macros used by vListTask to indicate which state a task is in.
 */
#define tskBLOCKED_CHAR      ( ( signed char ) 'B' )
#define tskREADY_CHAR      ( ( signed char ) 'R' )
#define tskDELETED_CHAR      ( ( signed char ) 'D' )
#define tskSUSPENDED_CHAR   ( ( signed char ) 'S' )

/*-----------------------------------------------------------*/

#if ( configUSE_PORT_OPTIMISED_TASK_SELECTION == 0 )

   /* If configUSE_PORT_OPTIMISED_TASK_SELECTION is 0 then task selection is
   performed in a generic way that is not optimised to any particular
   microcontroller architecture. */

   /* uxTopReadyPriority holds the priority of the highest priority ready
   state task. */
   #define taskRECORD_READY_PRIORITY( uxPriority )                                                      \
   {                                                                                       \
      if( ( uxPriority ) > uxTopReadyPriority )                                                      \
      {                                                                                    \
         uxTopReadyPriority = ( uxPriority );                                                      \
      }                                                                                    \
   } /* taskRECORD_READY_PRIORITY */

   /*-----------------------------------------------------------*/
    /*MAGIC happens here, this macro look at what is high priority in ready list and points the pxIndex to that high priority task 
      the endlist is not changed but pxIndex and pxCurrenTCB is changed to selected high priority task,esle it will point to idle task
      xListEnd will absolutely always point to the Idle Task as long as the Idle Task is the only task inside that Priority 0 list
      pxReadyTasksLists Array (In RAM)
      ----------------------------------------------------------------------------------------
      [Index 2] Priority 2 Ready List (Task 3 only)
                +------------+          +------------+
                ¦  xListEnd  ¦ --><--   ¦Task 3 Node ¦   (Loops completely within Priority 2)
                +------------+          +------------+
      ----------------------------------------------------------------------------------------
      [Index 1] Priority 1 Ready List (Task 1 & Task 2)
                +------------+          +------------+          +------------+
                ¦  xListEnd  ¦ --><--   ¦Task 1 Node ¦ --><--   ¦Task 2 Node ¦   (Loops within same Priority 1)  *****REMEMBER SAME PRIORITY*****
                +------------+          +------------+          +------------+
      ----------------------------------------------------------------------------------------
      [Index 0] Priority 0 Ready List (Idle Task only)
                +------------+          +------------+
                ¦  xListEnd  ¦ --><--   ¦ Idle Task  ¦   (Loops completely within Priority 0)
                +------------+          +------------+
      ----------------------------------------------------------------------------------------
      NOTE:- if task3 is highest priority,it will not allow task2 and task1 and idle to run unless
             1.we insert vTaskDelay()
             2.yield
             3.blocked waiting for semaphore


    */
   #define taskSELECT_HIGHEST_PRIORITY_TASK()                                                         /*uxTopReadyPriority is the KEY thing*/ \
   {                                                                                                                       \
      /* NOTE:- This is where priotiry based sorting happens   */                                \
        /* Find the highest priority queue that contains ready tasks. */                                     \
      while( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxTopReadyPriority ] ) ) )                              /*check if number of items in the ready DB (depending on priority order)*/\
      {                                                                                    \
         configASSERT( uxTopReadyPriority );                                                         \
         --uxTopReadyPriority;                                                               /*descending order as high priority sits on top*/   \
      }                                                                                    /*idle taks is lowest prioriy with uxTopReadyPriority = 0*/ \
      /* listGET_OWNER_OF_NEXT_ENTRY indexes through the list, so the tasks of                              \
      the   same priority get an equal share of the processor time. */                                       \
      listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopReadyPriority ] ) );                  /*pxCurrentTCB points to idle by default,but moves to task1 or task2 base on priority*/ \
   } /* taskSELECT_HIGHEST_PRIORITY_TASK */

   /*-----------------------------------------------------------*/

   /* Define away taskRESET_READY_PRIORITY() and portRESET_READY_PRIORITY() as
   they are only required when a port optimised method of task selection is
   being used. */
   #define taskRESET_READY_PRIORITY( uxPriority )
   #define portRESET_READY_PRIORITY( uxPriority, uxTopReadyPriority )

#else /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

   /* If configUSE_PORT_OPTIMISED_TASK_SELECTION is 1 then task selection is
   performed in a way that is tailored to the particular microcontroller
   architecture being used. */

   /* A port optimised version is provided.  Call the port defined macros. */
   #define taskRECORD_READY_PRIORITY( uxPriority )   portRECORD_READY_PRIORITY( uxPriority, uxTopReadyPriority )

   /*-----------------------------------------------------------*/

   #define taskSELECT_HIGHEST_PRIORITY_TASK()                                          \
   {                                                                        \
   unsigned long uxTopPriority;                                             \
                                                                           \
      /* Find the highest priority queue that contains ready tasks. */                     \
      portGET_HIGHEST_PRIORITY( uxTopPriority, uxTopReadyPriority );                        \
      configASSERT( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ uxTopPriority ] ) ) > 0 );      \
      listGET_OWNER_OF_NEXT_ENTRY( pxCurrentTCB, &( pxReadyTasksLists[ uxTopPriority ] ) );      \
   } /* taskSELECT_HIGHEST_PRIORITY_TASK() */

   /*-----------------------------------------------------------*/

   /* A port optimised version is provided, call it only if the TCB being reset
   is being referenced from a ready list.  If it is referenced from a delayed
   or suspended list then it won't be in a ready list. */
   #define taskRESET_READY_PRIORITY( uxPriority )                                       \
   {                                                                        \
      if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ ( uxPriority ) ] ) ) == 0 )            \
      {                                                                     \
         portRESET_READY_PRIORITY( ( uxPriority ), ( uxTopReadyPriority ) );                  \
      }                                                                     \
   }

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/*
 * Place the task represented by pxTCB into the appropriate ready queue for
 * the task.  It is inserted at the end of the list.  One quirk of this is
 * that if the task being inserted is at the same priority as the currently
 * executing task, then it will only be rescheduled after the currently
 * executing task has been rescheduled.
 */
#define prvAddTaskToReadyQueue( pxTCB )                                                            \
   traceMOVED_TASK_TO_READY_STATE( pxTCB )                                                         \
   taskRECORD_READY_PRIORITY( ( pxTCB )->uxPriority );                                                \
   vListInsertEnd( ( xList * ) &( pxReadyTasksLists[ ( pxTCB )->uxPriority ] ), &( ( pxTCB )->xGenericListItem ) )
/*-----------------------------------------------------------*/
#if 0
/*
 * Macro that looks at the list of tasks that are currently delayed to see if
 * any require waking.
 *
 * Tasks are stored in the queue in the order of their wake time - meaning
 * once one tasks has been found whose timer has not expired we need not look
 * any further down the list.
 */
#define prvCheckDelayedTasks()                                             \
{                                                                  \
    portTickType xItemValue;                                                \
                                                                  \
   /* Is the tick count greater than or equal to the wake time of the first         \
   task referenced from the delayed tasks list? */                              /*xTickCount++ is done in TaskIncrementTick*/\
   if( xTickCount >= xNextTaskUnblockTime )                                 /*xNextTaskUnblockTime gets updated with xTimeToWake in  prvAddCurrentTaskToDelayedList*/\
   {                                                               \
      for( ;; )                                                      \
      {                                                            \
         if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )                  /*number of items ? in delayedtasklist*/\
         {                                                         \
            /* The delayed list is empty.  Set xNextTaskUnblockTime to the         \
            maximum possible value so it is extremely unlikely that the            \
            if( xTickCount >= xNextTaskUnblockTime ) test will pass next         \
            time through. */                                          \
            xNextTaskUnblockTime = portMAX_DELAY;                           \
            break;                                                   \
         }                                                         \
         else                                                      /*task item exists in delayedtasklist*/\
         {                                                         \
            /* The delayed list is not empty, get the value of the item at         \
            the head of the delayed list.  This is the time at which the         \
            task at the head of the delayed list should be removed from            \
            the Blocked state. */                                       \
            pxTCB = ( tskTCB * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );    /*as stated the pvOwner points to the task TCB's address*/ \
            xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xGenericListItem ) );    /*get the itemValue for timeout check*/ \
                                                                  \
            if( xTickCount < xItemValue )                                  /*new 1ms timestamp <  xItemValue  = old 1ms timestamp + xTicksToDelay*/\
            {                                                      \
               /* It is not time to unblock this item yet, but the item         \
               value is the time at which the task at the head of the            \
               blocked list should be removed from the Blocked state -            \
               so record the item value in xNextTaskUnblockTime. */            \
               xNextTaskUnblockTime = xItemValue;                           \
               break;                                                \
            }                                                      \
                                                                  \
            /* It is time to remove the item from the Blocked state. */            \
            uxListRemove( &( pxTCB->xGenericListItem ) );                     /*task which is associaed with genericlistiem is removed from the container(delayed Master DB)*/\
                                                                  \
            /* Is the task waiting on an event also? */                        \
            if( pxTCB->xEventListItem.pvContainer != NULL )                     \
            {                                                      \
               uxListRemove( &( pxTCB->xEventListItem ) );                     \
            }                                                      \
            prvAddTaskToReadyQueue( pxTCB );                              /*the task that was removed from delayed is now put into reader (container pointer change)*/\
         }                                                         \
      }                                                            \
   }                                                               \
}
#endif

/*-----------------------------------------------------------*/

/*
 * Several functions take an xTaskHandle parameter that can optionally be NULL,
 * where NULL is used to indicate that the handle of the currently executing
 * task should be used in place of the parameter.  This macro simply checks to
 * see if the parameter is NULL and returns a pointer to the appropriate TCB.
 */
#define prvGetTCBFromHandle( pxHandle ) ( ( ( pxHandle ) == NULL ) ? ( tskTCB * ) pxCurrentTCB : ( tskTCB * ) ( pxHandle ) )

/* Callback function prototypes. --------------------------*/
extern void vApplicationStackOverflowHook( xTaskHandle xTask, signed char *pcTaskName );
extern void vApplicationTickHook( void );

/* File private functions. --------------------------------*/

/*
 * Utility to ready a TCB for a given task.  Mainly just copies the parameters
 * into the TCB structure.
 */
static void prvInitialiseTCBVariables( tskTCB *pxTCB, const signed char * const pcName, unsigned long uxPriority, const xMemoryRegion * const xRegions, unsigned short usStackDepth ) PRIVILEGED_FUNCTION;

/*
 * Utility to ready all the lists used by the scheduler.  This is called
 * automatically upon the creation of the first task.
 */
static void prvInitialiseTaskLists( void ) PRIVILEGED_FUNCTION;

/*
 * The idle task, which as all tasks is implemented as a never ending loop.
 * The idle task is automatically created and added to the ready lists upon
 * creation of the first user task.
 *
 * The portTASK_FUNCTION_PROTO() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */
static portTASK_FUNCTION_PROTO( prvIdleTask, pvParameters );

/*
 * Utility to free all memory allocated by the scheduler to hold a TCB,
 * including the stack pointed to by the TCB.
 *
 * This does not free memory allocated by the task itself (i.e. memory
 * allocated by calls to pvPortMalloc from within the tasks application code).
 */
#if ( INCLUDE_vTaskDelete == 1 )

   static void prvDeleteTCB( tskTCB *pxTCB ) PRIVILEGED_FUNCTION;

#endif

/*
 * Used only by the idle task.  This checks to see if anything has been placed
 * in the list of tasks waiting to be deleted.  If so the task is cleaned up
 * and its TCB deleted.
 */
static void prvCheckTasksWaitingTermination( void ) PRIVILEGED_FUNCTION;

/*
 * The currently executing task is entering the Blocked state.  Add the task to
 * either the current or the overflow delayed task list.
 */
static void prvAddCurrentTaskToDelayedList( portTickType xTimeToWake ) PRIVILEGED_FUNCTION;

/*
 * Allocates memory from the heap for a TCB and associated stack.  Checks the
 * allocation was successful.
 */
static tskTCB *prvAllocateTCBAndStack( unsigned short usStackDepth, unsigned long *puxStackBuffer ) PRIVILEGED_FUNCTION;

/*
 * Called from vTaskList.  vListTasks details all the tasks currently under
 * control of the scheduler.  The tasks may be in one of a number of lists.
 * prvListTaskWithinSingleList accepts a list and details the tasks from
 * within just that list.
 *
 * THIS FUNCTION IS INTENDED FOR DEBUGGING ONLY, AND SHOULD NOT BE CALLED FROM
 * NORMAL APPLICATION CODE.
 */
#if ( configUSE_TRACE_FACILITY == 1 )

   static void prvListTaskWithinSingleList( const signed char *pcWriteBuffer, xList *pxList, signed char cStatus ) PRIVILEGED_FUNCTION;

#endif

/*
 * When a task is created, the stack of the task is filled with a known value.
 * This function determines the 'high water mark' of the task stack by
 * determining how much of the stack remains at the original preset value.
 */
#if ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )

   static unsigned short usTaskCheckFreeStackSpace( const unsigned char * pucStackByte ) PRIVILEGED_FUNCTION;

#endif

/*
 * Return the amount of time, in ticks, that will pass before the kernel will
 * next move a task from the Blocked state to the Running state.
 *
 * This conditional compilation should use inequality to 0, not equality to 1.
 * This is to ensure portSUPPRESS_TICKS_AND_SLEEP() can be called when user
 * defined low power mode implementations require configUSE_TICKLESS_IDLE to be
 * set to a value other than 1.
 */
#if ( configUSE_TICKLESS_IDLE != 0 )

   static portTickType prvGetExpectedIdleTime( void ) PRIVILEGED_FUNCTION;

#endif

static void prvCheckDelayedTasks(void);

/*lint +e956 */

/* as an example
[ ADDRESS 0x4000: Task 1 TCB ]
+--------------------------------------------------------+

| pxTopOfStack = 0x3FFF00                                |
+--------------------------------------------------------+
| xGenericListItem (The Node inside your list) ----------+---> The important thing is this name"xGenericListItem" is moved back and forth between master
|   - xItemValue   = 2000  (Wake time)                   +---> DBs(ready,delayed,suspended) but only pvOwner will help in finding the TCB and its xItemValue for timeout check
|   - pxNext       = 0xXXXX (Points to next node)        +
|   - pxPrevious   = 0xXXXX (Points to previous node)    +
|   - pvOwner      = 0x4000 -----------------------------+---> Points back to the start of TCB
|   - pvContainer  = 0x10A0 -----------------------------+---> Points to master DB whichever it is ready,delayed,suspended 
+--------------------------------------------------------+
| xEventListItem                                         |
+--------------------------------------------------------+
| uxPriority       = 1                                   |
+--------------------------------------------------------+

*/
signed long xTaskGenericCreate( pdTASK_CODE pxTaskCode, //function pointer of the one to be created
                                const signed char * const pcName, //just a name
                                unsigned short usStackDepth, //stack depth in terms of 2 or 4 bytes
                                void *pvParameters,          //NULL     
                                unsigned long uxPriority,    //< 8
                                xTaskHandle *pxCreatedTask,    //NULL
                                unsigned long *puxStackBuffer,        //NULL
                                const xMemoryRegion * const xRegions )//NULL
{
signed long xReturn;
tskTCB * pxNewTCB;

   configASSERT( pxTaskCode );
   configASSERT( ( ( uxPriority & ( ~portPRIVILEGE_BIT ) ) < configMAX_PRIORITIES ) );//max 8 priorities

   /* Allocate the memory required by the TCB and stack for the new task,
   checking that the allocation was successful. */
   pxNewTCB = prvAllocateTCBAndStack( usStackDepth, puxStackBuffer );//72 bytes of TCB and 64 x[4 word or 2 half word] of stack depth allocated
   if( pxNewTCB != NULL )
   {
      unsigned long *pxTopOfStack;

      #if( portUSING_MPU_WRAPPERS == 1 )
         /* Should the task be created in privileged mode? */
         long xRunPrivileged;
         if( ( uxPriority & portPRIVILEGE_BIT ) != 0U )
         {
            xRunPrivileged = pdTRUE;
         }
         else
         {
            xRunPrivileged = pdFALSE;
         }
         uxPriority &= ~portPRIVILEGE_BIT;
      #endif /* portUSING_MPU_WRAPPERS == 1 */

      /* Calculate the top of stack address.  This depends on whether the
      stack grows from high memory to low (as per the 80x86) or visa versa.
      portSTACK_GROWTH is used to make the result positive or negative as
      required by the port. */
      #if( portSTACK_GROWTH < 0 )
      {
         pxTopOfStack = pxNewTCB->pxStack + ( usStackDepth - ( unsigned short ) 1 );//ex:- address + (255 or 127)
         //below aligns to 8 word say addres 0xFFFF F200 ->1F8->1F0->1E8..etc i think
         pxTopOfStack = ( unsigned long * ) ( ( ( portPOINTER_SIZE_TYPE ) pxTopOfStack ) & ( ( portPOINTER_SIZE_TYPE ) ~portBYTE_ALIGNMENT_MASK  ) );

         /* Check the alignment of the calculated top of stack is correct. */
         configASSERT( ( ( ( unsigned long ) pxTopOfStack & ( unsigned long ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );
      }
      #else /* portSTACK_GROWTH */
      {
         pxTopOfStack = pxNewTCB->pxStack;

         /* Check the alignment of the stack buffer is correct. */
         configASSERT( ( ( ( unsigned long ) pxNewTCB->pxStack & ( unsigned long ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );

         /* If we want to use stack checking on architectures that use
         a positive stack growth direction then we also need to store the
         other extreme of the stack space. */
         pxNewTCB->pxEndOfStack = pxNewTCB->pxStack + ( usStackDepth - 1 );
      }
      #endif /* portSTACK_GROWTH */

      /* Setup the newly allocated TCB with the initial state of the task. */
      prvInitialiseTCBVariables( pxNewTCB, pcName, uxPriority, xRegions, usStackDepth );// i think xRegions is NULL,stack depth is 256 or 128

      /* Initialize the TCB stack to look as if the task was already running,
      but had been interrupted by the scheduler.  The return address is set
      to the start of the task function. Once the stack has been initialised
      the   top of stack variable is updated. */
      #if( portUSING_MPU_WRAPPERS == 1 )//defined in mcu_wrappers.h
      {
         pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters, xRunPrivileged );
      }
      #else /* portUSING_MPU_WRAPPERS */
      {
            /* 
             Below function does this:-
               push all 16 registers on to stack
               pxTopOfStack--; *pxTopOfStack = portINITIAL_XPSR;   // xPSR = 0x01000000 
               pxTopOfStack--;   *pxTopOfStack = ( portSTACK_TYPE ) pxCode;   // PC 
               pxTopOfStack -= 6;   // LR,R12,R3,R2,R1
               *pxTopOfStack = ( portSTACK_TYPE ) pvParameters;   R0 here
               pxTopOfStack -= 8; // R11,R10,R9,R8,R7,R6,R5,R4. 
            */
         pxNewTCB->pxTopOfStack = pxPortInitialiseStack( pxTopOfStack, pxTaskCode, pvParameters );//top of stack gets updated with 1st stack frame filledup
      }
      #endif /* portUSING_MPU_WRAPPERS */

      /* Check the alignment of the initialised stack. */ //if its not aligned to 8 word address boundary?
      portALIGNMENT_ASSERT_pxCurrentTCB( ( ( ( unsigned long ) pxNewTCB->pxTopOfStack & ( unsigned long ) portBYTE_ALIGNMENT_MASK ) == 0UL ) );

      if( ( void * ) pxCreatedTask != NULL )
      {
         /* Pass the TCB out - in an anonymous way.  The calling function/
         task can use this as a handle to delete the task later if
         required.*/
         *pxCreatedTask = ( xTaskHandle ) pxNewTCB;
      }

      /* We are going to manipulate the task queues to add this task to a
      ready list, so must make sure no interrupts occur. */
      taskENTER_CRITICAL();//cpsid i i.e i bit is set
      {
         uxCurrentNumberOfTasks++;
         if( pxCurrentTCB == NULL )
         {
            /* There are no other tasks, or all the other tasks are in
            the suspended state - make this the current task. */
            pxCurrentTCB =  pxNewTCB;//this is used in vPortSVCHandler for context switching

            if( uxCurrentNumberOfTasks == ( unsigned long ) 1 )
            {
               /* This is the first task to be created so do the preliminary
               initialisation required.  We will not recover if this call
               fails, but we will report the failure. */
               prvInitialiseTaskLists();
            }
         }
         else
         {
            /* If the scheduler is not already running, make this task the
            current task if it is the highest priority task to be created
            so far. */
            if( xSchedulerRunning == pdFALSE )
            {
               if( pxCurrentTCB->uxPriority <= uxPriority )
               {
                  pxCurrentTCB = pxNewTCB;
               }
            }
         }

         /* Remember the top priority to make context switching faster.  Use
         the priority in pxNewTCB as this has been capped to a valid value. */
         if( pxNewTCB->uxPriority > uxTopUsedPriority ) //>0 for default
         {
            uxTopUsedPriority = pxNewTCB->uxPriority;//this variable holds the highest priority of the task
         }

         uxTaskNumber++;

         #if ( configUSE_TRACE_FACILITY == 1 )
         {
            /* Add a counter into the TCB for tracing only. */
            pxNewTCB->uxTCBNumber = uxTaskNumber;
         }
         #endif /* configUSE_TRACE_FACILITY */
         traceTASK_CREATE( pxNewTCB );

         prvAddTaskToReadyQueue( pxNewTCB );

         xReturn = pdPASS;
         portSETUP_TCB( pxNewTCB );
      }
      taskEXIT_CRITICAL();
   }
   else
   {
      xReturn = errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;
      traceTASK_CREATE_FAILED();
   }

   if( xReturn == pdPASS )
   {
      if( xSchedulerRunning != pdFALSE )
      {
         /* If the created task is of a higher priority than the current task
         then it should run now. */
         if( pxCurrentTCB->uxPriority < uxPriority )
         {
            portYIELD_WITHIN_API();//triggers a PendSV hardware interrupt
         }
      }
   }

   return xReturn;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )

   void vTaskDelete( xTaskHandle xTaskToDelete )
   {
   tskTCB *pxTCB;

      taskENTER_CRITICAL();
      {
         /* Ensure a yield is performed if the current task is being
         deleted. */
         if( xTaskToDelete == pxCurrentTCB )
         {
            xTaskToDelete = NULL;
         }

         /* If null is passed in here then we are deleting ourselves. */
         pxTCB = prvGetTCBFromHandle( xTaskToDelete );

         /* Remove task from the ready list and place in the   termination list.
         This will stop the task from be scheduled.  The idle task will check
         the termination list and free up any memory allocated by the
         scheduler for the TCB and stack. */
         if( uxListRemove( ( xListItem * ) &( pxTCB->xGenericListItem ) ) == 0 )
         {
            taskRESET_READY_PRIORITY( pxTCB->uxPriority );
         }

         /* Is the task waiting on an event also? */
         if( pxTCB->xEventListItem.pvContainer != NULL )
         {
            uxListRemove( &( pxTCB->xEventListItem ) );
         }

         vListInsertEnd( ( xList * ) &xTasksWaitingTermination, &( pxTCB->xGenericListItem ) );

         /* Increment the ucTasksDeleted variable so the idle task knows
         there is a task that has been deleted and that it should therefore
         check the xTasksWaitingTermination list. */
         ++uxTasksDeleted;

         /* Increment the uxTaskNumberVariable also so kernel aware debuggers
         can detect that the task lists need re-generating. */
         uxTaskNumber++;

         traceTASK_DELETE( pxTCB );
      }
      taskEXIT_CRITICAL();

      /* Force a reschedule if we have just deleted the current task. */
      if( xSchedulerRunning != pdFALSE )
      {
         if( ( void * ) xTaskToDelete == NULL )
         {
            portYIELD_WITHIN_API();
         }
      }
   }

#endif /* INCLUDE_vTaskDelete */
/*-----------------------------------------------------------*/

//#if ( INCLUDE_vTaskDelayUntil == 1 )

void vTaskDelayUntil( portTickType * const pxPreviousWakeTime, portTickType xTimeIncrement )
{
   portTickType xTimeToWake;
   long xAlreadyYielded, xShouldDelay = pdFALSE;

      configASSERT( pxPreviousWakeTime );
      configASSERT( ( xTimeIncrement > 0U ) );

      vTaskSuspendAll();
      {
         /* Generate the tick time at which the task wants to wake. */
         xTimeToWake = *pxPreviousWakeTime + xTimeIncrement;//prevTimeToWakeUp becomes t0 ,so t1 = t0 + delay i.e new timetowakeup t1
             /*Note :- in case if t1 < 1ms timestamp i.e we are early to trigger, then the xShouldDelay is TRUE and do not immediately context switch*/
          if( xTickCount < *pxPreviousWakeTime )//t0 > 1mstick timstamp , meaning 1ms timer has gone beyond and overshot
         {
            /* The tick count has overflowed since this function was
            lasted called.  In this case the only time we should ever
            actually delay is if the wake time has also   overflowed,
            and the wake time is greater than the tick time.  When this
            is the case it is as if neither time had overflowed. */
            if( ( xTimeToWake < *pxPreviousWakeTime ) && ( xTimeToWake > xTickCount ) )//t1 > 1ms timstamp, i.e 1ms timer is catching up
            {
               xShouldDelay = pdTRUE;
            }
         }
         else //xTickCount > *pxPreviousWakeTime
         {
            /* The tick time has not overflowed.  In this case we will
            delay if either the wake time has overflowed, and/or the
            tick time is less than the wake time. */
            if( ( xTimeToWake < *pxPreviousWakeTime ) || ( xTimeToWake > xTickCount ) )//t1 > 1ms timstamp, i.e 1ms timer is catching up
            {
               xShouldDelay = pdTRUE;
            }
         }
             /*Note :- in case if t1 < 1ms timestamp i.e we are late to trigger, then the xShouldDelay is FALSE and immediately context switch*/

         /* Update the wake time ready for the next call. */
         *pxPreviousWakeTime = xTimeToWake;//make sure that new t0 = t1

         if( xShouldDelay != pdFALSE )
         {
            traceTASK_DELAY_UNTIL();

            /* We must remove ourselves from the ready list before adding
            ourselves to the blocked list as the same list item is used for
            both lists. */
            if( uxListRemove( ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) ) == 0 )
            {
               /* The current task must be in a ready list, so there is
               no need to check, and the port reset macro can be called
               directly. */
               portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );
            }

            prvAddCurrentTaskToDelayedList( xTimeToWake );
         }
      }
      xAlreadyYielded = xTaskResumeAll();

      /* Force a reschedule if xTaskResumeAll has not already done so, we may
      have put ourselves to sleep. */
      if( xAlreadyYielded == pdFALSE )
      {
         portYIELD_WITHIN_API();
      }
}

//#endif /* INCLUDE_vTaskDelayUntil */
/*-----------------------------------------------------------*/

//#if ( INCLUDE_vTaskDelay == 1 )
/*
   if there is a valid delay then:-
   1. vIncermentTick is completely blocked inside (not that interrupt is disable) except missedticks get added
   2.remove the task from current masterDB i.e ready list
   3.add the generic list of this hanging task to delayed or overdelayed list
   4.resume all the pending from pendingready DB whic has high priority task
   5.If already yeilded meaning if pendSV was set then ignore
*/
void vTaskDelay( portTickType xTicksToDelay )   //unsigned long
{
   portTickType xTimeToWake;
   signed long xAlreadyYielded = pdFALSE;

      /* A delay time of zero just forces a reschedule. */
      if( xTicksToDelay > ( portTickType ) 0U )
      {
         vTaskSuspendAll();//++uxSchedulerSuspended, >0 task suspended,else allowed,xTickCount will not increment
         {
            traceTASK_DELAY();

            /* A task that is removed from the event list while the
            scheduler is suspended will not get placed in the ready
            list or removed from the blocked list until the scheduler
            is resumed.

            This task cannot be in an event list as it is the currently
            executing task. */

            /* Calculate the time to wake - this may overflow but this is
            not a problem. */
            xTimeToWake = xTickCount + xTicksToDelay;//add current 1ms timestamp + offset i.e new timestamp (xTimeToWake is temporary,xTickCount increments inside vTaskIncrementTick())

            /* We must remove ourselves from the ready list before adding
            ourselves to the blocked list as the same list item is used for
            both lists. */
            if( uxListRemove( ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) ) == 0 )//this is the pxNewTCB->xGenericListItem
            {
               /* The current task must be in a ready list, so there is
               no need to check, and the port reset macro can be called
               directly. */
               portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );
            }
            prvAddCurrentTaskToDelayedList( xTimeToWake ); //xItemValue = xTimeToWake(1ms timestamp + xTicksToDelay)
         }
         xAlreadyYielded = xTaskResumeAll();
      }

      /* Force a reschedule if xTaskResumeAll has not already done so, we may
      have put ourselves to sleep. */
      if( xAlreadyYielded == pdFALSE )//meaning pendSV call was already raised inside taskResume then ignore it
      {
         portYIELD_WITHIN_API();//set a pendSV exception (dont get consufed with SVC)
      }
}

//#endif /* INCLUDE_vTaskDelay */
/*-----------------------------------------------------------*/

#if ( INCLUDE_eTaskGetState == 1 )

   eTaskState eTaskGetState( xTaskHandle xTask )
   {
   eTaskState eReturn;
   xList *pxStateList;
   tskTCB *pxTCB;

      pxTCB = ( tskTCB * ) xTask;

      if( pxTCB == pxCurrentTCB )
      {
         /* The task calling this function is querying its own state. */
         eReturn = eRunning;
      }
      else
      {
         taskENTER_CRITICAL();
         {
            pxStateList = ( xList * ) listLIST_ITEM_CONTAINER( &( pxTCB->xGenericListItem ) );
         }
         taskEXIT_CRITICAL();

         if( ( pxStateList == pxDelayedTaskList ) || ( pxStateList == pxOverflowDelayedTaskList ) )
         {
            /* The task being queried is referenced from one of the Blocked
            lists. */
            eReturn = eBlocked;
         }

         #if ( INCLUDE_vTaskSuspend == 1 )
            else if( pxStateList == &xSuspendedTaskList )
            {
               /* The task being queried is referenced from the suspended
               list. */
               eReturn = eSuspended;
            }
         #endif

         #if ( INCLUDE_vTaskDelete == 1 )
            else if( pxStateList == &xTasksWaitingTermination )
            {
               /* The task being queried is referenced from the deleted
               tasks list. */
               eReturn = eDeleted;
            }
         #endif

         else
         {
            /* If the task is not in any other state, it must be in the
            Ready (including pending ready) state. */
            eReturn = eReady;
         }
      }

      return eReturn;
   }

#endif /* INCLUDE_eTaskGetState */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskPriorityGet == 1 )

   unsigned long uxTaskPriorityGet( xTaskHandle xTask )
   {
   tskTCB *pxTCB;
   unsigned long uxReturn;

      taskENTER_CRITICAL();
      {
         /* If null is passed in here then we are changing the
         priority of the calling function. */
         pxTCB = prvGetTCBFromHandle( xTask );
         uxReturn = pxTCB->uxPriority;
      }
      taskEXIT_CRITICAL();

      return uxReturn;
   }

#endif /* INCLUDE_uxTaskPriorityGet */
/*-----------------------------------------------------------*/
/*
-----------------------------+
¦ pxIndex ---> [ Task 2 Node ]    ¦
¦ uxNumberOfItems = 2             ¦
+---------------------------------+
                ¦
                v
   +---------------------------+         +---------------------------+
   ¦ Task 2 Node               ¦         ¦ Task 1 Node               ¦
   ¦ (pxTCB of Task 2)         ¦         ¦ (pxCurrentTCB)            ¦
   +---------------------------¦         +---------------------------¦
   ¦ uxPriority     = 1        ¦         ¦ uxPriority     = 1        ¦
   ¦ uxBasePriority = 1        ¦         ¦ uxBasePriority = 1        ¦
   ¦ +-----------------------+ ¦         ¦ +-----------------------+ ¦
   ¦ ¦ xGenericListItem      ¦ ¦         ¦ ¦ xGenericListItem      ¦ ¦
   ¦ ¦ xItemValue = 1        ¦ ¦         ¦ ¦ xItemValue = 1        ¦ ¦
   ¦ ¦ pvContainer = &List[1]¦ +->Next-->¦ ¦ pvContainer = &List[1]¦ +->Back to
   ¦ +-----------------------+ ¦         ¦ +-----------------------+ ¦  List End
   +---------------------------+         +---------------------------+




pxReadyTasksLists[1] (List Head)
+---------------------------------+
¦ pxIndex ---> [ Task 1 Node ]    ¦
¦ uxNumberOfItems = 1             ¦   <-- Decremented by 1. Since it's NOT 0,
+---------------------------------+       taskRESET_READY_PRIORITY(1) is skipped.
                ¦
                V
   +---------------------------+
   ¦ Task 1 Node (Active)      ¦
   ¦ (pxCurrentTCB)            ¦
   +---------------------------¦
   ¦ uxPriority     = 1        ¦
   ¦ +-----------------------+ ¦
   ¦ ¦ xGenericListItem      ¦ ¦
   ¦ ¦ pvContainer = &List[1]¦ +-> Links directly back to List End
   ¦ +-----------------------+ ¦
   +---------------------------+
  
   DETACHED FLOATING NODE IN RAM:
   +---------------------------+
   ¦ Task 2 Node TCB           ¦
   +---------------------------¦
   ¦ uxPriority     = 4        ¦   <-- Updated to New Priority
   ¦ uxBasePriority = 4        ¦
   ¦ +-----------------------+ ¦
   ¦ ¦ xGenericListItem      ¦ ¦
   ¦ ¦ xItemValue = 4        ¦ ¦   <-- Priority value injected into node sorting key
   ¦ ¦ pvContainer = NULL    ¦ ¦   <-- Cleared out by uxListRemove
   ¦ +-----------------------+ ¦
   +---------------------------+





pxReadyTasksLists[4] (Highest List Head Row)
+---------------------------------+
¦ pxIndex ---> [ Task 2 Node ]    ¦
¦ uxNumberOfItems = 1             ¦   <-- Priority 4 bit is turned ON in Ready Bitmask!
+---------------------------------+
                ¦
                V
   +---------------------------+
   ¦ Task 2 Node (Ready)       ¦
   +---------------------------¦
   ¦ uxPriority     = 4        ¦
   ¦ +-----------------------+ ¦
   ¦ ¦ xGenericListItem      ¦ ¦
   ¦ ¦ xItemValue = 4        ¦ ¦
   ¦ ¦ pvContainer = &List[4]¦ +-> Links back to List[4] End node
   ¦ +-----------------------+ ¦
   +---------------------------+

... Rows 3 and 2 are empty ...

pxReadyTasksLists[1] (Lower Row)
+---------------------------------+
¦ pxIndex ---> [ Task 1 Node ]    ¦
¦ uxNumberOfItems = 1             ¦
+---------------------------------+
                ¦
                V
   +---------------------------+
   ¦ Task 1 Node               ¦
   +---------------------------¦
   ¦ uxPriority     = 1        ¦
   ¦ +-----------------------+ ¦
   ¦ ¦ xGenericListItem      ¦ ¦
   ¦ ¦ pvContainer = &List[1]¦
   ¦ +-----------------------+ ¦
   +---------------------------+


*/
//#if ( INCLUDE_vTaskPrioritySet == 1 )

void vTaskPrioritySet( xTaskHandle xTask, unsigned long uxNewPriority )
{
   tskTCB *pxTCB;
   unsigned long uxCurrentPriority, uxPriorityUsedOnEntry;
   long xYieldRequired = pdFALSE;

      configASSERT( ( uxNewPriority < configMAX_PRIORITIES ) );

      /* Ensure the new priority is valid. */
      if( uxNewPriority >= configMAX_PRIORITIES )   //if > 8 
      {
         uxNewPriority = configMAX_PRIORITIES - ( unsigned long ) 1U; //new priotiy is 7
      }

      taskENTER_CRITICAL();
      {
         if( xTask == ( xTaskHandle ) pxCurrentTCB )//same task?
         {
            xTask = NULL;
         }

         /* If null is passed in here then we are changing the
         priority of the calling function. */
         pxTCB = prvGetTCBFromHandle( xTask );//task that you want to change priority TCB address

         traceTASK_PRIORITY_SET( pxTCB, uxNewPriority );
            /*NOTE :- here we have to get the current priority of the task you want to escalate to*/
         #if ( configUSE_MUTEXES == 1 )
         {
            uxCurrentPriority = pxTCB->uxBasePriority;
         }
         #else
         {
            uxCurrentPriority = pxTCB->uxPriority;
         }
         #endif

         if( uxCurrentPriority != uxNewPriority )//if change in  requested and current priority of the task you want to escalate,else just leave
         {
            /* The priority change may have readied a task of higher
            priority than the calling task. */
            if( uxNewPriority > uxCurrentPriority )//new task has higher priority so yield required
            {
               if( xTask != NULL )
               {
                  /* The priority of another task is being raised.  If we
                  were raising the priority of the currently running task
                  there would be no need to switch as it must have already
                  been the highest priority task. */
                  xYieldRequired = pdTRUE;
               }
            }
            else if( xTask == NULL )//means new priority is lower than current,so yield
            {
               /* Setting our own priority down means there may now be another
               task of higher priority that is ready to execute. */
               xYieldRequired = pdTRUE;
            }

            /* Remember the ready list the task might be referenced from
            before its uxPriority member is changed so the
            taskRESET_READY_PRIORITY() macro can function correctly. */
            uxPriorityUsedOnEntry = pxTCB->uxPriority;

            #if ( configUSE_MUTEXES == 1 )
            {
               /* Only change the priority being used if the task is not
               currently using an inherited priority. */
               if( pxTCB->uxBasePriority == pxTCB->uxPriority )
               {
                  pxTCB->uxPriority = uxNewPriority;
               }

               /* The base priority gets set whatever. */
               pxTCB->uxBasePriority = uxNewPriority;
            }
            #else
            {
               pxTCB->uxPriority = uxNewPriority;
            }
            #endif

            listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), ( configMAX_PRIORITIES - ( portTickType ) uxNewPriority ) );//eventList not genericlist

            /* If the task is in the blocked or suspended list we need do
            nothing more than change it's priority variable. However, if
            the task is in a ready list it needs to be removed and placed
            in the queue appropriate to its new priority. */
            /*this is just an extra verification that task you want to change is actually pointint to correct DB or not based on current priority index */
            if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ uxCurrentPriority ] ), &( pxTCB->xGenericListItem ) ) )//by using  currentpriority as index 
            {                                                                                                         //and check wether the TCB's container is pointing to correct ready[currentpriority] DB (i.e ready array )
               /* The task is currently in its ready list - remove before adding
               it to it's new ready list.  As we are in a critical section we
               can do this even if the scheduler is suspended. */
               if( uxListRemove( ( xListItem * ) &( pxTCB->xGenericListItem ) ) == 0 )//then remove this as ready[currentpriority] DB 
               {
                  taskRESET_READY_PRIORITY( uxPriorityUsedOnEntry );
               }
               prvAddTaskToReadyQueue( pxTCB );//now insert that TCB which we removed and hanging, into new ready[newpriority] DB pointed  by new priority
            }

            if( xYieldRequired == pdTRUE )//yield is anyway needed as priority has changed
            {
               portYIELD_WITHIN_API();//pendSV for context switch
            }

            /* Remove compiler warning about unused variables when the port
            optimised task selection is not being used. */
            ( void ) uxPriorityUsedOnEntry;
         }
      }
      taskEXIT_CRITICAL();
}

//#endif /* INCLUDE_vTaskPrioritySet */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )

   void vTaskSuspend( xTaskHandle xTaskToSuspend )
   {
   tskTCB *pxTCB;

      taskENTER_CRITICAL();
      {
         /* Ensure a yield is performed if the current task is being
         suspended. */
         if( xTaskToSuspend == ( xTaskHandle ) pxCurrentTCB )
         {
            xTaskToSuspend = NULL;
         }

         /* If null is passed in here then we are suspending ourselves. */
         pxTCB = prvGetTCBFromHandle( xTaskToSuspend );

         traceTASK_SUSPEND( pxTCB );

         /* Remove task from the ready/delayed list and place in the   suspended list. */
         if( uxListRemove( ( xListItem * ) &( pxTCB->xGenericListItem ) ) == 0 )
         {
            taskRESET_READY_PRIORITY( pxTCB->uxPriority );
         }

         /* Is the task waiting on an event also? */
         if( pxTCB->xEventListItem.pvContainer != NULL )
         {
            uxListRemove( &( pxTCB->xEventListItem ) );
         }

         vListInsertEnd( ( xList * ) &xSuspendedTaskList, &( pxTCB->xGenericListItem ) );
      }
      taskEXIT_CRITICAL();

      if( ( void * ) xTaskToSuspend == NULL )
      {
         if( xSchedulerRunning != pdFALSE )
         {
            /* We have just suspended the current task. */
            portYIELD_WITHIN_API();
         }
         else
         {
            /* The scheduler is not running, but the task that was pointed
            to by pxCurrentTCB has just been suspended and pxCurrentTCB
            must be adjusted to point to a different task. */
            if( listCURRENT_LIST_LENGTH( &xSuspendedTaskList ) == uxCurrentNumberOfTasks )
            {
               /* No other tasks are ready, so set pxCurrentTCB back to
               NULL so when the next task is created pxCurrentTCB will
               be set to point to it no matter what its relative priority
               is. */
               pxCurrentTCB = NULL;
            }
            else
            {
               vTaskSwitchContext();
            }
         }
      }
   }

#endif /* INCLUDE_vTaskSuspend */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )

   signed long xTaskIsTaskSuspended( xTaskHandle xTask )
   {
   long xReturn = pdFALSE;
   const tskTCB * const pxTCB = ( tskTCB * ) xTask;

      /* It does not make sense to check if the calling task is suspended. */
      configASSERT( xTask );

      /* Is the task we are attempting to resume actually in the
      suspended list? */
      if( listIS_CONTAINED_WITHIN( &xSuspendedTaskList, &( pxTCB->xGenericListItem ) ) != pdFALSE )
      {
         /* Has the task already been resumed from within an ISR? */
         if( listIS_CONTAINED_WITHIN( &xPendingReadyList, &( pxTCB->xEventListItem ) ) != pdTRUE )
         {
            /* Is it in the suspended list because it is in the
            Suspended state?  It is possible to be in the suspended
            list because it is blocked on a task with no timeout
            specified. */
            if( listIS_CONTAINED_WITHIN( NULL, &( pxTCB->xEventListItem ) ) == pdTRUE )
            {
               xReturn = pdTRUE;
            }
         }
      }

      return xReturn;
   }

#endif /* INCLUDE_vTaskSuspend */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskSuspend == 1 )

   void vTaskResume( xTaskHandle xTaskToResume )
   {
   tskTCB *pxTCB;

      /* It does not make sense to resume the calling task. */
      configASSERT( xTaskToResume );

      /* Remove the task from whichever list it is currently in, and place
      it in the ready list. */
      pxTCB = ( tskTCB * ) xTaskToResume;

      /* The parameter cannot be NULL as it is impossible to resume the
      currently executing task. */
      if( ( pxTCB != NULL ) && ( pxTCB != pxCurrentTCB ) )
      {
         taskENTER_CRITICAL();
         {
            if( xTaskIsTaskSuspended( pxTCB ) == pdTRUE )
            {
               traceTASK_RESUME( pxTCB );

               /* As we are in a critical section we can access the ready
               lists even if the scheduler is suspended. */
               uxListRemove(  &( pxTCB->xGenericListItem ) );
               prvAddTaskToReadyQueue( pxTCB );

               /* We may have just resumed a higher priority task. */
               if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
               {
                  /* This yield may not cause the task just resumed to run, but
                  will leave the lists in the correct state for the next yield. */
                  portYIELD_WITHIN_API();
               }
            }
         }
         taskEXIT_CRITICAL();
      }
   }

#endif /* INCLUDE_vTaskSuspend */

/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskResumeFromISR == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) )

   long xTaskResumeFromISR( xTaskHandle xTaskToResume )
   {
   long xYieldRequired = pdFALSE;
   tskTCB *pxTCB;
   unsigned long uxSavedInterruptStatus;

      configASSERT( xTaskToResume );

      pxTCB = ( tskTCB * ) xTaskToResume;

      uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
      {
         if( xTaskIsTaskSuspended( pxTCB ) == pdTRUE )
         {
            traceTASK_RESUME_FROM_ISR( pxTCB );

            if( uxSchedulerSuspended == ( unsigned long ) pdFALSE )
            {
               xYieldRequired = ( pxTCB->uxPriority >= pxCurrentTCB->uxPriority );
               uxListRemove(  &( pxTCB->xGenericListItem ) );
               prvAddTaskToReadyQueue( pxTCB );
            }
            else
            {
               /* We cannot access the delayed or ready lists, so will hold this
               task pending until the scheduler is resumed, at which point a
               yield will be performed if necessary. */
               vListInsertEnd( ( xList * ) &( xPendingReadyList ), &( pxTCB->xEventListItem ) );
            }
         }
      }
      portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

      return xYieldRequired;
   }

#endif /* ( ( INCLUDE_xTaskResumeFromISR == 1 ) && ( INCLUDE_vTaskSuspend == 1 ) ) */
/*-----------------------------------------------------------*/

void vTaskStartScheduler( void )
{
long xReturn;

   /* Add the idle task at the lowest priority. */
   #if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )
   {
      /* Create the idle task, storing its handle in xIdleTaskHandle so it can
      be returned by the xTaskGetIdleTaskHandle() function. */
      xReturn = xTaskCreate( prvIdleTask, ( signed char * ) "IDLE", tskIDLE_STACK_SIZE, ( void * ) NULL, ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), &xIdleTaskHandle );
   }
   #else
   {
      /* Create the idle task without storing its handle. */
      xReturn = xTaskCreate( prvIdleTask, ( signed char * ) "IDLE", tskIDLE_STACK_SIZE, ( void * ) NULL, ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), NULL );
   }
   #endif /* INCLUDE_xTaskGetIdleTaskHandle */

   #if ( configUSE_TIMERS == 1 )
   {
      if( xReturn == pdPASS )
      {
         xReturn = xTimerCreateTimerTask();
      }
   }
   #endif /* configUSE_TIMERS */

   if( xReturn == pdPASS )
   {
      /* Interrupts are turned off here, to ensure a tick does not occur
      before or during the call to xPortStartScheduler().  The stacks of
      the created tasks contain a status word with interrupts switched on
      so interrupts will automatically get re-enabled when the first task
      starts to run.

      STEPPING THROUGH HERE USING A DEBUGGER CAN CAUSE BIG PROBLEMS IF THE
      DEBUGGER ALLOWS INTERRUPTS TO BE PROCESSED. */
      portDISABLE_INTERRUPTS();

      xSchedulerRunning = pdTRUE;
      xTickCount = ( portTickType ) 0U;

      /* If configGENERATE_RUN_TIME_STATS is defined then the following
      macro must be defined to configure the timer/counter used to generate
      the run time counter time base. */
      portCONFIGURE_TIMER_FOR_RUN_TIME_STATS();

      /* Setting up the timer tick is hardware specific and thus in the
      portable interface. */
      if( xPortStartScheduler() != pdFALSE )
      {
         /* Should not reach here as if the scheduler is running the
         function will not return. */
      }
      else
      {
         /* Should only reach here if a task calls xTaskEndScheduler(). */
      }
   }
   else
   {
      /* This line will only be reached if the kernel could not be started,
      because there was not enough FreeRTOS heap to create the idle task
      or the timer task. */
      configASSERT( xReturn );
   }
}
/*-----------------------------------------------------------*/

void vTaskEndScheduler( void )
{
   /* Stop the scheduler interrupts and call the portable scheduler end
   routine so the original ISRs can be restored if necessary.  The port
   layer must ensure interrupts enable   bit is left in the correct state. */
   portDISABLE_INTERRUPTS();
   xSchedulerRunning = pdFALSE;
   vPortEndScheduler();
}
/*----------------------------------------------------------*/

void vTaskSuspendAll( void )
{
   /* A critical section is not required as the variable is of type
   long. */
   ++uxSchedulerSuspended;
}
/*----------------------------------------------------------*/

#if ( configUSE_TICKLESS_IDLE != 0 )

   static portTickType prvGetExpectedIdleTime( void )
   {
   portTickType xReturn;

      if( pxCurrentTCB->uxPriority > tskIDLE_PRIORITY )
      {
         xReturn = 0;
      }
      else if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > 1 )
      {
         /* There are other idle priority tasks in the ready state.  If
         time slicing is used then the very next tick interrupt must be
         processed. */
         xReturn = 0;
      }
      else
      {
         xReturn = xNextTaskUnblockTime - xTickCount;
      }

      return xReturn;
   }

#endif /* configUSE_TICKLESS_IDLE */
/*----------------------------------------------------------*/
/*
   1.Disable all the interrupt so that another pendSV will not corrupt registers
   2.if task was suspended then uxSchedulerSuspended becomes +ve,so now reduce it by one.
   3.if its 0 meaning scheduler is not suspended,allowed to schedule, if there are tasks created then proceed, else exit 
   4.if number of items in pendinglist, then take the Endlist of the pending DB and point to the next task its pointing to,
     then look at the endlist, take the Owner fetch the next tasks TCB address 
   5.remove the generic and event list of TCB from pendingReady
   6.add the TCB in Ready DB list
   7.if this tasks piority is above the current tasks TCB then a context switch is needed
   8.during this time a missed tick cannot happen, but if it happens then also context switch is needed
   9.Enable all interrupts
*/
signed long xTaskResumeAll( void )
{
    register tskTCB *pxTCB;
    signed long xAlreadyYielded = pdFALSE;

   /* If uxSchedulerSuspended is zero then this function does not match a
   previous call to vTaskSuspendAll(). */
   configASSERT( uxSchedulerSuspended );

   /* It is possible that an ISR caused a task to be removed from an event
   list while the scheduler was suspended.  If this was the case then the
   removed task will have been added to the xPendingReadyList.  Once the
   scheduler has been resumed it is safe to move all the pending ready
   tasks from this list into their appropriate ready list. */
   taskENTER_CRITICAL(); //disable all interrupt including tick 
   {
      --uxSchedulerSuspended;//if it was >0 then task was suspended, else allowed to run

      if( uxSchedulerSuspended == ( unsigned long ) pdFALSE )//if scheduler is not locked then you can switch tasks lists
      {
         if( uxCurrentNumberOfTasks > ( unsigned long ) 0U )//increments in xTaskGenericCreate
         {
            long xYieldRequired = pdFALSE;

            /* Move any readied tasks from the pending list into the appropriate ready list. */
            while( listLIST_IS_EMPTY( ( xList * ) &xPendingReadyList ) == pdFALSE )//uxNumberOfItems is 0 in vListInitialise,else something inside pending list so enter while loop 
            {
               pxTCB = ( tskTCB * ) listGET_OWNER_OF_HEAD_ENTRY(  ( ( xList * ) &xPendingReadyList ) );//looke at Endlist (anchorwall ) of pending DB to point to TCB ,get the owner i.e TCB's address
               /*we are removing this task's TCB from PendinReadylist , also cleanup the PendinReady's DB's main pointer
                 also we need to stitch bak the EndList of PendingReady back to next task or Endlist itself
               */
               uxListRemove( &( pxTCB->xEventListItem ) );//EndList->pxNext->pvOwner , take the pxTCB->EventList and remove it
               uxListRemove( &( pxTCB->xGenericListItem ) );//EndList->pxNext->pvOwner , take the pxTCB->GenericList and remove it
               prvAddTaskToReadyQueue( pxTCB );//put pxTCB in readytasklist 

               /* If we have moved a task that has a priority higher than the current task then we should yield. */
               if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )//task in pending is higher than current running then do context switch
               {
                  xYieldRequired = pdTRUE;
               }
            }

            /* If any ticks occurred while the scheduler was suspended then
            they should be processed now.  This ensures the tick count does not
            slip, and that any delayed tasks are resumed at the correct time. */
            if( uxMissedTicks > ( unsigned long ) 0U )
            {
               while( uxMissedTicks > ( unsigned long ) 0U )
               {
                  vTaskIncrementTick();
                  --uxMissedTicks;
               }

               /* As we have processed some ticks it is appropriate to yield
               to ensure the highest priority task that is ready to run is
               the task actually running. */
               #if configUSE_PREEMPTION == 1
               {
                  xYieldRequired = pdTRUE;
               }
               #endif
            }

            if( ( xYieldRequired == pdTRUE ) || ( xMissedYield == pdTRUE ) )
            {
               xAlreadyYielded = pdTRUE;
               xMissedYield = pdFALSE;
               portYIELD_WITHIN_API();
            }
         }
      }
   }
   taskEXIT_CRITICAL();//resume all interruts including tick

   return xAlreadyYielded;
}
/*-----------------------------------------------------------*/

portTickType xTaskGetTickCount( void )
{
portTickType xTicks;

   /* Critical section required if running on a 16 bit processor. */
   taskENTER_CRITICAL();
   {
      xTicks = xTickCount;
   }
   taskEXIT_CRITICAL();

   return xTicks;
}
/*-----------------------------------------------------------*/

portTickType xTaskGetTickCountFromISR( void )
{
portTickType xReturn;
unsigned long uxSavedInterruptStatus;

   uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
   xReturn = xTickCount;
   portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

   return xReturn;
}
/*-----------------------------------------------------------*/

unsigned long uxTaskGetNumberOfTasks( void )
{
   /* A critical section is not required because the variables are of type
   long. */
   return uxCurrentNumberOfTasks;
}
/*-----------------------------------------------------------*/

#if ( INCLUDE_pcTaskGetTaskName == 1 )

   signed char *pcTaskGetTaskName( xTaskHandle xTaskToQuery )
   {
   tskTCB *pxTCB;

      /* If null is passed in here then the name of the calling task is being queried. */
      pxTCB = prvGetTCBFromHandle( xTaskToQuery );
      configASSERT( pxTCB );
      return &( pxTCB->pcTaskName[ 0 ] );
   }

#endif /* INCLUDE_pcTaskGetTaskName */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   void vTaskList( signed char *pcWriteBuffer )
   {
   unsigned long uxQueue;

      /* This is a VERY costly function that should be used for debug only.
      It leaves interrupts disabled for a LONG time. */

      vTaskSuspendAll();
      {
         /* Run through all the lists that could potentially contain a TCB and
         report the task name, state and stack high water mark. */

         *pcWriteBuffer = ( signed char ) 0x00;
         strcat( ( char * ) pcWriteBuffer, ( const char * ) "\r\n" );

         uxQueue = uxTopUsedPriority + ( unsigned long ) 1U;

         do
         {
            uxQueue--;

            if( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxQueue ] ) ) == pdFALSE )
            {
               prvListTaskWithinSingleList( pcWriteBuffer, ( xList * ) &( pxReadyTasksLists[ uxQueue ] ), tskREADY_CHAR );
            }
         }while( uxQueue > ( unsigned short ) tskIDLE_PRIORITY );

         if( listLIST_IS_EMPTY( pxDelayedTaskList ) == pdFALSE )
         {
            prvListTaskWithinSingleList( pcWriteBuffer, ( xList * ) pxDelayedTaskList, tskBLOCKED_CHAR );
         }

         if( listLIST_IS_EMPTY( pxOverflowDelayedTaskList ) == pdFALSE )
         {
            prvListTaskWithinSingleList( pcWriteBuffer, ( xList * ) pxOverflowDelayedTaskList, tskBLOCKED_CHAR );
         }

         #if( INCLUDE_vTaskDelete == 1 )
         {
            if( listLIST_IS_EMPTY( &xTasksWaitingTermination ) == pdFALSE )
            {
               prvListTaskWithinSingleList( pcWriteBuffer, &xTasksWaitingTermination, tskDELETED_CHAR );
            }
         }
         #endif

         #if ( INCLUDE_vTaskSuspend == 1 )
         {
            if( listLIST_IS_EMPTY( &xSuspendedTaskList ) == pdFALSE )
            {
               prvListTaskWithinSingleList( pcWriteBuffer, &xSuspendedTaskList, tskSUSPENDED_CHAR );
            }
         }
         #endif
      }
      xTaskResumeAll();
   }

#endif /* configUSE_TRACE_FACILITY */
/*----------------------------------------------------------*/

#if ( configGENERATE_RUN_TIME_STATS == 1 )

   void vTaskGetRunTimeStats( signed char *pcWriteBuffer )
   {
   unsigned long uxQueue;
   unsigned long ulTotalRunTimeDiv100;

      /* This is a VERY costly function that should be used for debug only.
      It leaves interrupts disabled for a LONG time. */

      vTaskSuspendAll();
      {
         #ifdef portALT_GET_RUN_TIME_COUNTER_VALUE
            portALT_GET_RUN_TIME_COUNTER_VALUE( ulTotalRunTime );
         #else
            ulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
         #endif

         /* Divide ulTotalRunTime by 100 to make the percentage caluclations
         simpler in the prvGenerateRunTimeStatsForTasksInList() function. */
         ulTotalRunTimeDiv100 = ulTotalRunTime / 100UL;

         /* Run through all the lists that could potentially contain a TCB,
         generating a table of run timer percentages in the provided
         buffer. */

         *pcWriteBuffer = ( signed char ) 0x00;
         strcat( ( char * ) pcWriteBuffer, ( const char * ) "\r\n" );

         uxQueue = uxTopUsedPriority + ( unsigned long ) 1U;

         do
         {
            uxQueue--;

            if( listLIST_IS_EMPTY( &( pxReadyTasksLists[ uxQueue ] ) ) == pdFALSE )
            {
               prvGenerateRunTimeStatsForTasksInList( pcWriteBuffer, ( xList * ) &( pxReadyTasksLists[ uxQueue ] ), ulTotalRunTimeDiv100 );
            }
         }while( uxQueue > ( unsigned short ) tskIDLE_PRIORITY );

         if( listLIST_IS_EMPTY( pxDelayedTaskList ) == pdFALSE )
         {
            prvGenerateRunTimeStatsForTasksInList( pcWriteBuffer, ( xList * ) pxDelayedTaskList, ulTotalRunTimeDiv100 );
         }

         if( listLIST_IS_EMPTY( pxOverflowDelayedTaskList ) == pdFALSE )
         {
            prvGenerateRunTimeStatsForTasksInList( pcWriteBuffer, ( xList * ) pxOverflowDelayedTaskList, ulTotalRunTimeDiv100 );
         }

         #if ( INCLUDE_vTaskDelete == 1 )
         {
            if( listLIST_IS_EMPTY( &xTasksWaitingTermination ) == pdFALSE )
            {
               prvGenerateRunTimeStatsForTasksInList( pcWriteBuffer, &xTasksWaitingTermination, ulTotalRunTimeDiv100 );
            }
         }
         #endif

         #if ( INCLUDE_vTaskSuspend == 1 )
         {
            if( listLIST_IS_EMPTY( &xSuspendedTaskList ) == pdFALSE )
            {
               prvGenerateRunTimeStatsForTasksInList( pcWriteBuffer, &xSuspendedTaskList, ulTotalRunTimeDiv100 );
            }
         }
         #endif
      }
      xTaskResumeAll();
   }

#endif /* configGENERATE_RUN_TIME_STATS */
/*----------------------------------------------------------*/

#if ( INCLUDE_xTaskGetIdleTaskHandle == 1 )

   xTaskHandle xTaskGetIdleTaskHandle( void )
   {
      /* If xTaskGetIdleTaskHandle() is called before the scheduler has been
      started, then xIdleTaskHandle will be NULL. */
      configASSERT( ( xIdleTaskHandle != NULL ) );
      return xIdleTaskHandle;
   }

#endif /* INCLUDE_xTaskGetIdleTaskHandle */
/*----------------------------------------------------------*/

/* This conditional compilation should use inequality to 0, not equality to 1.
This is to ensure vTaskStepTick() is available when user defined low power mode
implementations require configUSE_TICKLESS_IDLE to be set to a value other than
1. */
#if ( configUSE_TICKLESS_IDLE != 0 )

   void vTaskStepTick( portTickType xTicksToJump )
   {
      configASSERT( ( xTickCount + xTicksToJump ) <= xNextTaskUnblockTime );
      xTickCount += xTicksToJump;
   }

#endif /* configUSE_TICKLESS_IDLE */
/*----------------------------------------------------------*/
/*
[ 0x10A0: pxDelayedTaskList ]
+------------------------------------+
¦ uxNumberOfItems = 2                ¦
¦ pxIndex = 0x10A8 (xListEnd)        ¦
+------------------------------------+
                 |
                 v 
+---------------------------------------------------------------------------------------+
¦ xListEnd (Anchor Wall at Address 0x10A8)                                              ¦
¦   - xItemValue  = 0xFFFFFFFF                                                          ¦
¦   - pxNext      = 0x4008 -------------------------+ (Points to Task 1)                ¦
¦   - pxPrevious  = 0x5008 <-----------------+      |                                   ¦
+--------------------------------------------|------|-----------------------------------+

                                             |      |
    +----------------------------------------+      |

    |                                               |
    v                                               v
+-----------------------------------+           +-----------------------------------+
¦ TCB 1 (vLEDTask1) at 0x4000       ¦           ¦ TCB 2 (vLEDTask2) at 0x5000       ¦
¦                                   ¦           ¦                                   ¦
¦ +-------------------------------+ ¦           ¦ +-------------------------------+ ¦
¦ ¦ xGenericListItem at 0x4008    ¦ ¦           ¦ ¦ xGenericListItem at 0x5008    ¦ ¦
¦ ¦   - xItemValue = 1000         ¦ ¦           ¦ ¦   - xItemValue = 2000         ¦ ¦
¦ ¦   - pxNext     = 0x5008 ------|-|-----------|-|-> - pxNext     = 0x10A8 ------|-|---+
¦ ¦   - pxPrevious = 0x10A8 <-----|-|-+         ¦ ¦   - pxPrevious = 0x4008 <-----|-|--+|
¦ ¦   - pvOwner    = 0x4000       ¦ ¦ |         ¦ ¦   - pvOwner    = 0x5000       ¦ ¦  ||
¦ ¦   - pvContainer= 0x10A0       ¦ ¦ |         ¦ ¦   - pvContainer= 0x10A0       ¦ ¦  ||
¦ +-------------------------------+ ¦ |         ¦ +-------------------------------+ ¦  ||
+-----------------------------------+ |         +-----------------------------------+  ||
    ^                                 |             ^                                  ||
    |                                 |             |                                  ||
    +---------------------------------+             +----------------------------------+|
                                                                                       ||
    +----------------------------------------------------------------------------------+|

    |                                                                                   |
    +-----------------------------------------------------------------------------------+

#########################################################################################
                             Task1 gets complete detached
#########################################################################################


[ 0x10A0: pxDelayedTaskList ]
+------------------------------------+
¦ uxNumberOfItems = 1                ¦  <=== Decremented to 1
¦ pxIndex = 0x10A8 (xListEnd)        ¦
+------------------------------------+
                 |
                 v
+---------------------------------------------------------------------------------------+
¦ xListEnd (Anchor Wall at Address 0x10A8)                                              ¦
¦   - xItemValue  = 0xFFFFFFFF                                                          ¦
¦   - pxNext      = 0x5008 -----------------------------------------+ (Now points to T2)¦
¦   - pxPrevious  = 0x5008 <---------------------------------+      |                   ¦
+------------------------------------------------------------|------|-------------------+

                                                             |      |
                                                             v      v
                                                +-----------------------------------+
                                                ¦ TCB 2 (vLEDTask2) at 0x5000       ¦
                                                ¦                                   ¦
                                                ¦ +-------------------------------+ ¦
                                                ¦ | xGenericListItem at 0x5008    | ¦
                                                ¦ |   - xItemValue = 2000         | ¦
                                                ¦ |   - pxNext     = 0x10A8 ------|-|---+
                                                ¦ |   - pxPrevious = 0x10A8 <-----|-|-+ |
                                                ¦ |   - pvOwner    = 0x5000 ------|-|-|-+
                                                ¦ |   - pvContainer= 0x10A0       | | | |
                                                ¦ +-------------------------------+ ¦ | |
                                                +-----------------------------------+ | |
                                                    ^                                 | |

                                                    |                                 | |
                                                    +---------------------------------+ |
                                                                                        v
                                                                             [0x5000: TCB 2 Head]

  [ FREED & ISOLATED NODE STATE: TASK 1 ]
  +-------------------------------+
  ¦ xGenericListItem at 0x4008    ¦ 
  ¦   - xItemValue = 1000         ¦ 
  ¦   - pxNext     = 0x5008       ¦ (Leftover values; bypassed by active list pointers)
  ¦   - pxPrevious = 0x10A8       ¦ 
  ¦   - pvOwner    = 0x4000 ------+---> Points back to its TCB container
  ¦   - pvContainer= NULL         ¦ <=== Safely cleared to NULL!
  +-------------------------------+

*/
void vTaskIncrementTick( void )
{
    tskTCB * pxTCB;

   /* Called by the portable layer each time a tick interrupt occurs.
   Increments the tick then checks to see if the new tick value will cause any
   tasks to be unblocked. */
   traceTASK_INCREMENT_TICK( xTickCount );
   if( uxSchedulerSuspended == ( unsigned long ) pdFALSE )//if >0 then suspended else not
   {
      ++xTickCount;
      if( xTickCount == ( portTickType ) 0U )//its overflow repair code (do not think that xTickCount was negative)
      {
         xList *pxTemp;

         /* Tick count has overflowed so we need to swap the delay lists.
         If there are any items in pxDelayedTaskList here then there is
         an error! */
         configASSERT( ( listLIST_IS_EMPTY( pxDelayedTaskList ) ) );

         //a temporary list is used to swap between delayed and overflowdelayed DB
         pxTemp = pxDelayedTaskList;
         pxDelayedTaskList = pxOverflowDelayedTaskList;
         pxOverflowDelayedTaskList = pxTemp;
         xNumOfOverflows++;

         if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )//if the newly swapped previous overflowdelayed has any empty?
         {
            /* The new current delayed list is empty.  Set
            xNextTaskUnblockTime to the maximum possible value so it is
            extremely unlikely that the
            if( xTickCount >= xNextTaskUnblockTime ) test will pass until
            there is an item in the delayed list. */
            xNextTaskUnblockTime = portMAX_DELAY;//0xffffffff
         }
         else
         {
            /* The new current delayed list is not empty, get the value of
            the item at the head of the delayed list.  This is the time at
            which the task at the head of the delayed list should be removed
            from the Blocked state. */
            pxTCB = ( tskTCB * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );//this owner assignment happens in TaskGenericCreate (pxDelayedTaskList->xListEnd->pxNext->pvOwner = pxTCB)
            xNextTaskUnblockTime = listGET_LIST_ITEM_VALUE( &( pxTCB->xGenericListItem ) );//xNextTaskUnblockTime= xItemValue
         }
      }

      /* See if this tick has made a timeout expire. */
      prvCheckDelayedTasks();//itemValue is checked and
   }
   else
   {
      ++uxMissedTicks;

      /* The tick hook gets called at regular intervals, even if the
      scheduler is locked. */
      #if ( configUSE_TICK_HOOK == 1 )
      {
         vApplicationTickHook();
      }
      #endif
   }

   #if ( configUSE_TICK_HOOK == 1 )
   {
      /* Guard against the tick hook being called when the missed tick
      count is being unwound (when the scheduler is being unlocked. */
      if( uxMissedTicks == ( unsigned long ) 0U )
      {
         vApplicationTickHook();
      }
   }
   #endif /* configUSE_TICK_HOOK */
}
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )

   void vTaskSetApplicationTaskTag( xTaskHandle xTask, pdTASK_HOOK_CODE pxHookFunction )
   {
   tskTCB *xTCB;

      /* If xTask is NULL then we are setting our own task hook. */
      if( xTask == NULL )
      {
         xTCB = ( tskTCB * ) pxCurrentTCB;
      }
      else
      {
         xTCB = ( tskTCB * ) xTask;
      }

      /* Save the hook function in the TCB.  A critical section is required as
      the value can be accessed from an interrupt. */
      taskENTER_CRITICAL();
         xTCB->pxTaskTag = pxHookFunction;
      taskEXIT_CRITICAL();
   }

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )

   pdTASK_HOOK_CODE xTaskGetApplicationTaskTag( xTaskHandle xTask )
   {
   tskTCB *xTCB;
   pdTASK_HOOK_CODE xReturn;

      /* If xTask is NULL then we are setting our own task hook. */
      if( xTask == NULL )
      {
         xTCB = ( tskTCB * ) pxCurrentTCB;
      }
      else
      {
         xTCB = ( tskTCB * ) xTask;
      }

      /* Save the hook function in the TCB.  A critical section is required as
      the value can be accessed from an interrupt. */
      taskENTER_CRITICAL();
         xReturn = xTCB->pxTaskTag;
      taskEXIT_CRITICAL();

      return xReturn;
   }

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

#if ( configUSE_APPLICATION_TASK_TAG == 1 )

   long xTaskCallApplicationTaskHook( xTaskHandle xTask, void *pvParameter )
   {
   tskTCB *xTCB;
   long xReturn;

      /* If xTask is NULL then we are calling our own task hook. */
      if( xTask == NULL )
      {
         xTCB = ( tskTCB * ) pxCurrentTCB;
      }
      else
      {
         xTCB = ( tskTCB * ) xTask;
      }

      if( xTCB->pxTaskTag != NULL )
      {
         xReturn = xTCB->pxTaskTag( pvParameter );
      }
      else
      {
         xReturn = pdFAIL;
      }

      return xReturn;
   }

#endif /* configUSE_APPLICATION_TASK_TAG */
/*-----------------------------------------------------------*/

void vTaskSwitchContext( void )
{
   if( uxSchedulerSuspended != ( unsigned long ) pdFALSE )
   {
      /* The scheduler is currently suspended - do not allow a context
      switch. */
      xMissedYield = pdTRUE;
   }
   else
   {
      traceTASK_SWITCHED_OUT();

      #if ( configGENERATE_RUN_TIME_STATS == 1 )
      {
            #ifdef portALT_GET_RUN_TIME_COUNTER_VALUE
               portALT_GET_RUN_TIME_COUNTER_VALUE( ulTotalRunTime );
            #else
               ulTotalRunTime = portGET_RUN_TIME_COUNTER_VALUE();
            #endif

            /* Add the amount of time the task has been running to the accumulated
            time so far.  The time the task started running was stored in
            ulTaskSwitchedInTime.  Note that there is no overflow protection here
            so count values are only valid until the timer overflows.  Generally
            this will be about 1 hour assuming a 1uS timer increment. */
            pxCurrentTCB->ulRunTimeCounter += ( ulTotalRunTime - ulTaskSwitchedInTime );
            ulTaskSwitchedInTime = ulTotalRunTime;
      }
      #endif /* configGENERATE_RUN_TIME_STATS */

      taskFIRST_CHECK_FOR_STACK_OVERFLOW();
      taskSECOND_CHECK_FOR_STACK_OVERFLOW();

      taskSELECT_HIGHEST_PRIORITY_TASK();//uxPriority is set in prvinitialiseTCBvariables which gets initialzed from xTaskGenericCreate

      traceTASK_SWITCHED_IN();
   }
}
/*-----------------------------------------------------------*/
/*
   1. Place EventList and GenericList in xSuspendedTaskList if xTicksToWait is portMAX_DELAY (indefinite)
                                         pxOverflowDelayedTaskList if xticks overshot xTicksToWait 
                                         pxDelayedTaskList if xTicks waiting to hit timeout
*/
void vTaskPlaceOnEventList( const xList * const pxEventList, portTickType xTicksToWait )
{
    portTickType xTimeToWake;

   configASSERT( pxEventList );

   /* THIS FUNCTION MUST BE CALLED WITH INTERRUPTS DISABLED OR THE
   SCHEDULER SUSPENDED. */

   /* Place the event list item of the TCB in the appropriate event list.
   This is placed in the list in priority order so the highest priority task
   is the first to be woken by the event. */
   vListInsert( ( xList * ) pxEventList, ( xListItem * ) &( pxCurrentTCB->xEventListItem ) );//Insert tasks xEventListItem into the xTasksWaitingToSend DB list 

   /* We must remove ourselves from the ready list before adding ourselves
   to the blocked list as the same list item is used for both lists.  We have
   exclusive access to the ready lists as the scheduler is locked. */
   if( uxListRemove( ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) ) == 0 )//removed the current generic list item from the DB
   {
      /* The current task must be in a ready list, so there is no need to
      check, and the port reset macro can be called directly. */
      portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );
   }

   #if ( INCLUDE_vTaskSuspend == 1 )
   {
      if( xTicksToWait == portMAX_DELAY )//no indefinite wait
      {
         /* Add ourselves to the suspended task list instead of a delayed task
         list to ensure we are not woken by a timing event.  We will block
         indefinitely. */
         vListInsertEnd( ( xList * ) &xSuspendedTaskList, ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) );
      }
      else
      {
         /* Calculate the time at which the task should be woken if the event does
         not occur.  This may overflow but this doesn't matter. */
         xTimeToWake = xTickCount + xTicksToWait;
         prvAddCurrentTaskToDelayedList( xTimeToWake );
      }
   }
   #else /* INCLUDE_vTaskSuspend */
   {
         /* Calculate the time at which the task should be woken if the event does
         not occur.  This may overflow but this doesn't matter. */
         xTimeToWake = xTickCount + xTicksToWait;
         prvAddCurrentTaskToDelayedList( xTimeToWake );//insert the current task into delayed list with newtime to wakeup (this is just lke vdelaytask)
   }
   #endif /* INCLUDE_vTaskSuspend */
}
/*-----------------------------------------------------------*/

#if configUSE_TIMERS == 1

   void vTaskPlaceOnEventListRestricted( const xList * const pxEventList, portTickType xTicksToWait )
   {
   portTickType xTimeToWake;

      configASSERT( pxEventList );

      /* This function should not be called by application code hence the
      'Restricted' in its name.  It is not part of the public API.  It is
      designed for use by kernel code, and has special calling requirements -
      it should be called from a critical section. */


      /* Place the event list item of the TCB in the appropriate event list.
      In this case it is assume that this is the only task that is going to
      be waiting on this event list, so the faster vListInsertEnd() function
      can be used in place of vListInsert. */
      vListInsertEnd( ( xList * ) pxEventList, ( xListItem * ) &( pxCurrentTCB->xEventListItem ) );

      /* We must remove this task from the ready list before adding it to the
      blocked list as the same list item is used for both lists.  This
      function is called form a critical section. */
      if( uxListRemove( ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) ) == 0 )
      {
         /* The current task must be in a ready list, so there is no need to
         check, and the port reset macro can be called directly. */
         portRESET_READY_PRIORITY( pxCurrentTCB->uxPriority, uxTopReadyPriority );
      }

      /* Calculate the time at which the task should be woken if the event does
      not occur.  This may overflow but this doesn't matter. */
      xTimeToWake = xTickCount + xTicksToWait;

      traceTASK_DELAY_UNTIL();
      prvAddCurrentTaskToDelayedList( xTimeToWake );
   }

#endif /* configUSE_TIMERS */
/*-----------------------------------------------------------*/
/*
   1.Remove the eventlist and generic list of TCB from currnet container 
     and put in PendingList or ReadyTask List depending on TaskSuspended state
*/
signed long xTaskRemoveFromEventList( const xList * const pxEventList )
{
    tskTCB *pxUnblockedTCB;
    long xReturn;

   /* THIS FUNCTION MUST BE CALLED WITH INTERRUPTS DISABLED OR THE
   SCHEDULER SUSPENDED.  It can also be called from within an ISR. */

   /* The event list is sorted in priority order, so we can remove the
   first in the list, remove the TCB from the delayed list, and add
   it to the ready list.

   If an event is for a queue that is locked then this function will never
   get called - the lock count on the queue will get modified instead.  This
   means we can always expect exclusive access to the event list here.

   This function assumes that a check has already been made to ensure that
   pxEventList is not empty. */
   pxUnblockedTCB = ( tskTCB * ) listGET_OWNER_OF_HEAD_ENTRY( pxEventList );//get the task that is waiting 
   configASSERT( pxUnblockedTCB );
   uxListRemove( &( pxUnblockedTCB->xEventListItem ) );//remove the eventlistitem from pvContainer i.e the xTasksWaitingToReceive

   if( uxSchedulerSuspended == ( unsigned long ) pdFALSE )//dont get confused wrt critical section,scheduler not suspended
   {
      uxListRemove( &( pxUnblockedTCB->xGenericListItem ) );//remove the genericlistitem also from xTasksWaitingToReceive
      prvAddTaskToReadyQueue( pxUnblockedTCB );//add the TCB to the pxReadyTasksLists master list DB
   }
   else//scheduler suspended
   {
      /* We cannot access the delayed or ready lists, so will hold this task pending until the scheduler is resumed. */
      vListInsertEnd( ( xList * ) &( xPendingReadyList ), &( pxUnblockedTCB->xEventListItem ) );//if scheduler suspended then add to xPendingReady list 
   }

   if( pxUnblockedTCB->uxPriority >= pxCurrentTCB->uxPriority )
   {
      /* Return true if the task removed from the event list has
      a higher priority than the calling task.  This allows
      the calling task to know if it should force a context
      switch now. */
      xReturn = pdTRUE;
   }
   else//current task priority is same as unblocked task priority as per xTasksWaitingToReceive
   {
      xReturn = pdFALSE;
   }

   return xReturn;
}
/*-----------------------------------------------------------*/

void vTaskSetTimeOutState( xTimeOutType * const pxTimeOut )
{
   configASSERT( pxTimeOut );
   pxTimeOut->xOverflowCount = xNumOfOverflows;//1ms tick overflow
   pxTimeOut->xTimeOnEntering = xTickCount;//1ms timestamp
}
/*-----------------------------------------------------------*/

long xTaskCheckForTimeOut( xTimeOutType * const pxTimeOut, portTickType * const pxTicksToWait )
{
    long xReturn;

   configASSERT( pxTimeOut );
   configASSERT( pxTicksToWait );

   taskENTER_CRITICAL();
   {
      #if ( INCLUDE_vTaskSuspend == 1 )
         /* If INCLUDE_vTaskSuspend is set to 1 and the block time specified is
         the maximum block time then the task should block indefinitely, and
         therefore never time out. */
         if( *pxTicksToWait == portMAX_DELAY )
         {
            xReturn = pdFALSE;
         }
         else /* We are not blocking indefinitely, perform the checks below. */
      #endif

      if( ( xNumOfOverflows != pxTimeOut->xOverflowCount ) && ( ( portTickType ) xTickCount >= ( portTickType ) pxTimeOut->xTimeOnEntering ) )
      {
         /* The tick count is greater than the time at which vTaskSetTimeout()
         was called, but has also overflowed since vTaskSetTimeOut() was called.
         It must have wrapped all the way around and gone past us again. This
         passed since vTaskSetTimeout() was called. */
         xReturn = pdTRUE;
      }
      /*
      Imagine if task entered the function at xTimeOnEntering 100 with xTicksToWait = 10. 
      Due to high-priority interrupts processing , suppose 4 ticks pass before the scheduler 
      can execute this check line. xTickCount becomes 104.The math check executes: 104 - 100 = 4 ticks 
      already spent.Is 4 < 10? True. Time hasn't fully expired.The adjustment code updates 
      variable: *pxTicksToWait -= 4 \[\rightarrow \] 10 - 4 = 6
      */
      else if( ( ( portTickType ) ( ( portTickType ) xTickCount - ( portTickType ) pxTimeOut->xTimeOnEntering ) ) < ( portTickType ) *pxTicksToWait )
      {
         /* Not a genuine timeout. Adjust parameters for time remaining. */
         *pxTicksToWait -= ( ( portTickType ) xTickCount - ( portTickType ) pxTimeOut->xTimeOnEntering );
         vTaskSetTimeOutState( pxTimeOut );//update new xTimeOnEntering as 6 
         xReturn = pdFALSE;
      }
      else
      {
         xReturn = pdTRUE;
      }
   }
   taskEXIT_CRITICAL();

   return xReturn;
}
/*-----------------------------------------------------------*/

void vTaskMissedYield( void )
{
   xMissedYield = pdTRUE;
}
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   unsigned long uxTaskGetTaskNumber( xTaskHandle xTask )
   {
   unsigned long uxReturn;
   tskTCB *pxTCB;

      if( xTask != NULL )
      {
         pxTCB = ( tskTCB * ) xTask;
         uxReturn = pxTCB->uxTaskNumber;
      }
      else
      {
         uxReturn = 0U;
      }

      return uxReturn;
   }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   void vTaskSetTaskNumber( xTaskHandle xTask, unsigned long uxHandle )
   {
   tskTCB *pxTCB;

      if( xTask != NULL )
      {
         pxTCB = ( tskTCB * ) xTask;
         pxTCB->uxTaskNumber = uxHandle;
      }
   }

#endif /* configUSE_TRACE_FACILITY */

/*
 * -----------------------------------------------------------
 * The Idle task.
 * ----------------------------------------------------------
 *
 * The portTASK_FUNCTION() macro is used to allow port/compiler specific
 * language extensions.  The equivalent prototype for this function is:
 *
 * void prvIdleTask( void *pvParameters );
 *
 */
static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
   /* Stop warnings. */
   ( void ) pvParameters;

   for( ;; )
   {
      /* See if any tasks have been deleted. */
      prvCheckTasksWaitingTermination();

      #if ( configUSE_PREEMPTION == 0 )
      {
         /* If we are not using preemption we keep forcing a task switch to
         see if any other task has become available.  If we are using
         preemption we don't need to do this as any task becoming available
         will automatically get the processor anyway. */
         taskYIELD();
      }
      #endif /* configUSE_PREEMPTION */

      #if ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) )
      {
         /* When using preemption tasks of equal priority will be
         timesliced.  If a task that is sharing the idle priority is ready
         to run then the idle task should yield before the end of the
         timeslice.

         A critical region is not required here as we are just reading from
         the list, and an occasional incorrect value will not matter.  If
         the ready list at the idle priority contains more than one task
         then a task other than the idle task is ready to execute. */
         if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > ( unsigned long ) 1 )
         {
            taskYIELD();
         }
      }
      #endif /* ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) ) */

      #if ( configUSE_IDLE_HOOK == 1 )
      {
         extern void vApplicationIdleHook( void );

         /* Call the user defined function from within the idle task.  This
         allows the application designer to add background functionality
         without the overhead of a separate task.
         NOTE: vApplicationIdleHook() MUST NOT, UNDER ANY CIRCUMSTANCES,
         CALL A FUNCTION THAT MIGHT BLOCK. */
         vApplicationIdleHook();
      }
      #endif /* configUSE_IDLE_HOOK */

      /* This conditional compilation should use inequality to 0, not equality
      to 1.  This is to ensure portSUPPRESS_TICKS_AND_SLEEP() is called when
      user defined low power mode   implementations require
      configUSE_TICKLESS_IDLE to be set to a value other than 1. */
      #if ( configUSE_TICKLESS_IDLE != 0 )
      {
      portTickType xExpectedIdleTime;

         /* It is not desirable to suspend then resume the scheduler on
         each iteration of the idle task.  Therefore, a preliminary
         test of the expected idle time is performed without the
         scheduler suspended.  The result here is not necessarily
         valid. */
         xExpectedIdleTime = prvGetExpectedIdleTime();

         if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
         {
            vTaskSuspendAll();
            {
               /* Now the scheduler is suspended, the expected idle
               time can be sampled again, and this time its value can
               be used. */
               configASSERT( xNextTaskUnblockTime >= xTickCount );
               xExpectedIdleTime = prvGetExpectedIdleTime();

               if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
               {
                  portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime );
               }
            }
            xTaskResumeAll();
         }
      }
      #endif /* configUSE_TICKLESS_IDLE */
   }
} /*lint !e715 pvParameters is not accessed but all task functions require the same prototype. */
/*-----------------------------------------------------------*/

#if configUSE_TICKLESS_IDLE != 0

   eSleepModeStatus eTaskConfirmSleepModeStatus( void )
   {
   eSleepModeStatus eReturn = eStandardSleep;

      if( listCURRENT_LIST_LENGTH( &xPendingReadyList ) != 0 )
      {
         /* A task was made ready while the scheduler was suspended. */
         eReturn = eAbortSleep;
      }
      else if( xMissedYield != pdFALSE )
      {
         /* A yield was pended while the scheduler was suspended. */
         eReturn = eAbortSleep;
      }
      else
      {
         #if configUSE_TIMERS == 0
         {
            /* The idle task exists in addition to the application tasks. */
            const unsigned long uxNonApplicationTasks = 1;

            /* If timers are not being used and all the tasks are in the
            suspended list (which might mean they have an infinite block
            time rather than actually being suspended) then it is safe to
            turn all clocks off and just wait for external interrupts. */
            if( listCURRENT_LIST_LENGTH( &xSuspendedTaskList ) == ( uxCurrentNumberOfTasks - uxNonApplicationTasks ) )
            {
               eReturn = eNoTasksWaitingTimeout;
            }
         }
         #endif /* configUSE_TIMERS */
      }

      return eReturn;
   }
#endif /* configUSE_TICKLESS_IDLE */
/*-----------------------------------------------------------*/

static void prvInitialiseTCBVariables( tskTCB *pxTCB, 
                                       const signed char * const pcName, 
                                       unsigned long uxPriority, 
                                       const xMemoryRegion * const xRegions, 
                                       unsigned short usStackDepth )
{
   /* Store the function name in the TCB. */
   #if configMAX_TASK_NAME_LEN > 1
   {
      /* Don't bring strncpy into the build unnecessarily. */
      strncpy( ( char * ) pxTCB->pcTaskName, ( const char * ) pcName, ( unsigned short ) configMAX_TASK_NAME_LEN );
   }
   #endif /* configMAX_TASK_NAME_LEN */
   pxTCB->pcTaskName[ ( unsigned short ) configMAX_TASK_NAME_LEN - ( unsigned short ) 1 ] = ( signed char ) '\0';

   /* This is used as an array index so must ensure it's not too large.  First
   remove the privilege bit if one is present. */
   if( uxPriority >= configMAX_PRIORITIES )//if >8 then make it 7
   {
      uxPriority = configMAX_PRIORITIES - ( unsigned long ) 1U;
   }

   pxTCB->uxPriority = uxPriority;
   #if ( configUSE_MUTEXES == 1 )
   {
      pxTCB->uxBasePriority = uxPriority;
   }
   #endif /* configUSE_MUTEXES */

   vListInitialiseItem( &( pxTCB->xGenericListItem ) );//made pvContainer as NULL
   vListInitialiseItem( &( pxTCB->xEventListItem ) );//made pvContainer as NULL

   /* Set the pxTCB as a link back from the xListItem.  This is so we can get
   back to   the containing TCB from a generic item in a list. */
   listSET_LIST_ITEM_OWNER( &( pxTCB->xGenericListItem ), pxTCB );//pxTCB->xGenericListItem->pvOwner = pxTCB

   /* Event lists are always in priority order. */
   listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), configMAX_PRIORITIES - ( portTickType ) uxPriority );//&( pxTCB->xEventListItem )->xItemValue = configMAX_PRIORITIES - uxPriority
   listSET_LIST_ITEM_OWNER( &( pxTCB->xEventListItem ), pxTCB );//pxTCB->xEventListItem->pvOwner = pxTCB

   #if ( portCRITICAL_NESTING_IN_TCB == 1 )
   {
      pxTCB->uxCriticalNesting = ( unsigned long ) 0U;
   }
   #endif /* portCRITICAL_NESTING_IN_TCB */

   #if ( configUSE_APPLICATION_TASK_TAG == 1 )
   {
      pxTCB->pxTaskTag = NULL;
   }
   #endif /* configUSE_APPLICATION_TASK_TAG */

   #if ( configGENERATE_RUN_TIME_STATS == 1 )
   {
      pxTCB->ulRunTimeCounter = 0UL;
   }
   #endif /* configGENERATE_RUN_TIME_STATS */

   #if ( portUSING_MPU_WRAPPERS == 1 )//defined in mcu_wrappers.h
   {
      vPortStoreTaskMPUSettings( &( pxTCB->xMPUSettings ), xRegions, pxTCB->pxStack, usStackDepth );
   }
   #else /* portUSING_MPU_WRAPPERS */
   {
      ( void ) xRegions;
      ( void ) usStackDepth;
   }
   #endif /* portUSING_MPU_WRAPPERS */

}
/*-----------------------------------------------------------*/

#if ( portUSING_MPU_WRAPPERS == 1 )

   void vTaskAllocateMPURegions( xTaskHandle xTaskToModify, const xMemoryRegion * const xRegions )
   {
   tskTCB *pxTCB;

      if( xTaskToModify == pxCurrentTCB )
      {
         xTaskToModify = NULL;
      }

      /* If null is passed in here then we are deleting ourselves. */
      pxTCB = prvGetTCBFromHandle( xTaskToModify );

        vPortStoreTaskMPUSettings( &( pxTCB->xMPUSettings ), xRegions, NULL, 0 );
   }

#endif /* portUSING_MPU_WRAPPERS */
/*-----------------------------------------------------------*/

static void prvInitialiseTaskLists( void )//during initialization of(ready,delayed1,delayed2,pending,waiting,suspend),all are individual Master DB's
{
    unsigned long uxPriority;

   for( uxPriority = ( unsigned long ) 0U; uxPriority < configMAX_PRIORITIES; uxPriority++ )//<8
   {
      vListInitialise( ( xList * ) &( pxReadyTasksLists[ uxPriority ] ) );
   }

   vListInitialise( ( xList * ) &xDelayedTaskList1 );
   vListInitialise( ( xList * ) &xDelayedTaskList2 );
   vListInitialise( ( xList * ) &xPendingReadyList );

   #if ( INCLUDE_vTaskDelete == 1 )
   {
      vListInitialise( ( xList * ) &xTasksWaitingTermination );
   }
   #endif /* INCLUDE_vTaskDelete */

   #if ( INCLUDE_vTaskSuspend == 1 )
   {
      vListInitialise( ( xList * ) &xSuspendedTaskList );
   }
   #endif /* INCLUDE_vTaskSuspend */

   /* Start with pxDelayedTaskList using list1 and the pxOverflowDelayedTaskList
   using list2. */
   pxDelayedTaskList = &xDelayedTaskList1;
   pxOverflowDelayedTaskList = &xDelayedTaskList2;
}
/*-----------------------------------------------------------*/

static void prvCheckTasksWaitingTermination( void )
{
   #if ( INCLUDE_vTaskDelete == 1 )
   {
      long xListIsEmpty;

      /* ucTasksDeleted is used to prevent vTaskSuspendAll() being called
      too often in the idle task. */
      while( uxTasksDeleted > ( unsigned long ) 0U )//gets incremented inside vTaskDelete() if called
      {
         vTaskSuspendAll();
            xListIsEmpty = listLIST_IS_EMPTY( &xTasksWaitingTermination );
         xTaskResumeAll();

         if( xListIsEmpty == pdFALSE )
         {
            tskTCB *pxTCB;

            taskENTER_CRITICAL();
            {
               pxTCB = ( tskTCB * ) listGET_OWNER_OF_HEAD_ENTRY( ( ( xList * ) &xTasksWaitingTermination ) );
               uxListRemove( &( pxTCB->xGenericListItem ) );
               --uxCurrentNumberOfTasks;
               --uxTasksDeleted;
            }
            taskEXIT_CRITICAL();

            prvDeleteTCB( pxTCB );
         }
      }
   }
   #endif /* vTaskDelete */
}
/*-----------------------------------------------------------*/
/*     Timetowake = timer tick + delay
   1.itemValue of the task is loaded with new recalculated wake up time
   2.if time to wake then insert the task fom already delayed DB ) to overflowdelayed DB...this just to prioritize
   3.if not yet i.e still we need to put in delayed DB from ready DB
   4.make a copy of the triggertime(wakeup time) to another variable which will be used in TaskIncrementTick
*/
static void prvAddCurrentTaskToDelayedList( portTickType xTimeToWake )
{
   /* The list item will be inserted in wake time order. */
   listSET_LIST_ITEM_VALUE( &( pxCurrentTCB->xGenericListItem ), xTimeToWake );//&( pxCurrentTCB->xGenericListItem )->ItemValue = xTimeToWake

   if( xTimeToWake < xTickCount )//xTimeToWake = xTickCount + xTicksToDelay 
   {
      /* Wake time has overflowed.  Place this item in the overflow list. */
      vListInsert( ( xList * ) pxOverflowDelayedTaskList, ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) );
   }
   else //waiting
   {
      /* The wake time has not overflowed, so we can use the current block list. */
      vListInsert( ( xList * ) pxDelayedTaskList, ( xListItem * ) &( pxCurrentTCB->xGenericListItem ) );//remember pxDelayedTaskList = xDelayedTaskList1

      /* If the task entering the blocked state was placed at the head of the
      list of blocked tasks then xNextTaskUnblockTime needs to be updated
      too. */
      if( xTimeToWake < xNextTaskUnblockTime )//initially xNextTaskUnblockTime = 0xffffffff
      {
         xNextTaskUnblockTime = xTimeToWake;//copy this with TimeToWake
      }
   }
}
/*-----------------------------------------------------------*/

static tskTCB *prvAllocateTCBAndStack( unsigned short usStackDepth, unsigned long *puxStackBuffer )
{
tskTCB *pxNewTCB;

   /* Allocate space for the TCB.  Where the memory comes from depends on
   the implementation of the port malloc function. */
   pxNewTCB = ( tskTCB * ) pvPortMalloc( sizeof( tskTCB ) );//somewhere around 72 bytes

   if( pxNewTCB != NULL )
   {
      /* Allocate space for the stack used by the task being created.
      The base of the stack memory stored in the TCB so the task can
      be deleted later if required. */
      pxNewTCB->pxStack = ( unsigned long * ) pvPortMallocAligned( ( ( ( size_t )usStackDepth ) * sizeof( unsigned long ) ), puxStackBuffer );//stackdepth is allocated

      if( pxNewTCB->pxStack == NULL )
      {
         /* Could not allocate the stack.  Delete the allocated TCB. */
         vPortFree( pxNewTCB );
         pxNewTCB = NULL;
      }
      else
      {
         /* Just to help debugging. */
         memset( pxNewTCB->pxStack, ( int ) tskSTACK_FILL_BYTE, ( size_t ) usStackDepth * sizeof( unsigned long ) );//the pxNewTCB->pxStack  i.e stack[usStackDepth] = 0xa5
      }
   }

   return pxNewTCB;
}
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   static void prvListTaskWithinSingleList( const signed char *pcWriteBuffer, xList *pxList, signed char cStatus )
   {
   volatile tskTCB *pxNextTCB, *pxFirstTCB;
   unsigned short usStackRemaining;
   PRIVILEGED_DATA static char pcStatusString[ configMAX_TASK_NAME_LEN + 30 ];

      /* Write the details of all the TCB's in pxList into the buffer. */
      listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
      do
      {
         listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );
         #if ( portSTACK_GROWTH > 0 )
         {
            usStackRemaining = usTaskCheckFreeStackSpace( ( unsigned char * ) pxNextTCB->pxEndOfStack );
         }
         #else
         {
            usStackRemaining = usTaskCheckFreeStackSpace( ( unsigned char * ) pxNextTCB->pxStack );
         }
         #endif

         sprintf( pcStatusString, ( char * ) "%s\t\t%c\t%u\t%u\t%u\r\n", pxNextTCB->pcTaskName, cStatus, ( unsigned int ) pxNextTCB->uxPriority, ( unsigned int ) usStackRemaining, ( unsigned int ) pxNextTCB->uxTCBNumber );
         strcat( ( char * ) pcWriteBuffer, ( char * ) pcStatusString );

      } while( pxNextTCB != pxFirstTCB );
   }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

#if ( configGENERATE_RUN_TIME_STATS == 1 )

   static void prvGenerateRunTimeStatsForTasksInList( const signed char *pcWriteBuffer, xList *pxList, unsigned long ulTotalRunTimeDiv100 )
   {
   volatile tskTCB *pxNextTCB, *pxFirstTCB;
   unsigned long ulStatsAsPercentage;

      /* Write the run time stats of all the TCB's in pxList into the buffer. */
      listGET_OWNER_OF_NEXT_ENTRY( pxFirstTCB, pxList );
      do
      {
         /* Get next TCB in from the list. */
         listGET_OWNER_OF_NEXT_ENTRY( pxNextTCB, pxList );

         /* Divide by zero check. */
         if( ulTotalRunTimeDiv100 > 0UL )
         {
            /* Has the task run at all? */
            if( pxNextTCB->ulRunTimeCounter == 0UL )
            {
               /* The task has used no CPU time at all. */
               sprintf( pcStatsString, ( char * ) "%s\t\t0\t\t0%%\r\n", pxNextTCB->pcTaskName );
            }
            else
            {
               /* What percentage of the total run time has the task used?
               This will always be rounded down to the nearest integer.
               ulTotalRunTimeDiv100 has already been divided by 100. */
               ulStatsAsPercentage = pxNextTCB->ulRunTimeCounter / ulTotalRunTimeDiv100;

               if( ulStatsAsPercentage > 0UL )
               {
                  #ifdef portLU_PRINTF_SPECIFIER_REQUIRED
                  {
                     sprintf( pcStatsString, ( char * ) "%s\t\t%lu\t\t%lu%%\r\n", pxNextTCB->pcTaskName, pxNextTCB->ulRunTimeCounter, ulStatsAsPercentage );
                  }
                  #else
                  {
                     /* sizeof( int ) == sizeof( long ) so a smaller
                     printf() library can be used. */
                     sprintf( pcStatsString, ( char * ) "%s\t\t%u\t\t%u%%\r\n", pxNextTCB->pcTaskName, ( unsigned int ) pxNextTCB->ulRunTimeCounter, ( unsigned int ) ulStatsAsPercentage );
                  }
                  #endif
               }
               else
               {
                  /* If the percentage is zero here then the task has
                  consumed less than 1% of the total run time. */
                  #ifdef portLU_PRINTF_SPECIFIER_REQUIRED
                  {
                     sprintf( pcStatsString, ( char * ) "%s\t\t%lu\t\t<1%%\r\n", pxNextTCB->pcTaskName, pxNextTCB->ulRunTimeCounter );
                  }
                  #else
                  {
                     /* sizeof( int ) == sizeof( long ) so a smaller
                     printf() library can be used. */
                     sprintf( pcStatsString, ( char * ) "%s\t\t%u\t\t<1%%\r\n", pxNextTCB->pcTaskName, ( unsigned int ) pxNextTCB->ulRunTimeCounter );
                  }
                  #endif
               }
            }

            strcat( ( char * ) pcWriteBuffer, ( char * ) pcStatsString );
         }

      } while( pxNextTCB != pxFirstTCB );
   }

#endif /* configGENERATE_RUN_TIME_STATS */
/*-----------------------------------------------------------*/

#if ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) )

   static unsigned short usTaskCheckFreeStackSpace( const unsigned char * pucStackByte )
   {
   register unsigned short usCount = 0U;

      while( *pucStackByte == tskSTACK_FILL_BYTE )
      {
         pucStackByte -= portSTACK_GROWTH;
         usCount++;
      }

      usCount /= sizeof( unsigned long );

      return usCount;
   }

#endif /* ( ( configUSE_TRACE_FACILITY == 1 ) || ( INCLUDE_uxTaskGetStackHighWaterMark == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetStackHighWaterMark == 1 )

   unsigned long uxTaskGetStackHighWaterMark( xTaskHandle xTask )
   {
   tskTCB *pxTCB;
   unsigned char *pcEndOfStack;
   unsigned long uxReturn;

      pxTCB = prvGetTCBFromHandle( xTask );

      #if portSTACK_GROWTH < 0
      {
         pcEndOfStack = ( unsigned char * ) pxTCB->pxStack;
      }
      #else
      {
         pcEndOfStack = ( unsigned char * ) pxTCB->pxEndOfStack;
      }
      #endif

      uxReturn = ( unsigned long ) usTaskCheckFreeStackSpace( pcEndOfStack );

      return uxReturn;
   }

#endif /* INCLUDE_uxTaskGetStackHighWaterMark */
/*-----------------------------------------------------------*/

#if ( INCLUDE_vTaskDelete == 1 )

   static void prvDeleteTCB( tskTCB *pxTCB )
   {
      /* This call is required specifically for the TriCore port.  It must be
      above the vPortFree() calls.  The call is also used by ports/demos that
      want to allocate and clean RAM statically. */
      portCLEAN_UP_TCB( pxTCB );

      /* Free up the memory allocated by the scheduler for the task.  It is up to
      the task to free any memory allocated at the application level. */
      vPortFreeAligned( pxTCB->pxStack );
      vPortFree( pxTCB );
   }

#endif /* INCLUDE_vTaskDelete */
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) )

   xTaskHandle xTaskGetCurrentTaskHandle( void )
   {
   xTaskHandle xReturn;

      /* A critical section is not required as this is not called from
      an interrupt and the current TCB will always be the same for any
      individual execution thread. */
      xReturn = pxCurrentTCB;

      return xReturn;
   }

#endif /* ( ( INCLUDE_xTaskGetCurrentTaskHandle == 1 ) || ( configUSE_MUTEXES == 1 ) ) */
/*-----------------------------------------------------------*/

#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )

   long xTaskGetSchedulerState( void )
   {
   long xReturn;

      if( xSchedulerRunning == pdFALSE )
      {
         xReturn = taskSCHEDULER_NOT_STARTED;
      }
      else
      {
         if( uxSchedulerSuspended == ( unsigned long ) pdFALSE )
         {
            xReturn = taskSCHEDULER_RUNNING;
         }
         else
         {
            xReturn = taskSCHEDULER_SUSPENDED;
         }
      }

      return xReturn;
   }

#endif /* ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) ) */
/*-----------------------------------------------------------*/
/*
pxCurrentTCB is the high priority task
pxMutexHolder is the low priority task holding the mutex
this function will reassign LP holder task from readyTaskList[old priority] to readyTaskList[new priority]
the priority of HP task will become the priority of LP task.
*/
#if ( configUSE_MUTEXES == 1 )

   void vTaskPriorityInherit( xTaskHandle * const pxMutexHolder )
   {
   tskTCB * const pxTCB = ( tskTCB * ) pxMutexHolder;

      /* If the mutex was given back by an interrupt while the queue was
      locked then the mutex holder might now be NULL. */
      if( pxMutexHolder != NULL )
      {
         if( pxTCB->uxPriority < pxCurrentTCB->uxPriority )//if current HP task priority > mutex holder LP task
         {
            /* Adjust the mutex holder state to account for its new priority. */
            listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), configMAX_PRIORITIES - ( portTickType ) pxCurrentTCB->uxPriority );//adjust the priority of LP task

            /* If the task being modified is in the ready state it will need to be moved into a new list. */
            if( listIS_CONTAINED_WITHIN( &( pxReadyTasksLists[ pxTCB->uxPriority ] ), &( pxTCB->xGenericListItem ) ) != pdFALSE )//If the TCB holder task is currently in readyTaskList[old priority]
            {
               if( uxListRemove( ( xListItem * ) &( pxTCB->xGenericListItem ) ) == 0 )//remove the current TCB from the readyTaskList[old priority]
               {
                  taskRESET_READY_PRIORITY( pxTCB->uxPriority );
               }

               /* Inherit the priority before being moved into the new list. */
               pxTCB->uxPriority = pxCurrentTCB->uxPriority;//new priority of the TCB holder LP task is not changed to HP task
               prvAddTaskToReadyQueue( pxTCB );//re add this to the readyTaskList[new priority]
            }
            else
            {
               /* Just inherit the priority. */
               pxTCB->uxPriority = pxCurrentTCB->uxPriority;
            }

            traceTASK_PRIORITY_INHERIT( pxTCB, pxCurrentTCB->uxPriority );
         }
      }
   }

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/
/*
  ReadyList[oldpriority] => ReadyList[BasePriority]
  same way eventlist and generic list priority is reduced
*/
#if ( configUSE_MUTEXES == 1 )

   void vTaskPriorityDisinherit( xTaskHandle * const pxMutexHolder )
   {
   tskTCB * const pxTCB = ( tskTCB * ) pxMutexHolder;

      if( pxMutexHolder != NULL )
      {
         if( pxTCB->uxPriority != pxTCB->uxBasePriority )//if the mutex holder i.e the task is having higher priority than default
         {
            /* We must be the running task to be able to give the mutex back.
            Remove ourselves from the ready list we currently appear in. */
            if( uxListRemove( ( xListItem * ) &( pxTCB->xGenericListItem ) ) == 0 )
            {
               taskRESET_READY_PRIORITY( pxTCB->uxPriority );
            }

            /* Disinherit the priority before adding the task into the new
            ready list. */
            traceTASK_PRIORITY_DISINHERIT( pxTCB, pxTCB->uxBasePriority );
            pxTCB->uxPriority = pxTCB->uxBasePriority;
            listSET_LIST_ITEM_VALUE( &( pxTCB->xEventListItem ), configMAX_PRIORITIES - ( portTickType ) pxTCB->uxPriority );
            prvAddTaskToReadyQueue( pxTCB );
         }
      }
   }

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

#if ( portCRITICAL_NESTING_IN_TCB == 1 )

   void vTaskEnterCritical( void )
   {
      portDISABLE_INTERRUPTS();

      if( xSchedulerRunning != pdFALSE )
      {
         ( pxCurrentTCB->uxCriticalNesting )++;
      }
   }

#endif /* portCRITICAL_NESTING_IN_TCB */
/*-----------------------------------------------------------*/

#if ( portCRITICAL_NESTING_IN_TCB == 1 )

   void vTaskExitCritical( void )
   {
      if( xSchedulerRunning != pdFALSE )
      {
         if( pxCurrentTCB->uxCriticalNesting > 0U )
         {
            ( pxCurrentTCB->uxCriticalNesting )--;

            if( pxCurrentTCB->uxCriticalNesting == 0U )
            {
               portENABLE_INTERRUPTS();
            }
         }
      }
   }

#endif /* portCRITICAL_NESTING_IN_TCB */
/*-----------------------------------------------------------*/


/*
 * Macro that looks at the list of tasks that are currently delayed to see if
 * any require waking.
 *
 * Tasks are stored in the queue in the order of their wake time - meaning
 * once one tasks has been found whose timer has not expired we need not look
 * any further down the list.
 */
static void prvCheckDelayedTasks(void)                                             
{                                                                  
   tskTCB *pxTCB;
   portTickType xItemValue;                                                
                                                                  
   /* Is the tick count greater than or equal to the wake time of the first         
   task referenced from the delayed tasks list? */                              /*xTickCount++ is done in TaskIncrementTick*/
   if( xTickCount >= xNextTaskUnblockTime )                                 /*xNextTaskUnblockTime gets updated with xTimeToWake in  prvAddCurrentTaskToDelayedList*/
   {                                                               
      for( ;; )                                                      
      {                                                            
         if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )                  /*number of items ? in delayedtasklist*/
         {                                                         
            /* The delayed list is empty.  Set xNextTaskUnblockTime to the         
            maximum possible value so it is extremely unlikely that the            
            if( xTickCount >= xNextTaskUnblockTime ) test will pass next         
            time through. */                                          
            xNextTaskUnblockTime = portMAX_DELAY;                           
            break;                                                   
         }                                                         
         else                                                      /*task item exists in delayedtasklist*/
         {                                                         
            /* The delayed list is not empty, get the value of the item at         
            the head of the delayed list.  This is the time at which the         
            task at the head of the delayed list should be removed from            
            the Blocked state. */                                       
            pxTCB = ( tskTCB * ) listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList );    /*as stated the pvOwner points to the task TCB's address*/ 
            xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xGenericListItem ) );    /*get the itemValue for timeout check*/ 
                                                                  
            if( xTickCount < xItemValue )                                  /*new 1ms timestamp <  xItemValue  = old 1ms timestamp + xTicksToDelay*/
            {                                                      
               /* It is not time to unblock this item yet, but the item         
               value is the time at which the task at the head of the            
               blocked list should be removed from the Blocked state -            
               so record the item value in xNextTaskUnblockTime. */            
               xNextTaskUnblockTime = xItemValue;                           
               break;                                                
            }                                                      
                                                                  
            /* It is time to remove the item from the Blocked state. */            
            uxListRemove( &( pxTCB->xGenericListItem ) );                     /*task which is associaed with genericlistiem is removed from the container(delayed Master DB)*/
                                                                  
            /* Is the task waiting on an event also? */                        
            if( pxTCB->xEventListItem.pvContainer != NULL )                     
            {                                                      
               uxListRemove( &( pxTCB->xEventListItem ) );                     
            }                                                      
            prvAddTaskToReadyQueue( pxTCB );                              /*the task that was removed from delayed is now put into reader (container pointer change)*/
         }                                                         
      }                                                            
   }                                                               
}





