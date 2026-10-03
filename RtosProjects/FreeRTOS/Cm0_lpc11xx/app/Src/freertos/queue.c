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

#include <stdlib.h>
#include <string.h>

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#if ( configUSE_CO_ROUTINES == 1 )
   #include "croutine.h"
#endif

#undef MPU_WRAPPERS_INCLUDED_FROM_API_FILE

/* Constants used with the cRxLock and xTxLock structure members. */
#define queueUNLOCKED               ( ( signed portBASE_TYPE ) -1 )
#define queueLOCKED_UNMODIFIED         ( ( signed portBASE_TYPE ) 0 )

#define queueERRONEOUS_UNBLOCK         ( -1 )

/* Effectively make a union out of the xQUEUE structure. */
#define pxMutexHolder               pcTail
#define uxQueueType                  pcHead
#define uxRecursiveCallCount         pcReadFrom
#define queueQUEUE_IS_MUTEX            NULL

/* Semaphores do not actually store or copy data, so have an items size of
zero. */
#define queueSEMAPHORE_QUEUE_ITEM_LENGTH ( ( unsigned portBASE_TYPE ) 0 )
#define queueDONT_BLOCK                ( ( portTickType ) 0U )
#define queueMUTEX_GIVE_BLOCK_TIME       ( ( portTickType ) 0U )


/*
 * Definition of the queue used by the scheduler.
 * Items are queued by copy, not reference.
 */
/*
=====================================================================================
                             CORRECTED QUEUE MEMORY MAP
=====================================================================================
===================================================================================
                         INITIAL EMPTY QUEUE MEMORY MAP
===================================================================================

       [pcHead]
       [pcWriteTo]
           ¦
           v
+-------------------------------------------------------------------------------+
¦  SLOT 0 (Empty)  ¦  SLOT 1 (Empty)  ¦  SLOT 2 (Empty)  ¦ EXTRA SENTINEL (1B)  ¦
+-------------------------------------------------------------------------------+
   ^                                         ^                      ^
   ¦                                         ¦                      ¦
START OF BUFFER                         [pcReadFrom]             [pcTail]
(The very first byte)                  (Points to the          (The very last byte
                                        end boundary            of the buffer)
                                        of the data slots)

pcHead & pcWriteTo : Both sit exactly at the front edge of Slot 0. When you send data, it drops right into Slot 0, and pcWriteTo moves forward to Slot 1
pcReadFrom         : Sits at the back edge of Slot 2. When a task tries to read, FreeRTOS runs a math check that advances this pointer first. 
                     Moving forward from the end of Slot 2 wraps it perfectly back around to the front of Slot 0.
pcTail             : Sits at the absolute end of the buffer, marking the boundary line.

i think the size should be 76 bytes
*/
typedef struct QueueDefinition
{
   signed char *pcHead;               /*< Points to the beginning of the queue storage area. */
   signed char *pcTail;               /*< Points to the byte at the end of the queue storage area.  Once more byte is allocated than necessary to store the queue items, this is used as a marker. */

   signed char *pcWriteTo;               /*< Points to the free next place in the storage area. */
   signed char *pcReadFrom;            /*< Points to the last place that a queued item was read from. */

   xList xTasksWaitingToSend;            /*< List of tasks that are blocked waiting to post onto this queue.  Stored in priority order. */
   xList xTasksWaitingToReceive;         /*< List of tasks that are blocked waiting to read from this queue.  Stored in priority order. */

   volatile unsigned portBASE_TYPE uxMessagesWaiting;/*< The number of items currently in the queue. */
   unsigned portBASE_TYPE uxLength;      /*< The length of the queue defined as the number of items it will hold, not the number of bytes. */
   unsigned portBASE_TYPE uxItemSize;      /*< The size of each items that the queue will hold. */

   volatile signed portBASE_TYPE xRxLock;   /*< Stores the number of items received from the queue (removed from the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */
   volatile signed portBASE_TYPE xTxLock;   /*< Stores the number of items transmitted to the queue (added to the queue) while the queue was locked.  Set to queueUNLOCKED when the queue is not locked. */

   #if ( configUSE_TRACE_FACILITY == 1 )
      unsigned char ucQueueNumber;
      unsigned char ucQueueType;
   #endif

   #if ( configUSE_QUEUE_SETS == 1 )
      struct QueueDefinition *pxQueueSetContainer;
   #endif

} xQUEUE;
/*-----------------------------------------------------------*/

/*
 * The queue registry is just a means for kernel aware debuggers to locate
 * queue structures.  It has no other purpose so is an optional component.
 */
#if ( configQUEUE_REGISTRY_SIZE > 0 )

   /* The type stored within the queue registry array.  This allows a name
   to be assigned to each queue making kernel aware debugging a little
   more user friendly. */
   typedef struct QUEUE_REGISTRY_ITEM
   {
      signed char *pcQueueName;
      xQueueHandle xHandle;
   } xQueueRegistryItem;

   /* The queue registry is simply an array of xQueueRegistryItem structures.
   The pcQueueName member of a structure being NULL is indicative of the
   array position being vacant. */
   xQueueRegistryItem xQueueRegistry[ configQUEUE_REGISTRY_SIZE ];

   /* Removes a queue from the registry by simply setting the pcQueueName
   member to NULL. */
   static void prvQueueUnregisterQueue( xQueueHandle xQueue ) PRIVILEGED_FUNCTION;

#endif /* configQUEUE_REGISTRY_SIZE */

/*
 * Unlocks a queue locked by a call to prvLockQueue.  Locking a queue does not
 * prevent an ISR from adding or removing items to the queue, but does prevent
 * an ISR from removing tasks from the queue event lists.  If an ISR finds a
 * queue is locked it will instead increment the appropriate queue lock count
 * to indicate that a task may require unblocking.  When the queue in unlocked
 * these lock counts are inspected, and the appropriate action taken.
 */
static void prvUnlockQueue( xQUEUE *pxQueue ) PRIVILEGED_FUNCTION;

/*
 * Uses a critical section to determine if there is any data in a queue.
 *
 * @return pdTRUE if the queue contains no items, otherwise pdFALSE.
 */
static signed portBASE_TYPE prvIsQueueEmpty( const xQUEUE *pxQueue ) PRIVILEGED_FUNCTION;

/*
 * Uses a critical section to determine if there is any space in a queue.
 *
 * @return pdTRUE if there is no space, otherwise pdFALSE;
 */
static signed portBASE_TYPE prvIsQueueFull( const xQUEUE *pxQueue ) PRIVILEGED_FUNCTION;

/*
 * Copies an item into the queue, either at the front of the queue or the
 * back of the queue.
 */
static void prvCopyDataToQueue( xQUEUE *pxQueue, const void *pvItemToQueue, portBASE_TYPE xPosition ) PRIVILEGED_FUNCTION;

/*
 * Copies an item out of a queue.
 */
static void prvCopyDataFromQueue( xQUEUE * const pxQueue, const void *pvBuffer ) PRIVILEGED_FUNCTION;

#if ( configUSE_QUEUE_SETS == 1 )
   /*
    * Checks to see if a queue is a member of a queue set, and if so, notifies
    * the queue set that the queue contains data.
    */
   static portBASE_TYPE prvNotifyQueueSetContainer( xQUEUE *pxQueue, portBASE_TYPE xCopyPosition );
#endif

/*-----------------------------------------------------------*/

/*
 * Macro to mark a queue as locked.  Locking a queue prevents an ISR from
 * accessing the queue event lists.
 */
#define prvLockQueue( pxQueue )                        \
   taskENTER_CRITICAL();                           \
   {                                          \
      if( ( pxQueue )->xRxLock == queueUNLOCKED )         \
      {                                       \
         ( pxQueue )->xRxLock = queueLOCKED_UNMODIFIED;   \
      }                                       \
      if( ( pxQueue )->xTxLock == queueUNLOCKED )         \
      {                                       \
         ( pxQueue )->xTxLock = queueLOCKED_UNMODIFIED;   \
      }                                       \
   }                                          \
   taskEXIT_CRITICAL()
/*-----------------------------------------------------------*/

portBASE_TYPE xQueueGenericReset( xQueueHandle xQueue, portBASE_TYPE xNewQueue )
{
    xQUEUE *pxQueue;

   pxQueue = ( xQUEUE * ) xQueue;
   configASSERT( pxQueue );

   taskENTER_CRITICAL();
   {
      pxQueue->pcTail = pxQueue->pcHead + ( pxQueue->uxLength * pxQueue->uxItemSize );//say ex: pcTail =  pcHead + (5*4)
      pxQueue->uxMessagesWaiting = ( unsigned portBASE_TYPE ) 0U;
      pxQueue->pcWriteTo = pxQueue->pcHead;//next item will drop into slot 0
      pxQueue->pcReadFrom = pxQueue->pcHead + ( ( pxQueue->uxLength - ( unsigned portBASE_TYPE ) 1U ) * pxQueue->uxItemSize );//pcHead + (4*4)
      pxQueue->xRxLock = queueUNLOCKED;
      pxQueue->xTxLock = queueUNLOCKED;

      if( xNewQueue == pdFALSE )
      {
         /* If there are tasks blocked waiting to read from the queue, then the tasks will remain blocked as after this function exits the queue
         will still be empty.  If there are tasks blocked waiting to   write to  the queue, then one should be unblocked as after this function exits
         it will be possible to write to it. */

         if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )//if xTasksWaitingToSend DB list has something
         {
            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) == pdTRUE )//remove it from the EventList
            {
               portYIELD_WITHIN_API();//do a context switch afterwards
            }
         }
      }
      else
      {
         /* Ensure the event queues start in the correct state. */
         vListInitialise( &( pxQueue->xTasksWaitingToSend ) );//make all pxIndex,pxNext,PxPrev etc point to ListEnd
         vListInitialise( &( pxQueue->xTasksWaitingToReceive ) );//make all pxIndex,pxNext,PxPrev etc point to ListEnd
      }
   }
   taskEXIT_CRITICAL();

   /* A value is returned for calling semantic consistency with previous
   versions. */
   return pdPASS;
}
/*-----------------------------------------------------------*/
/*
         Example:- STORAGE BUFFER (21 Bytes Total)
         =========================================
         
         Slot 0      Slot 1      Slot 2      Slot 3      Slot 4      Sentinel
        [4 Bytes]   [4 Bytes]   [4 Bytes]   [4 Bytes]   [4 Bytes]    [1 Byte]
       +-----------+-----------+-----------+-----------+-----------+---+
Bytes: | 0 1 2 3   | 4 5 6 7   | 8 9 10 11 | 12 13 14 15| 16 17 18 19| 20|
       +-----------+-----------+-----------+-----------+-----------+---+
         ^                                               ^           ^

         |                                               |           |
         +-- pcHead                                      |           +-- pcTail

         |                                               |
         +-- pcWriteTo                                   +-- pcReadFrom

*/
xQueueHandle xQueueGenericCreate( unsigned portBASE_TYPE uxQueueLength, 
                                  unsigned portBASE_TYPE uxItemSize, 
                                  unsigned char ucQueueType )
{
   xQUEUE *pxNewQueue;
   size_t xQueueSizeInBytes;
   xQueueHandle xReturn = NULL;

   /* Remove compiler warnings about unused parameters should
   configUSE_TRACE_FACILITY not be set to 1. */
   ( void ) ucQueueType;

   /* Allocate the new queue structure. */
   if( uxQueueLength > ( unsigned portBASE_TYPE ) 0 )//if Q needs to be created
   {
      pxNewQueue = ( xQUEUE * ) pvPortMalloc( sizeof( xQUEUE ) );//i thnk it should be 76bytes
      if( pxNewQueue != NULL )
      {
         /* Create the list of pointers to queue items.  The queue is one byte
         longer than asked for to make wrap checking easier/faster (i.e sentinel byte */
         xQueueSizeInBytes = ( size_t ) ( uxQueueLength * uxItemSize ) + ( size_t ) 1;

         pxNewQueue->pcHead = ( signed char * ) pvPortMalloc( xQueueSizeInBytes );
         if( pxNewQueue->pcHead != NULL )
         {
            /* Initialise the queue members as described above where the
            queue type is defined. */
            pxNewQueue->uxLength = uxQueueLength;
            pxNewQueue->uxItemSize = uxItemSize;
            xQueueGenericReset( pxNewQueue, pdTRUE );

            #if ( configUSE_TRACE_FACILITY == 1 )
            {
               pxNewQueue->ucQueueType = ucQueueType;
            }
            #endif /* configUSE_TRACE_FACILITY */

            #if( configUSE_QUEUE_SETS == 1 )
            {
               pxNewQueue->pxQueueSetContainer = NULL;
            }
            #endif /* configUSE_QUEUE_SETS */

            traceQUEUE_CREATE( pxNewQueue );
            xReturn = pxNewQueue;
         }
         else
         {
            traceQUEUE_CREATE_FAILED( ucQueueType );
            vPortFree( pxNewQueue );
         }
      }
   }

   configASSERT( xReturn );

   return xReturn;
}
/*-----------------------------------------------------------*/

#if ( configUSE_MUTEXES == 1 )

   xQueueHandle xQueueCreateMutex( unsigned char ucQueueType )
   {
   xQUEUE *pxNewQueue;

      /* Prevent compiler warnings about unused parameters if
      configUSE_TRACE_FACILITY does not equal 1. */
      ( void ) ucQueueType;

      /* Allocate the new queue structure. */
      pxNewQueue = ( xQUEUE * ) pvPortMalloc( sizeof( xQUEUE ) );
      if( pxNewQueue != NULL )
      {
         /* Information required for priority inheritance. */
         pxNewQueue->pxMutexHolder = NULL;
         pxNewQueue->uxQueueType = queueQUEUE_IS_MUTEX;

         /* Queues used as a mutex no data is actually copied into or out
         of the queue. */
         pxNewQueue->pcWriteTo = NULL;
         pxNewQueue->pcReadFrom = NULL;

         /* Each mutex has a length of 1 (like a binary semaphore) and
         an item size of 0 as nothing is actually copied into or out
         of the mutex. */
         pxNewQueue->uxMessagesWaiting = ( unsigned portBASE_TYPE ) 0U;
         pxNewQueue->uxLength = ( unsigned portBASE_TYPE ) 1U;
         pxNewQueue->uxItemSize = ( unsigned portBASE_TYPE ) 0U;
         pxNewQueue->xRxLock = queueUNLOCKED;
         pxNewQueue->xTxLock = queueUNLOCKED;

         #if ( configUSE_TRACE_FACILITY == 1 )
         {
            pxNewQueue->ucQueueType = ucQueueType;
         }
         #endif

         #if ( configUSE_QUEUE_SETS == 1 )
         {
            pxNewQueue->pxQueueSetContainer = NULL;
         }
         #endif

         /* Ensure the event queues start with the correct state. */
         vListInitialise( &( pxNewQueue->xTasksWaitingToSend ) );
         vListInitialise( &( pxNewQueue->xTasksWaitingToReceive ) );

         traceCREATE_MUTEX( pxNewQueue );

         /* Start with the semaphore in the expected state. */
         xQueueGenericSend( pxNewQueue, NULL, ( portTickType ) 0U, queueSEND_TO_BACK );
      }
      else
      {
         traceCREATE_MUTEX_FAILED();
      }

      configASSERT( pxNewQueue );
      return pxNewQueue;
   }

#endif /* configUSE_MUTEXES */
/*-----------------------------------------------------------*/

#if ( ( configUSE_MUTEXES == 1 ) && ( INCLUDE_xSemaphoreGetMutexHolder == 1 ) )

   void* xQueueGetMutexHolder( xQueueHandle xSemaphore )
   {
   void *pxReturn;

      /* This function is called by xSemaphoreGetMutexHolder(), and should not
      be called directly.  Note:  This is is a good way of determining if the
      calling task is the mutex holder, but not a good way of determining the
      identity of the mutex holder, as the holder may change between the
      following critical section exiting and the function returning. */
      taskENTER_CRITICAL();
      {
         if( ( ( xQUEUE * ) xSemaphore )->uxQueueType == queueQUEUE_IS_MUTEX )
         {
            pxReturn = ( void * ) ( ( xQUEUE * ) xSemaphore )->pxMutexHolder;
         }
         else
         {
            pxReturn = NULL;
         }
      }
      taskEXIT_CRITICAL();

      return pxReturn;
   }

#endif
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )

   portBASE_TYPE xQueueGiveMutexRecursive( xQueueHandle xMutex )
   {
   portBASE_TYPE xReturn;
   xQUEUE *pxMutex;

      pxMutex = ( xQUEUE * ) xMutex;
      configASSERT( pxMutex );

      /* If this is the task that holds the mutex then pxMutexHolder will not
      change outside of this task.  If this task does not hold the mutex then
      pxMutexHolder can never coincidentally equal the tasks handle, and as
      this is the only condition we are interested in it does not matter if
      pxMutexHolder is accessed simultaneously by another task.  Therefore no
      mutual exclusion is required to test the pxMutexHolder variable. */
      if( pxMutex->pxMutexHolder == xTaskGetCurrentTaskHandle() )
      {
         traceGIVE_MUTEX_RECURSIVE( pxMutex );

         /* uxRecursiveCallCount cannot be zero if pxMutexHolder is equal to
         the task handle, therefore no underflow check is required.  Also,
         uxRecursiveCallCount is only modified by the mutex holder, and as
         there can only be one, no mutual exclusion is required to modify the
         uxRecursiveCallCount member. */
         ( pxMutex->uxRecursiveCallCount )--;

         /* Have we unwound the call count? */
         if( pxMutex->uxRecursiveCallCount == 0 )
         {
            /* Return the mutex.  This will automatically unblock any other
            task that might be waiting to access the mutex. */
            xQueueGenericSend( pxMutex, NULL, queueMUTEX_GIVE_BLOCK_TIME, queueSEND_TO_BACK );
         }

         xReturn = pdPASS;
      }
      else
      {
         /* We cannot give the mutex because we are not the holder. */
         xReturn = pdFAIL;

         traceGIVE_MUTEX_RECURSIVE_FAILED( pxMutex );
      }

      return xReturn;
   }

#endif /* configUSE_RECURSIVE_MUTEXES */
/*-----------------------------------------------------------*/

#if ( configUSE_RECURSIVE_MUTEXES == 1 )

   portBASE_TYPE xQueueTakeMutexRecursive( xQueueHandle xMutex, portTickType xBlockTime )
   {
   portBASE_TYPE xReturn;
   xQUEUE *pxMutex;

      pxMutex = ( xQUEUE * ) xMutex;
      configASSERT( pxMutex );

      /* Comments regarding mutual exclusion as per those within
      xQueueGiveMutexRecursive(). */

      traceTAKE_MUTEX_RECURSIVE( pxMutex );

      if( pxMutex->pxMutexHolder == xTaskGetCurrentTaskHandle() )
      {
         ( pxMutex->uxRecursiveCallCount )++;
         xReturn = pdPASS;
      }
      else
      {
         xReturn = xQueueGenericReceive( pxMutex, NULL, xBlockTime, pdFALSE );

         /* pdPASS will only be returned if we successfully obtained the mutex,
         we may have blocked to reach here. */
         if( xReturn == pdPASS )
         {
            ( pxMutex->uxRecursiveCallCount )++;
         }
         else
         {
            traceTAKE_MUTEX_RECURSIVE_FAILED( pxMutex );
         }
      }

      return xReturn;
   }

#endif /* configUSE_RECURSIVE_MUTEXES */
/*-----------------------------------------------------------*/

#if ( configUSE_COUNTING_SEMAPHORES == 1 )

   xQueueHandle xQueueCreateCountingSemaphore( unsigned portBASE_TYPE uxCountValue, unsigned portBASE_TYPE uxInitialCount )
   {
   xQueueHandle xHandle;

      xHandle = xQueueGenericCreate( ( unsigned portBASE_TYPE ) uxCountValue, queueSEMAPHORE_QUEUE_ITEM_LENGTH, queueQUEUE_TYPE_COUNTING_SEMAPHORE );

      if( xHandle != NULL )
      {
         ( ( xQUEUE * ) xHandle )->uxMessagesWaiting = uxInitialCount;

         traceCREATE_COUNTING_SEMAPHORE();
      }
      else
      {
         traceCREATE_COUNTING_SEMAPHORE_FAILED();
      }

      configASSERT( xHandle );
      return xHandle;
   }

#endif /* configUSE_COUNTING_SEMAPHORES */
/*-----------------------------------------------------------*/
/*
    1.Add the ItemtoQueue(data we need to send) if there is room in Q defined by xQueue
      Check if any task is waiting to receive/read this data,if yes the remove (Event and Generic List) out of xTasksWaitingToReceive:-
      if shceduler is not suspended,remove the genericlistitem also from xTasksWaitingToReceive DB 
      if scheduler is suspended,add xEventListItem in xPendingReadyList DB 
    2.If the queue is full and xTicksToWait is 0, exit the critical section immediately and return errQUEUE_FULL.
      If the queue is full and a non-zero timeout is specified, capture the current system tick count as a baseline snapshot (vTaskSetTimeOutState).
      a.make xTxLock = xRxLock = 0
      b.Check if the timeout expired else recapture the adjusted time back
         a.IsQ full pxQueue->uxMessagesWaiting == pxQueue->uxLength????
           then just like vtaskdelay configure pxQueue->xTasksWaitingToSend with 
           if xTimeToTicks is not portMAX_DELAY , then put in pxOverflowDelayedTaskList or pxDelayedTaskList list depending if xTicks overshot or not
           if xTimeToTicks is portMAX_DELAY , then put in xSuspendedTaskList
    3.Unlock Q
       if pxQueue->xTxLock is 0? then unlock it
       else 
           if there are items in xTasksWaitingToReceive Q then add add EventList and GenericList to xPendingReadyList if scheduler is suspended 
           else put in pxReadyTasksLists
           we repeat this until its unlocked meaning xTxLock = 0
     
       if pxQueue->xRxLock is 0? then unlock it 
       else
           if there are items in xTasksWaitingToSend Q then add EventList and GenericList to xPendingReadyList if scheduler is suspended 
           else put in pxReadyTasksLists
           we repeat this until its unlocked meaning xRxLock = 0
*/


signed portBASE_TYPE xQueueGenericSend( xQueueHandle xQueue, 
                                        const void * const pvItemToQueue, //the variable that we need to send
                                        portTickType xTicksToWait, 
                                        portBASE_TYPE xCopyPosition )//queueSEND_TO_BACK = 0,queueSEND_TO_FRONT = 1 
{
    signed portBASE_TYPE xEntryTimeSet = pdFALSE;
    xTimeOutType xTimeOut;
    xQUEUE *pxQueue;

   pxQueue = ( xQUEUE * ) xQueue;
   configASSERT( pxQueue );
   configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

   /* This function relaxes the coding standard somewhat to allow return
   statements within the function itself.  This is done in the interest
   of execution time efficiency. */
   for( ;; )
   {
      taskENTER_CRITICAL();//cpsid i i.e i bit is set
      {
         /* Is there room on the queue now?  To be running we must be the highest priority task wanting to access the queue. */
         if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )//say 0 < 5 for example
         {
            traceQUEUE_SEND( pxQueue );
            //pcWriteTo = pvItemToQueue[itemsize] or pcReadfrom = pvItemToQueue[itemsize]
            prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );//depending on sentoBACK/FRONT,the readFrom and WriteTo is copied

            #if ( configUSE_QUEUE_SETS == 1 )
            {
               if( pxQueue->pxQueueSetContainer != NULL )
               {
                  if( prvNotifyQueueSetContainer( pxQueue, xCopyPosition ) == pdTRUE )
                  {
                     /* The queue is a member of a queue set, and posting
                     to the queue set caused a higher priority task to
                     unblock. A context switch is required. */
                     portYIELD_WITHIN_API();
                  }
               }
               else
               {
                  /* If there was a task waiting for data to arrive on the queue then unblock it now. */
                  if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                  {
                     if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) == pdTRUE )
                     {
                        /* The unblocked task has a priority higher than
                        our own so yield immediately.  Yes it is ok to
                        do this from within the critical section - the
                        kernel takes care of that. */
                        portYIELD_WITHIN_API();
                     }
                  }
               }
            }
            #else /* configUSE_QUEUE_SETS */
            {   //Q is not full so if a task is waiting to read data, relieve it immediately
               /* If there was a task waiting for data to arrive on the queue then unblock it now. */
               if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )//if numberofitems is not 0
               {
                        //send the master DB list i.e xTasksWaitingToReceive DB and remove the TCB from that list
                  if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) == pdTRUE )//remove the genericlistitem also from xTasksWaitingToReceive DB if shceduler is not suspended
                  {                                                                               //add xEventListItem in xPendingReadyList DB if scheduler is suspended
                     /* The unblocked task has a priority higher than
                     our own so yield immediately.  Yes it is ok to do
                     this from within the critical section - the kernel
                     takes care of that. */
                     portYIELD_WITHIN_API();
                  }
               }
            }
            #endif /* configUSE_QUEUE_SETS */

            taskEXIT_CRITICAL();//enable interrupts

            /* Return to the original privilege level before exiting the function. */
            return pdPASS;
         }
         else//the Q is full, task is blocked from writing
         {
            if( xTicksToWait == ( portTickType ) 0 )
            {
               /* The queue was full and no block time is specified (or
               the block time has expired) so leave now. */
               taskEXIT_CRITICAL();

               /* Return to the original privilege level before exiting
               the function. */
               traceQUEUE_SEND_FAILED( pxQueue );
               return errQUEUE_FULL;
            }
            else if( xEntryTimeSet == pdFALSE )
            {
               /* The queue was full and a block time was specified so
               configure the timeout structure. */
               vTaskSetTimeOutState( &xTimeOut );
               xEntryTimeSet = pdTRUE;
            }
         }
      }
      taskEXIT_CRITICAL();

      /* Interrupts and other tasks can send to and receive from the queue
      now the critical section has been exited. */

      vTaskSuspendAll();
      prvLockQueue( pxQueue );/*xTxLock = xRxLock = 0 , locks the queue. If an interrupt fires right now and tries to read from the queue 
                                  which would free up a slot, it is blocked from modifying the task lists. Instead, 
                                  it will just increment the queue's xRxLock counter.*/

      /* Update the timeout state to see if it has expired yet. */
      if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )//did the example time of 1s that we want to wait expired
      {
         if( prvIsQueueFull( pxQueue ) != pdFALSE )//Q full pxQueue->uxMessagesWaiting == pxQueue->uxLength????
         {
            traceBLOCKING_ON_QUEUE_SEND( pxQueue );
            /* depending on the TickstoWait ,it will enter 
                    xSuspendedTaskList if xTicksToWait is portMAX_DELAY
                    pxOverflowDelayedTaskList if xticks overshot xTicksToWait
                    pxDelayedTaskList if xTicks waiting to hit timeout
                 */
                /*   All tasks waiting to receive data will :-
                     place EventList and GenericList in xSuspendedTaskList if xTicksToWait is portMAX_DELAY (indefinite)
                                         pxOverflowDelayedTaskList if xticks overshot xTicksToWait 
                                         pxDelayedTaskList if xTicks waiting to hit timeout
            */
            vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToSend ), xTicksToWait );

            /* Unlocking the queue means queue events can effect the
            event list.  It is possible   that interrupts occurring now
            remove this task from the event   list again - but as the
            scheduler is suspended the task will go onto the pending
            ready last instead of the actual ready list. */
            prvUnlockQueue( pxQueue );

            /* Resuming the scheduler will move tasks from the pending
            ready list into the ready list - so it is feasible that this
            task is already in a ready list before it yields - in which
            case the yield will not cause a context switch unless there
            is also a higher priority task in the pending ready list. */
            if( xTaskResumeAll() == pdFALSE )
            {
               portYIELD_WITHIN_API();
            }
         }
         else//no Q not full
         {
            /* Try again. */
            prvUnlockQueue( pxQueue );
            ( void ) xTaskResumeAll();
         }
      }
      else
      {
         /* The timeout has expired. */
         prvUnlockQueue( pxQueue );
         ( void ) xTaskResumeAll();

         /* Return to the original privilege level before exiting the
         function. */
         traceQUEUE_SEND_FAILED( pxQueue );
         return errQUEUE_FULL;
      }
   }
}
/*-----------------------------------------------------------*/

#if ( configUSE_ALTERNATIVE_API == 1 )

   signed portBASE_TYPE xQueueAltGenericSend( xQueueHandle xQueue, const void * const pvItemToQueue, portTickType xTicksToWait, portBASE_TYPE xCopyPosition )
   {
   signed portBASE_TYPE xEntryTimeSet = pdFALSE;
   xTimeOutType xTimeOut;
   xQUEUE *pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;
      configASSERT( pxQueue );
      configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

      for( ;; )
      {
         taskENTER_CRITICAL();
         {
            /* Is there room on the queue now?  To be running we must be
            the highest priority task wanting to access the queue. */
            if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
            {
               traceQUEUE_SEND( pxQueue );
               prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

               /* If there was a task waiting for data to arrive on the
               queue then unblock it now. */
               if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
               {
                  if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) == pdTRUE )
                  {
                     /* The unblocked task has a priority higher than
                     our own so yield immediately. */
                     portYIELD_WITHIN_API();
                  }
               }

               taskEXIT_CRITICAL();
               return pdPASS;
            }
            else
            {
               if( xTicksToWait == ( portTickType ) 0 )
               {
                  taskEXIT_CRITICAL();
                  return errQUEUE_FULL;
               }
               else if( xEntryTimeSet == pdFALSE )
               {
                  vTaskSetTimeOutState( &xTimeOut );
                  xEntryTimeSet = pdTRUE;
               }
            }
         }
         taskEXIT_CRITICAL();

         taskENTER_CRITICAL();
         {
            if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
            {
               if( prvIsQueueFull( pxQueue ) != pdFALSE )
               {
                  traceBLOCKING_ON_QUEUE_SEND( pxQueue );
                  vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToSend ), xTicksToWait );
                  portYIELD_WITHIN_API();
               }
            }
            else
            {
               taskEXIT_CRITICAL();
               traceQUEUE_SEND_FAILED( pxQueue );
               return errQUEUE_FULL;
            }
         }
         taskEXIT_CRITICAL();
      }
   }

#endif /* configUSE_ALTERNATIVE_API */
/*-----------------------------------------------------------*/

#if ( configUSE_ALTERNATIVE_API == 1 )

   signed portBASE_TYPE xQueueAltGenericReceive( xQueueHandle xQueue, void * const pvBuffer, portTickType xTicksToWait, portBASE_TYPE xJustPeeking )
   {
   signed portBASE_TYPE xEntryTimeSet = pdFALSE;
   xTimeOutType xTimeOut;
   signed char *pcOriginalReadPosition;
   xQUEUE *pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;
      configASSERT( pxQueue );
      configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

      for( ;; )
      {
         taskENTER_CRITICAL();
         {
            if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
            {
               /* Remember our read position in case we are just peeking. */
               pcOriginalReadPosition = pxQueue->pcReadFrom;

               prvCopyDataFromQueue( pxQueue, pvBuffer );

               if( xJustPeeking == pdFALSE )
               {
                  traceQUEUE_RECEIVE( pxQueue );

                  /* We are actually removing data. */
                  --( pxQueue->uxMessagesWaiting );

                  #if ( configUSE_MUTEXES == 1 )
                  {
                     if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
                     {
                        /* Record the information required to implement
                        priority inheritance should it become necessary. */
                        pxQueue->pxMutexHolder = xTaskGetCurrentTaskHandle();
                     }
                  }
                  #endif

                  if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
                  {
                     if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) == pdTRUE )
                     {
                        portYIELD_WITHIN_API();
                     }
                  }
               }
               else
               {
                  traceQUEUE_PEEK( pxQueue );

                  /* We are not removing the data, so reset our read
                  pointer. */
                  pxQueue->pcReadFrom = pcOriginalReadPosition;

                  /* The data is being left in the queue, so see if there are
                  any other tasks waiting for the data. */
                  if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                  {
                     /* Tasks that are removed from the event list will get added to
                     the pending ready list as the scheduler is still suspended. */
                     if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                     {
                        /* The task waiting has a higher priority than this task. */
                        portYIELD_WITHIN_API();
                     }
                  }

               }

               taskEXIT_CRITICAL();
               return pdPASS;
            }
            else
            {
               if( xTicksToWait == ( portTickType ) 0 )
               {
                  taskEXIT_CRITICAL();
                  traceQUEUE_RECEIVE_FAILED( pxQueue );
                  return errQUEUE_EMPTY;
               }
               else if( xEntryTimeSet == pdFALSE )
               {
                  vTaskSetTimeOutState( &xTimeOut );
                  xEntryTimeSet = pdTRUE;
               }
            }
         }
         taskEXIT_CRITICAL();

         taskENTER_CRITICAL();
         {
            if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
            {
               if( prvIsQueueEmpty( pxQueue ) != pdFALSE )
               {
                  traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue );

                  #if ( configUSE_MUTEXES == 1 )
                  {
                     if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
                     {
                        portENTER_CRITICAL();
                        {
                           vTaskPriorityInherit( ( void * ) pxQueue->pxMutexHolder );
                        }
                        portEXIT_CRITICAL();
                     }
                  }
                  #endif

                  vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
                  portYIELD_WITHIN_API();
               }
            }
            else
            {
               taskEXIT_CRITICAL();
               traceQUEUE_RECEIVE_FAILED( pxQueue );
               return errQUEUE_EMPTY;
            }
         }
         taskEXIT_CRITICAL();
      }
   }


#endif /* configUSE_ALTERNATIVE_API */
/*-----------------------------------------------------------*/
/*. The xTxLock (The Transmit Lock)
     The xTxLock protects the xTasksWaitingToReceive list (the list of consumer tasks that are asleep because the queue was empty).
     The Potential Disaster (Without xTxLock)
      Imagine a low-priority Task R (Receiver) wants to read from an empty queue.
        Task R checks the queue, sees it is empty, and prepares to put itself to sleep.
        It suspends the scheduler (vTaskSuspendAll()). This means no other task can interrupt it.
        Task R starts moving meains INSERTING its own TCB into the queue's xTasksWaitingToReceive list. This takes multiple CPU instructions.
        [CRASH POINT] Right in the middle of modifying this list, a hardware timer or UART interrupt fires!
        The ISR calls xQueueSendFromISR(). It drops data into the queue. It sees Task R in the list and says: "Oh, a task is waiting! 
          Let me REMOVE it from this list and wake it up."
        The Result: The ISR modifies the exact same memory pointers that Task R was currently modifying. The list pointers become corrupt, pointing to random memory addresses. 
        The microcontroller hard-faults and crashes.
        
    How xTxLock Fixes It
        When Task R suspended the scheduler, FreeRTOS automatically set xTxLock = 0 (Locked).
        When the ISR fires mid-operation, it sees xTxLock == queueUNLOCKED is False.
        The ISR says: "I cannot touch the xTasksWaitingToReceive list right now; a task is mid-modification.
        "The ISR drops the data into the storage slot, increments xTxLock to 1, and leaves.
        Once Task R is completely finished safely formatting its list structures, it calls prvUnlockQueue(). 
        It sees xTxLock == 1, knows data arrived, and safely unblocks itself without any pointer collisions.

  Summary:- the entire drama is to block INSERT and REMOVE from xTasksWaitingToReceive happending at the same time

*/
signed portBASE_TYPE xQueueGenericSendFromISR( xQueueHandle xQueue, const void * const pvItemToQueue, signed portBASE_TYPE *pxHigherPriorityTaskWoken, portBASE_TYPE xCopyPosition )
{
   signed portBASE_TYPE xReturn;
   unsigned portBASE_TYPE uxSavedInterruptStatus;
   xQUEUE *pxQueue;

   pxQueue = ( xQUEUE * ) xQueue;
   configASSERT( pxQueue );
   configASSERT( !( ( pvItemToQueue == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

   /* Similar to xQueueGenericSend, except we don't block if there is no room
   in the queue.  Also we don't directly wake a task that was blocked on a
   queue read, instead we return a flag to say whether a context switch is
   required or not (i.e. has a task with a higher priority than us been woken
   by this   post). */
   uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
   {
      if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )//Q not full?
      {
         traceQUEUE_SEND_FROM_ISR( pxQueue );

         prvCopyDataToQueue( pxQueue, pvItemToQueue, xCopyPosition );

         /* If the queue is locked we do not alter the event list.  This will be done when the queue is unlocked later. */
         if( pxQueue->xTxLock == queueUNLOCKED )//its to distinguish between task and Q aribitration, 
         {                                      //here it means queue is completely open and no task is currently modifying the task lists
            #if ( configUSE_QUEUE_SETS == 1 )
            {
               if( pxQueue->pxQueueSetContainer != NULL )
               {
                  if( prvNotifyQueueSetContainer( pxQueue, xCopyPosition ) == pdTRUE )
                  {
                     /* The queue is a member of a queue set, and posting
                     to the queue set caused a higher priority task to
                     unblock.  A context switch is required. */
                     if( pxHigherPriorityTaskWoken != NULL )
                     {
                        *pxHigherPriorityTaskWoken = pdTRUE;
                     }
                  }
               }
               else
               {
                  if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
                  {
                     if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                     {
                        /* The task waiting has a higher priority so record that a
                        context   switch is required. */
                        if( pxHigherPriorityTaskWoken != NULL )
                        {
                           *pxHigherPriorityTaskWoken = pdTRUE;
                        }
                     }
                  }
               }
            }
            #else /* configUSE_QUEUE_SETS */
            {
               if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
               {
                  /*
                     1.Remove the eventlist and generic list of TCB from currnet container 
                            and put in PendingList or ReadyTask List depending on TaskSuspended state
                         */
                        if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                  {
                     /* The task waiting has a higher priority so record that a
                     context   switch is required. */
                     if( pxHigherPriorityTaskWoken != NULL )
                     {
                        *pxHigherPriorityTaskWoken = pdTRUE;
                     }
                  }
               }
            }
            #endif /* configUSE_QUEUE_SETS */
         }
         else //there is a contention and i cannot unblock event list Q as some other task has already locked the event list Q,so just increment the lock
         {
            /* Increment the lock count so the task that unlocks the queue knows that data was posted while it was locked. */
            ++( pxQueue->xTxLock );//the data is dropped into the Q so the prvUnlockQueue() will see and unblock the task that wants to read it
         }

         xReturn = pdPASS;
      }
      else //Q full
      {
         traceQUEUE_SEND_FROM_ISR_FAILED( pxQueue );
         xReturn = errQUEUE_FULL;
      }
   }
   portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

   return xReturn;
}
/*-----------------------------------------------------------*/
/*
   Very similar to GenericSend,but difference is here xTasksWaitingToReceive will   
                     place EventList and GenericList in xSuspendedTaskList if xTicksToWait is portMAX_DELAY (indefinite)
                                         pxOverflowDelayedTaskList if xticks overshot xTicksToWait 
                                         pxDelayedTaskList if xTicks waiting to hit timeout
*/
signed portBASE_TYPE xQueueGenericReceive( xQueueHandle xQueue, void * const pvBuffer, portTickType xTicksToWait, portBASE_TYPE xJustPeeking )
{
    signed portBASE_TYPE xEntryTimeSet = pdFALSE;
    xTimeOutType xTimeOut;
    signed char *pcOriginalReadPosition;
    xQUEUE *pxQueue;

   pxQueue = ( xQUEUE * ) xQueue;
   configASSERT( pxQueue );
   configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

   /* This function relaxes the coding standard somewhat to allow return
   statements within the function itself.  This is done in the interest
   of execution time efficiency. */

   for( ;; )
   {
      taskENTER_CRITICAL();
      {
         /* Is there data in the queue now?  To be running we must be the highest priority task wanting to access the queue. */
         if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )//if messagwaiting in Q
         {
            /* Remember our read position in case we are just peeking. */
            pcOriginalReadPosition = pxQueue->pcReadFrom;

            prvCopyDataFromQueue( pxQueue, pvBuffer );//memcpy(pvBuffer,ReadFrom,Itemsize) in case if pcReadTo reaches pcTail  then wrap around to pcHead 
   
            if( xJustPeeking == pdFALSE )
            {
               traceQUEUE_RECEIVE( pxQueue );

               /* We are actually removing data. */
               --( pxQueue->uxMessagesWaiting );

               #if ( configUSE_MUTEXES == 1 )
               {
                  if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
                  {
                     /* Record the information required to implement
                     priority inheritance should it become necessary. */
                     pxQueue->pxMutexHolder = xTaskGetCurrentTaskHandle();
                  }
               }
               #endif

               if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
               {
                  if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) == pdTRUE )
                  {
                     portYIELD_WITHIN_API();
                  }
               }
            }
            else//Q is not full so the task waiting to read data can be relieved immediately
            {
               traceQUEUE_PEEK( pxQueue );

               /* The data is not being removed, so reset the read
               pointer. */
               pxQueue->pcReadFrom = pcOriginalReadPosition;

               /* The data is being left in the queue, so see if there are any other tasks waiting for the data. */
               if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )//if any items inside the waitingToReceive DB is not zero?
               {
                  /* Tasks that are removed from the event list will get added to
                  the pending ready list as the scheduler is still suspended. */

                  /* Remove the eventlist and generic list of TCB from currnet container 
                     and put in PendingList or ReadyTask List depending on TaskSuspended state */
                  if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                  {
                     /* The task waiting has a higher priority than this task. */
                     portYIELD_WITHIN_API();
                  }
               }
            }

            taskEXIT_CRITICAL();
            return pdPASS;
         }
         else //Q is empty i.e there are no message in Q ,then is there a timeout specified because task will get blocked
         {
            if( xTicksToWait == ( portTickType ) 0 )
            {
               /* The queue was empty and no block time is specified (or
               the block time has expired) so leave now. */
               taskEXIT_CRITICAL();
               traceQUEUE_RECEIVE_FAILED( pxQueue );
               return errQUEUE_EMPTY;
            }
            else if( xEntryTimeSet == pdFALSE )
            {
               /* The queue was empty and a block time was specified so
               configure the timeout structure. */
               vTaskSetTimeOutState( &xTimeOut );
               xEntryTimeSet = pdTRUE;
            }
         }
      }
      taskEXIT_CRITICAL();

      /* Interrupts and other tasks can send to and receive from the queue
      now the critical section has been exited. */

      vTaskSuspendAll();
      prvLockQueue( pxQueue );

      /* Update the timeout state to see if it has expired yet. */
      if( xTaskCheckForTimeOut( &xTimeOut, &xTicksToWait ) == pdFALSE )
      {
         if( prvIsQueueEmpty( pxQueue ) != pdFALSE )//if Q empty
         {
            traceBLOCKING_ON_QUEUE_RECEIVE( pxQueue );

            #if ( configUSE_MUTEXES == 1 )
            {
               if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
               {
                  portENTER_CRITICAL();
                  {
                     vTaskPriorityInherit( ( void * ) pxQueue->pxMutexHolder );
                  }
                  portEXIT_CRITICAL();
               }
            }
            #endif

                /*   All tasks waiting to receive data will :-
                     place EventList and GenericList in xSuspendedTaskList if xTicksToWait is portMAX_DELAY (indefinite)
                                         pxOverflowDelayedTaskList if xticks overshot xTicksToWait 
                                         pxDelayedTaskList if xTicks waiting to hit timeout
            */
            vTaskPlaceOnEventList( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
            prvUnlockQueue( pxQueue );
            if( xTaskResumeAll() == pdFALSE )
            {
               portYIELD_WITHIN_API();
            }
         }
         else//Q full
         {
            /* Try again. */
            prvUnlockQueue( pxQueue );
            ( void ) xTaskResumeAll();
         }
      }
      else
      {
         prvUnlockQueue( pxQueue );
         ( void ) xTaskResumeAll();
         traceQUEUE_RECEIVE_FAILED( pxQueue );
         return errQUEUE_EMPTY;
      }
   }
}
/*-----------------------------------------------------------*/
/*
The xRxLock (The Receive Lock)The xRxLock protects the xTasksWaitingToSend list 
(the list of producer tasks that are asleep because the queue was completely full).
The Potential Disaster (Without xRxLock)
  Imagine a high-priority Task W (Writer/Sender) wants to write to a completely full queue.
    Task W checks the queue, sees it is full (5 out of 5), and needs to go to sleep.
    It suspends the scheduler and starts inserting its TCB into the queue's xTasksWaitingToSend list.
    [CRASH POINT] Right in the middle of linking the pointers, a DMA or ADC interrupt fires!
    The ISR calls xQueueReceiveFromISR(). It reads an item out of the queue buffer to make room.
    The ISR looks at the xTasksWaitingToSend list and says: "Hey, the queue was full, but I just freed up a slot! 
       Let me pull Task W out of this list so it can wake up and write its data."
    The Result: The ISR tries to untangle a node from a list that Task W hasn't even finished weaving together. 
    The kernel's internal list structure is permanently corrupted.
  How xRxLock Fixes It
    When Task W suspended the scheduler, FreeRTOS set xRxLock = 0 (Locked).
    When the ISR fires mid-operation and removes data, it checks the lock and sees xRxLock is locked.
    The ISR says: "I successfully freed a slot, but I am forbidden from touching the xTasksWaitingToSend list right now.
    "The ISR updates the data pointers, increments xRxLock to 1, and exits.
    When Task W finishes safely placing itself on the list, it runs prvUnlockQueue(). It looks at xRxLock == 1, realizes a slot was freed up while it was preparing to sleep, and safely unblocks the next writer task in line.

  Summary:- the entire drama is to block INSERT and REMOVE from xTasksWaitingToSend happending at the same time
*/
signed portBASE_TYPE xQueueReceiveFromISR( xQueueHandle xQueue, void * const pvBuffer, signed portBASE_TYPE *pxHigherPriorityTaskWoken )
{
   signed portBASE_TYPE xReturn;
   unsigned portBASE_TYPE uxSavedInterruptStatus;
   xQUEUE *pxQueue;

   pxQueue = ( xQUEUE * ) xQueue;
   configASSERT( pxQueue );
   configASSERT( !( ( pvBuffer == NULL ) && ( pxQueue->uxItemSize != ( unsigned portBASE_TYPE ) 0U ) ) );

   uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();
   {
      /* We cannot block from an ISR, so check there is data available. */
      if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
      {
         traceQUEUE_RECEIVE_FROM_ISR( pxQueue );

         prvCopyDataFromQueue( pxQueue, pvBuffer );
         --( pxQueue->uxMessagesWaiting );

         /* If the queue is locked we will not modify the event list.  Instead
         we update the lock count so the task that unlocks the queue will know
         that an ISR has removed data while the queue was locked. */
         if( pxQueue->xRxLock == queueUNLOCKED )
         {
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
               if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
               {
                  /* The task waiting has a higher priority than us so
                  force a context switch. */
                  if( pxHigherPriorityTaskWoken != NULL )
                  {
                     *pxHigherPriorityTaskWoken = pdTRUE;
                  }
               }
            }
         }
         else
         {
            /* Increment the lock count so the task that unlocks the queue knows that data was removed while it was locked. */
            ++( pxQueue->xRxLock );
         }

         xReturn = pdPASS;
      }
      else
      {
         xReturn = pdFAIL;
         traceQUEUE_RECEIVE_FROM_ISR_FAILED( pxQueue );
      }
   }
   portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );

   return xReturn;
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE uxQueueMessagesWaiting( const xQueueHandle xQueue )
{
unsigned portBASE_TYPE uxReturn;

   configASSERT( xQueue );

   taskENTER_CRITICAL();
      uxReturn = ( ( xQUEUE * ) xQueue )->uxMessagesWaiting;
   taskEXIT_CRITICAL();

   return uxReturn;
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE uxQueueMessagesWaitingFromISR( const xQueueHandle xQueue )
{
unsigned portBASE_TYPE uxReturn;

   configASSERT( xQueue );

   uxReturn = ( ( xQUEUE * ) xQueue )->uxMessagesWaiting;

   return uxReturn;
}
/*-----------------------------------------------------------*/

void vQueueDelete( xQueueHandle xQueue )
{
xQUEUE *pxQueue;

   pxQueue = ( xQUEUE * ) xQueue;
   configASSERT( pxQueue );

   traceQUEUE_DELETE( pxQueue );
   #if ( configQUEUE_REGISTRY_SIZE > 0 )
   {
      prvQueueUnregisterQueue( pxQueue );
   }
   #endif
   vPortFree( pxQueue->pcHead );
   vPortFree( pxQueue );
}
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   unsigned char ucQueueGetQueueNumber( xQueueHandle xQueue )
   {
      return ( ( xQUEUE * ) xQueue )->ucQueueNumber;
   }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   void vQueueSetQueueNumber( xQueueHandle xQueue, unsigned char ucQueueNumber )
   {
      ( ( xQUEUE * ) xQueue )->ucQueueNumber = ucQueueNumber;
   }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

#if ( configUSE_TRACE_FACILITY == 1 )

   unsigned char ucQueueGetQueueType( xQueueHandle xQueue )
   {
      return ( ( xQUEUE * ) xQueue )->ucQueueType;
   }

#endif /* configUSE_TRACE_FACILITY */
/*-----------------------------------------------------------*/

static void prvCopyDataToQueue( xQUEUE *pxQueue, const void *pvItemToQueue, portBASE_TYPE xPosition )
{
   if( pxQueue->uxItemSize == ( unsigned portBASE_TYPE ) 0 )
   {
      #if ( configUSE_MUTEXES == 1 )
      {
         if( pxQueue->uxQueueType == queueQUEUE_IS_MUTEX )
         {
            /* The mutex is no longer being held. */
            vTaskPriorityDisinherit( ( void * ) pxQueue->pxMutexHolder );
            pxQueue->pxMutexHolder = NULL;
         }
      }
      #endif
   }
   else if( xPosition == queueSEND_TO_BACK )
   {
      memcpy( ( void * ) pxQueue->pcWriteTo, pvItemToQueue, ( size_t ) pxQueue->uxItemSize );//copy data from vairble to Q slot from Front, i.e pcWriteTo <= variable[itemsize]
      pxQueue->pcWriteTo += pxQueue->uxItemSize;//pcWriteTo will point to the next to Tail
      if( pxQueue->pcWriteTo >= pxQueue->pcTail )
      {
         pxQueue->pcWriteTo = pxQueue->pcHead;//if prWriteTo reached Tail then roll back to pcHead
      }
   }
   else //queueSEND_TO_FRONT
   {
      memcpy( ( void * ) pxQueue->pcReadFrom, pvItemToQueue, ( size_t ) pxQueue->uxItemSize );//copy data from vairble to Q slot from Back, i.e pcReadFrom <= variable[itemsize]
      pxQueue->pcReadFrom -= pxQueue->uxItemSize;//pcReadFrom will point to start
      if( pxQueue->pcReadFrom < pxQueue->pcHead )//if pcReadFrom reached before PcHead then roll back to pcTail - itemsize i.e just one before sentinel
      {
         pxQueue->pcReadFrom = ( pxQueue->pcTail - pxQueue->uxItemSize );
      }
   }

   ++( pxQueue->uxMessagesWaiting );//now a message is waiting where to send to front or back
}
/*-----------------------------------------------------------*/

static void prvCopyDataFromQueue( xQUEUE * const pxQueue, const void *pvBuffer )
{
   if( pxQueue->uxQueueType != queueQUEUE_IS_MUTEX )
   {
      pxQueue->pcReadFrom += pxQueue->uxItemSize;
      if( pxQueue->pcReadFrom >= pxQueue->pcTail )
      {
         pxQueue->pcReadFrom = pxQueue->pcHead;
      }
      memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->pcReadFrom, ( size_t ) pxQueue->uxItemSize );
   }
}
/*-----------------------------------------------------------*/
/*
 *
 * Unblocks tasks waiting to receive data (xTasksWaitingToReceive) if an ISR posted new messages to the queue (xTxLock > 0).
 * Unblocks tasks waiting to send data (xTasksWaitingToSend) if an ISR  removed messages and freed up slot space (xRxLock > 0).
 * Defers context switches for newly unblocked higher-priority tasks via  vTaskMissedYield(), as the scheduler remains suspended during execution. 
 *
 * =================================================================================
 *                                 FUNCTION SUMMARY
 * =================================================================================
 * 1. RECONCILES INTERRUPT ACTIONS: Processes data movements (reads/writes) that 
 *    occurred inside Interrupt Service Routines (ISRs) while the queue was locked.
 * 2. SAFE EVENT LIST UPDATES: Modifies task event waiting lists safely without 
 *    interfering with active scheduling operations.
 * 3. UNBLOCKS DELAYED CONSUMERS: Evaluates the Tx Lock counter to awaken tasks 
 *    blocked on an empty queue once an ISR adds new data.
 * 4. UNBLOCKS DELAYED PRODUCERS: Evaluates the Rx Lock counter to awaken tasks 
 *    blocked on a full queue once an ISR frees up storage space.
 * 5. DEFERS CONTEXT SWITCHING: Registers pending task switches using vTaskMissedYield() 
 *    because the scheduler remains frozen during execution.
 *
 * =================================================================================
 *                             LINE-BY-LINE EXPLANATION
 * =================================================================================
 * * taskENTER_CRITICAL() / taskEXIT_CRITICAL()
 *   Disables interrupts on the LPC1114 to cleanly freeze the lock counters, ensuring 
 *   an incoming hardware interrupt cannot change the counts mid-evaluation.
 *
 * * while( pxQueue->xTxLock > queueLOCKED_UNMODIFIED )
 *   Loops continuously if an ISR appended data while the queue was locked. If xTxLock 
 *   is 2, it indicates an ISR executed two successful 'SendFromISR' operations.
 *
 * * if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
 *   Checks if any tasks are sleeping because the queue was empty. If a task is waiting, 
 *   it calls xTaskRemoveFromEventList() to move that task into the Pending Ready List.
 *
 * * vTaskMissedYield()
 *   Triggers if the newly unblocked task has a higher priority than the currently running 
 *   task. It records a flag to force a context switch the moment the scheduler is unfrozen.
 *
 * * --( pxQueue->xTxLock ) / pxQueue->xTxLock = queueUNLOCKED
 *   Decrements the processed event count. Once the loop exhausts all counts or waiting 
 *   tasks, it restores the state identifier back to its default unlocked state (-1).
 *
 * * [The Rx Lock Block]
 *   Repeats the identical logic path for data removals. If an ISR read data (xRxLock > 0), 
 *   it checks xTasksWaitingToSend and wakes up tasks that were trapped waiting on a full queue.
 *
 * =================================================================================
 *                             CONCRETE SYSTEM EXAMPLES
 * =================================================================================
 *  if pxQueue->xTxLock is 0? then unlock it
 *   else 
 *      if there are items in xTasksWaitingToReceive Q then add add EventList and GenericList to 
 *      pendingReadyList if scheduler is suspended else put in pxReadyTasksLists
 *      we repeat this until its unlocked meaning xTxLock = 0
 *
 *  if pxQueue->xRxLock is 0? then unlock it 
 *  else
 *      if there are items in xTasksWaitingToSend Q then add add EventList and GenericList to 
 *      pendingReadyList if scheduler is suspended else put in pxReadyTasksLists
 *      we repeat this until its unlocked meaning xRxLock = 0
 *
 * EXAMPlE 1: UART RX INTERRUPT (Tx Lock Processing)
 * 1. A high-priority Task A is asleep waiting for commands on an empty data queue.
 * 2. While the queue is locked elsewhere, a UART character arrives.
 * 3. The UART Interrupt fires, runs xQueueSendFromISR(), and places the byte in the buffer.
 * 4. The ISR cannot safely modify Task A's state directly, so it leaves xTxLock = 1.
 * 5. When prvUnlockQueue() runs, it notices xTxLock is 1, checks the waiting list, finds 
 *    Task A, and moves it to the Pending Ready List so it can process the incoming byte.
 *
 * EXAMPLE 2: ADC SAMPLING DMA INTERRUPT (Rx Lock Processing)
 * 1. A Producer Task is blocked because it completely filled up a 5-slot queue.
 * 2. While locked, a DMA Interrupt fires to read an old processed sample out of the 
 *    queue using xQueueReceiveFromISR() to free up space.
 * 3. The interrupt leaves xRxLock = 1.
 * 4. prvUnlockQueue() runs, catches xRxLock at 1, identifies the blocked Producer Task 
 *    waiting to send data, and wakes it up because a storage slot is now vacant.
 
    TxLock tells that Queue is going to be filled becuase a write is currently happening
    RxLock tells that Queue is going to be empty becuase a read is currently happening
 
 */

static void prvUnlockQueue( xQUEUE *pxQueue )
{
   /* THIS FUNCTION MUST BE CALLED WITH THE SCHEDULER SUSPENDED. */

   /* The lock counts contains the number of extra data items placed or removed from the queue while the queue was locked.  
        When a queue is locked items can be added or removed, but the event lists cannot be   updated. */
   taskENTER_CRITICAL();
   {
    /*#######################################################*/
   /* Do the same for the Tx lock (xTasksWaitingToReceive). */
    /*#######################################################*/
      /* See if data was added to the queue while it was locked. */
      while( pxQueue->xTxLock > queueLOCKED_UNMODIFIED )//xTxLock > 0? actually xTxLock gets incremented inside SendfromISR,else it will be -1 or 0
      {
         /* Data was posted while the queue was locked.  Are any tasks
         blocked waiting for data to become available? */
         #if ( configUSE_QUEUE_SETS == 1 )
         {
            if( pxQueue->pxQueueSetContainer != NULL )
            {
               if( prvNotifyQueueSetContainer( pxQueue, queueSEND_TO_BACK ) == pdTRUE )
               {
                  /* The queue is a member of a queue set, and posting to
                  the queue set caused a higher priority task to unblock.
                  A context switch is required. */
                  vTaskMissedYield();
               }
            }
            else
            {
               /* Tasks that are removed from the event list will get added to
               the pending ready list as the scheduler is still suspended. */
               if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
               {
                  if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
                  {
                     /* The task waiting has a higher priority so record that a
                     context   switch is required. */
                     vTaskMissedYield();
                  }
               }
               else
               {
                  break;
               }
            }
         }
         #else /* configUSE_QUEUE_SETS */
         {
            /* Tasks that are removed from the event list will get added to the pending ready list as the scheduler is still suspended. */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )//if there are items in xTasksWaitingToReceive Q
            {
               if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )//add Q to pendingReady if scheduler is suspended else put in ready
               {
                  /* The task waiting has a higher priority so record that a
                  context   switch is required. */
                  vTaskMissedYield();//switch to this pending ready list as soon as scheduler is activated
               }
            }
            else
            {
               break;
            }
         }
         #endif /* configUSE_QUEUE_SETS */

         --( pxQueue->xTxLock );//assuming if lock is above 0. reduce to UNLOCKED one by one
      }

      pxQueue->xTxLock = queueUNLOCKED;//unlock TxLock , xTxLock = -1 
   }
   taskEXIT_CRITICAL();
    /*##################################################*/
   /* Do the same for the Rx lock xTasksWaitingToSend. */
    /*##################################################*/
   taskENTER_CRITICAL();
   {
      while( pxQueue->xRxLock > queueLOCKED_UNMODIFIED )//xRxLock > 0? is it locked?
      {
         if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )//here check if there any items inside task waiting to send i.e list is not empty
         {
            if( xTaskRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )//add Q to pending if scheduler is suspended else put in ready 
            {
               vTaskMissedYield();
            }

            --( pxQueue->xRxLock );//reduce the lock one by one make it to UNLOCKED at the end
         }
         else
         {
            break;
         }
      }

      pxQueue->xRxLock = queueUNLOCKED;
   }
   taskEXIT_CRITICAL();
}
/*-----------------------------------------------------------*/

static signed portBASE_TYPE prvIsQueueEmpty( const xQUEUE *pxQueue )
{
signed portBASE_TYPE xReturn;

   taskENTER_CRITICAL();
   {
      if( pxQueue->uxMessagesWaiting == 0 )
      {
         xReturn = pdTRUE;
      }
      else
      {
         xReturn = pdFALSE;
      }
   }
   taskEXIT_CRITICAL();

   return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueIsQueueEmptyFromISR( const xQueueHandle xQueue )
{
signed portBASE_TYPE xReturn;

   configASSERT( xQueue );
   if( ( ( xQUEUE * ) xQueue )->uxMessagesWaiting == 0 )
   {
      xReturn = pdTRUE;
   }
   else
   {
      xReturn = pdFALSE;
   }

   return xReturn;
}
/*-----------------------------------------------------------*/

static signed portBASE_TYPE prvIsQueueFull( const xQUEUE *pxQueue )
{
    signed portBASE_TYPE xReturn;

   taskENTER_CRITICAL();
   {
      if( pxQueue->uxMessagesWaiting == pxQueue->uxLength )
      {
         xReturn = pdTRUE;
      }
      else
      {
         xReturn = pdFALSE;
      }
   }
   taskEXIT_CRITICAL();

   return xReturn;
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xQueueIsQueueFullFromISR( const xQueueHandle xQueue )
{
signed portBASE_TYPE xReturn;

   configASSERT( xQueue );
   if( ( ( xQUEUE * ) xQueue )->uxMessagesWaiting == ( ( xQUEUE * ) xQueue )->uxLength )
   {
      xReturn = pdTRUE;
   }
   else
   {
      xReturn = pdFALSE;
   }

   return xReturn;
}
/*-----------------------------------------------------------*/

#if ( configUSE_CO_ROUTINES == 1 )

   signed portBASE_TYPE xQueueCRSend( xQueueHandle xQueue, const void *pvItemToQueue, portTickType xTicksToWait )
   {
   signed portBASE_TYPE xReturn;
   xQUEUE *pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;

      /* If the queue is already full we may have to block.  A critical section
      is required to prevent an interrupt removing something from the queue
      between the check to see if the queue is full and blocking on the queue. */
      portDISABLE_INTERRUPTS();
      {
         if( prvIsQueueFull( pxQueue ) != pdFALSE )
         {
            /* The queue is full - do we want to block or just leave without
            posting? */
            if( xTicksToWait > ( portTickType ) 0 )
            {
               /* As this is called from a coroutine we cannot block directly, but
               return indicating that we need to block. */
               vCoRoutineAddToDelayedList( xTicksToWait, &( pxQueue->xTasksWaitingToSend ) );
               portENABLE_INTERRUPTS();
               return errQUEUE_BLOCKED;
            }
            else
            {
               portENABLE_INTERRUPTS();
               return errQUEUE_FULL;
            }
         }
      }
      portENABLE_INTERRUPTS();

      portDISABLE_INTERRUPTS();
      {
         if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
         {
            /* There is room in the queue, copy the data into the queue. */
            prvCopyDataToQueue( pxQueue, pvItemToQueue, queueSEND_TO_BACK );
            xReturn = pdPASS;

            /* Were any co-routines waiting for data to become available? */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
            {
               /* In this instance the co-routine could be placed directly
               into the ready list as we are within a critical section.
               Instead the same pending ready list mechanism is used as if
               the event were caused from within an interrupt. */
               if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
               {
                  /* The co-routine waiting has a higher priority so record
                  that a yield might be appropriate. */
                  xReturn = errQUEUE_YIELD;
               }
            }
         }
         else
         {
            xReturn = errQUEUE_FULL;
         }
      }
      portENABLE_INTERRUPTS();

      return xReturn;
   }

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

#if ( configUSE_CO_ROUTINES == 1 )

   signed portBASE_TYPE xQueueCRReceive( xQueueHandle xQueue, void *pvBuffer, portTickType xTicksToWait )
   {
   signed portBASE_TYPE xReturn;
   xQUEUE *pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;

      /* If the queue is already empty we may have to block.  A critical section
      is required to prevent an interrupt adding something to the queue
      between the check to see if the queue is empty and blocking on the queue. */
      portDISABLE_INTERRUPTS();
      {
         if( pxQueue->uxMessagesWaiting == ( unsigned portBASE_TYPE ) 0 )
         {
            /* There are no messages in the queue, do we want to block or just
            leave with nothing? */
            if( xTicksToWait > ( portTickType ) 0 )
            {
               /* As this is a co-routine we cannot block directly, but return
               indicating that we need to block. */
               vCoRoutineAddToDelayedList( xTicksToWait, &( pxQueue->xTasksWaitingToReceive ) );
               portENABLE_INTERRUPTS();
               return errQUEUE_BLOCKED;
            }
            else
            {
               portENABLE_INTERRUPTS();
               return errQUEUE_FULL;
            }
         }
      }
      portENABLE_INTERRUPTS();

      portDISABLE_INTERRUPTS();
      {
         if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
         {
            /* Data is available from the queue. */
            pxQueue->pcReadFrom += pxQueue->uxItemSize;
            if( pxQueue->pcReadFrom >= pxQueue->pcTail )
            {
               pxQueue->pcReadFrom = pxQueue->pcHead;
            }
            --( pxQueue->uxMessagesWaiting );
            memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->pcReadFrom, ( unsigned ) pxQueue->uxItemSize );

            xReturn = pdPASS;

            /* Were any co-routines waiting for space to become available? */
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
               /* In this instance the co-routine could be placed directly
               into the ready list as we are within a critical section.
               Instead the same pending ready list mechanism is used as if
               the event were caused from within an interrupt. */
               if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
               {
                  xReturn = errQUEUE_YIELD;
               }
            }
         }
         else
         {
            xReturn = pdFAIL;
         }
      }
      portENABLE_INTERRUPTS();

      return xReturn;
   }

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

#if ( configUSE_CO_ROUTINES == 1 )

   signed portBASE_TYPE xQueueCRSendFromISR( xQueueHandle xQueue, const void *pvItemToQueue, signed portBASE_TYPE xCoRoutinePreviouslyWoken )
   {
   xQUEUE *pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;

      /* Cannot block within an ISR so if there is no space on the queue then
      exit without doing anything. */
      if( pxQueue->uxMessagesWaiting < pxQueue->uxLength )
      {
         prvCopyDataToQueue( pxQueue, pvItemToQueue, queueSEND_TO_BACK );

         /* We only want to wake one co-routine per ISR, so check that a
         co-routine has not already been woken. */
         if( xCoRoutinePreviouslyWoken == pdFALSE )
         {
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToReceive ) ) == pdFALSE )
            {
               if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToReceive ) ) != pdFALSE )
               {
                  return pdTRUE;
               }
            }
         }
      }

      return xCoRoutinePreviouslyWoken;
   }

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

#if ( configUSE_CO_ROUTINES == 1 )

   signed portBASE_TYPE xQueueCRReceiveFromISR( xQueueHandle xQueue, void *pvBuffer, signed portBASE_TYPE *pxCoRoutineWoken )
   {
   signed portBASE_TYPE xReturn;
   xQUEUE * pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;

      /* We cannot block from an ISR, so check there is data available. If
      not then just leave without doing anything. */
      if( pxQueue->uxMessagesWaiting > ( unsigned portBASE_TYPE ) 0 )
      {
         /* Copy the data from the queue. */
         pxQueue->pcReadFrom += pxQueue->uxItemSize;
         if( pxQueue->pcReadFrom >= pxQueue->pcTail )
         {
            pxQueue->pcReadFrom = pxQueue->pcHead;
         }
         --( pxQueue->uxMessagesWaiting );
         memcpy( ( void * ) pvBuffer, ( void * ) pxQueue->pcReadFrom, ( unsigned ) pxQueue->uxItemSize );

         if( ( *pxCoRoutineWoken ) == pdFALSE )
         {
            if( listLIST_IS_EMPTY( &( pxQueue->xTasksWaitingToSend ) ) == pdFALSE )
            {
               if( xCoRoutineRemoveFromEventList( &( pxQueue->xTasksWaitingToSend ) ) != pdFALSE )
               {
                  *pxCoRoutineWoken = pdTRUE;
               }
            }
         }

         xReturn = pdPASS;
      }
      else
      {
         xReturn = pdFAIL;
      }

      return xReturn;
   }

#endif /* configUSE_CO_ROUTINES */
/*-----------------------------------------------------------*/

#if ( configQUEUE_REGISTRY_SIZE > 0 )

   void vQueueAddToRegistry( xQueueHandle xQueue, signed char *pcQueueName )
   {
   unsigned portBASE_TYPE ux;

      /* See if there is an empty space in the registry.  A NULL name denotes
      a free slot. */
      for( ux = ( unsigned portBASE_TYPE ) 0U; ux < ( unsigned portBASE_TYPE ) configQUEUE_REGISTRY_SIZE; ux++ )
      {
         if( xQueueRegistry[ ux ].pcQueueName == NULL )
         {
            /* Store the information on this queue. */
            xQueueRegistry[ ux ].pcQueueName = pcQueueName;
            xQueueRegistry[ ux ].xHandle = xQueue;
            break;
         }
      }
   }

#endif /* configQUEUE_REGISTRY_SIZE */
/*-----------------------------------------------------------*/

#if ( configQUEUE_REGISTRY_SIZE > 0 )

   static void prvQueueUnregisterQueue( xQueueHandle xQueue )
   {
   unsigned portBASE_TYPE ux;

      /* See if the handle of the queue being unregistered in actually in the
      registry. */
      for( ux = ( unsigned portBASE_TYPE ) 0U; ux < ( unsigned portBASE_TYPE ) configQUEUE_REGISTRY_SIZE; ux++ )
      {
         if( xQueueRegistry[ ux ].xHandle == xQueue )
         {
            /* Set the name to NULL to show that this slot if free again. */
            xQueueRegistry[ ux ].pcQueueName = NULL;
            break;
         }
      }

   }

#endif /* configQUEUE_REGISTRY_SIZE */
/*-----------------------------------------------------------*/

#if ( configUSE_TIMERS == 1 )

   void vQueueWaitForMessageRestricted( xQueueHandle xQueue, portTickType xTicksToWait )
   {
   xQUEUE *pxQueue;

      pxQueue = ( xQUEUE * ) xQueue;

      /* This function should not be called by application code hence the
      'Restricted' in its name.  It is not part of the public API.  It is
      designed for use by kernel code, and has special calling requirements.
      It can result in vListInsert() being called on a list that can only
      possibly ever have one item in it, so the list will be fast, but even
      so it should be called with the scheduler locked and not from a critical
      section. */

      /* Only do anything if there are no messages in the queue.  This function
      will not actually cause the task to block, just place it on a blocked
      list.  It will not block until the scheduler is unlocked - at which
      time a yield will be performed.  If an item is added to the queue while
      the queue is locked, and the calling task blocks on the queue, then the
      calling task will be immediately unblocked when the queue is unlocked. */
      prvLockQueue( pxQueue );
      if( pxQueue->uxMessagesWaiting == ( unsigned portBASE_TYPE ) 0U )
      {
         /* There is nothing in the queue, block for the specified period. */
         vTaskPlaceOnEventListRestricted( &( pxQueue->xTasksWaitingToReceive ), xTicksToWait );
      }
      prvUnlockQueue( pxQueue );
   }

#endif /* configUSE_TIMERS */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )

   xQueueSetHandle xQueueCreateSet( unsigned portBASE_TYPE uxEventQueueLength )
   {
   xQueueSetHandle pxQueue;

      pxQueue = xQueueGenericCreate( uxEventQueueLength, sizeof( xQUEUE * ), queueQUEUE_TYPE_SET );

      return pxQueue;
   }

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )

   portBASE_TYPE xQueueAddToSet( xQueueSetMemberHandle xQueueOrSemaphore, xQueueSetHandle xQueueSet )
   {
   portBASE_TYPE xReturn;

      if( ( ( xQUEUE * ) xQueueOrSemaphore )->pxQueueSetContainer != NULL )
      {
         xReturn = pdFAIL;
      }
      else
      {
         taskENTER_CRITICAL();
         {
            ( ( xQUEUE * ) xQueueOrSemaphore )->pxQueueSetContainer = xQueueSet;
         }
         taskEXIT_CRITICAL();
         xReturn = pdPASS;
      }

      return xReturn;
   }

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )

   portBASE_TYPE xQueueRemoveFromSet( xQueueSetMemberHandle xQueueOrSemaphore, xQueueSetHandle xQueueSet )
   {
   portBASE_TYPE xReturn;
   xQUEUE *pxQueueOrSemaphore;

      pxQueueOrSemaphore = ( xQUEUE * ) xQueueOrSemaphore;

      if( pxQueueOrSemaphore->pxQueueSetContainer != xQueueSet )
      {
         /* The queue was not a member of the set. */
         xReturn = pdFAIL;
      }
      else if( pxQueueOrSemaphore->uxMessagesWaiting != 0 )
      {
         /* It is dangerous to remove a queue from a set when the queue is
         not empty because the queue set will still hold pending events for
         the queue. */
         xReturn = pdFAIL;
      }
      else
      {
         taskENTER_CRITICAL();
         {
            /* The queue is no longer contained in the set. */
            pxQueueOrSemaphore->pxQueueSetContainer = NULL;
         }
         taskEXIT_CRITICAL();
         xReturn = pdPASS;
      }

      return xReturn;
   }

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )

   xQueueSetMemberHandle xQueueSelectFromSet( xQueueSetHandle xQueueSet, portTickType xBlockTimeTicks )
   {
   xQueueSetMemberHandle xReturn = NULL;

      xQueueGenericReceive( ( xQueueHandle ) xQueueSet, &xReturn, xBlockTimeTicks, pdFALSE );
      return xReturn;
   }

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )

   xQueueSetMemberHandle xQueueSelectFromSetFromISR( xQueueSetHandle xQueueSet )
   {
   xQueueSetMemberHandle xReturn = NULL;

      xQueueReceiveFromISR( ( xQueueHandle ) xQueueSet, &xReturn, NULL );
      return xReturn;
   }

#endif /* configUSE_QUEUE_SETS */
/*-----------------------------------------------------------*/

#if ( configUSE_QUEUE_SETS == 1 )

   static portBASE_TYPE prvNotifyQueueSetContainer( xQUEUE *pxQueue, portBASE_TYPE xCopyPosition )
   {
   xQUEUE *pxQueueSetContainer = pxQueue->pxQueueSetContainer;
   portBASE_TYPE xReturn = pdFALSE;

      configASSERT( pxQueueSetContainer );
      configASSERT( pxQueueSetContainer->uxMessagesWaiting < pxQueueSetContainer->uxLength );

      if( pxQueueSetContainer->uxMessagesWaiting < pxQueueSetContainer->uxLength )
      {
         traceQUEUE_SEND( pxQueueSetContainer );
         /* The data copies is the handle of the queue that contains data. */
         prvCopyDataToQueue( pxQueueSetContainer, &pxQueue, xCopyPosition );
         if( listLIST_IS_EMPTY( &( pxQueueSetContainer->xTasksWaitingToReceive ) ) == pdFALSE )
         {
            if( xTaskRemoveFromEventList( &( pxQueueSetContainer->xTasksWaitingToReceive ) ) != pdFALSE )
            {
               /* The task waiting has a higher priority */
               xReturn = pdTRUE;
            }
         }
      }

      return xReturn;
   }

#endif /* configUSE_QUEUE_SETS */

