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
#include "FreeRTOS.h"
#include "list.h"

/*-----------------------------------------------------------
 * PUBLIC LIST API documented in list.h
 *----------------------------------------------------------*/
/*
    make the DB index , next,previous to point to ListEnd
    make itemValue as 0xFFFFFFFF
    make NumberofItems as 0
*/
void vListInitialise( xList *pxList )
{
   /* The list structure contains a list item which is used to mark the
   end of the list.  To initialise the list the list end is inserted
   as the only list entry. */
   pxList->pxIndex = ( xListItem * ) &( pxList->xListEnd );//points to the wall end 

   /* The list end value is the highest possible value in the list to
   ensure it remains at the end of the list. */
   pxList->xListEnd.xItemValue = portMAX_DELAY;//0xffffffff

   /* The list end next and previous pointers point to itself so we know
   when the list is empty. */
   pxList->xListEnd.pxNext = ( xListItem * ) &( pxList->xListEnd );//points to the wall end
   pxList->xListEnd.pxPrevious = ( xListItem * ) &( pxList->xListEnd );//points to the wall end

   pxList->uxNumberOfItems = ( unsigned portBASE_TYPE ) 0U;
}                                           
/*-----------------------------------------------------------*/

void vListInitialiseItem( xListItem *pxItem )
{
   /* Make sure the list item is not recorded as being on a list. */
   pxItem->pvContainer = NULL;
}
/*-----------------------------------------------------------*/
/*
[ 0x1000: pxList (Master DB ) ]
 +---------------------------------+
 ¦ uxNumberOfItems = 0             ¦
 ¦ pxIndex = 0x1008 --------+      ¦
 +--------------------------+------+
                            ¦
                            ?
               +---------------------------------+
               ¦ 0x1008: xListEnd (Anchor Wall)  ¦
               ¦                                 ¦
         +----?¦  pxNext = 0x1008                ¦
         ¦     ¦  pxPrev = 0x1008                ¦
         +---------------------------------------+

 [ Temporary Variable ]
+--------------------+
¦ pxIndex = 0x1008   ¦
+--------------------+

[ 0x4000: pxNewListItem(New Task1 Container) ]
+----------------------------+
¦ pxNext = 0x1008            ¦ <--- Updated!
¦ pxPrev = NULL              ¦
+----------------------------+

[ 0x4000: pxNewListItem(New Task1 Container) ]
+----------------------------+
¦ pxNext = 0x1008            ¦
¦ pxPrev = 0x1008            ¦ <--- Updated!
+----------------------------+

[ 0x1008: xListEnd (Anchor Wall) ]
+----------------------------+
¦ xItemValue = 0xFFFFFFFF    ¦
¦ pxNext = 0x1008            ¦
¦ pxPrev = 0x4000            ¦ <--- Updated!
+----------------------------+

[ 0x1008: xListEnd (Anchor Wall) ]
+----------------------------+
¦ xItemValue = 0xFFFFFFFF    ¦
¦ pxNext = 0x4000            ¦ <--- Updated!
¦ pxPrev = 0x4000            ¦
+----------------------------+

[ 0x1000: pxList (Master Folder Container) ]
+----------------------------+
¦ uxNumberOfItems = 0        ¦ (Updates to 1 on line 9)
¦ pxIndex = 0x4000           ¦ <--- Updated!
+----------------------------+

+---------------------------------+
 ¦ 0x4000: New Task1 Container      ¦
 ¦                                 ¦
 ¦  pxNext = NULL                  ¦
 ¦  pxPrev = NULL                  ¦
 +---------------------------------+


                                             [ 0x1000: pxList (Master DB) ]
                                             +---------------------------------+
                                             ¦ uxNumberOfItems = 1             ¦
                                             ¦ pxIndex = 0x4000 --------+      ¦  <=== Master Pointer shifted to New Task
                                             +--------------------------+------+
                                                                        |
                                                                        v
    +---------------------------------+        +---------------------------------+   
    ¦ 0x1008: xListEnd (Anchor Wall)  ¦        ¦ 0x4000: New Task1 Container     ¦   
    +---------------------------------+        +---------------------------------+   
¦-->¦ pxPrev=0x4000 | pxNext=0x4000   +------->¦ pxPrev=0x1008 | pxNext=0x1008   ¦-->¦
¦   +---------------------------------+        +---------------------------------+   ¦
¦                                                                                    ¦
¦------------------------------------------------------------------------------------¦

----End of 1st insert-------
-------------------------------------------------------------------------------------
what happens when another New Task2 wants to come
+---------------------------------+
¦ pxIndexLocal = 0x4000           ¦ ---? (Points directly to Task 1 because Master DB is updated with pxIndex->0x4000)
+---------------------------------+

[ 0x5000: Task 2 Container (NEW) ](look above and end of first insert
+---------------------------------+
¦  pxNext = 0x1008                ¦ ?--- [CHANGED!] Points to Anchor Wall
¦  pxPrev = NULL                  ¦
+---------------------------------+

[ 0x5000: Task 2 Container (NEW) ]
+---------------------------------+
¦  pxNext = 0x1008                ¦
¦  pxPrev = 0x4000                ¦ ?--- [CHANGED!] Points back to Task 1
+---------------------------------+

[ 0x1008: xListEnd (Anchor Wall) ]
+---------------------------------+
¦ xItemValue = 0xFFFFFFFF         ¦
¦ pxNext = 0x4000                 ¦
¦ pxPrev = 0x5000                 ¦ ?--- [CHANGED!] Now points back to Task 2
+---------------------------------+

[ 0x4000: Task 1 Container ]
+---------------------------------+
¦  pxNext = 0x5000                ¦ ?--- [CHANGED!] Now points forward to Task 2
¦  pxPrev = 0x1008                ¦
+---------------------------------+

[ 0x1000: pxList (Master Folder Container) ]
+---------------------------------+
¦ uxNumberOfItems = 1             ¦ (Will increment to 2 on line 9)
¦ pxIndex = 0x5000                ¦ ?--- [CHANGED!] Master Bookmark shifts to Task 2
+---------------------------------+


========================================================================================================================
STARTING HOOKED LAYOUT IN RAM (BEFORE REMOVAL BEGINS)
========================================================================================================================
                                                                              [ 0x1000: pxList (Master DB) ]
                                                                              +---------------------------------+
                                                                              ¦ uxNumberOfItems = 1             ¦
                                                                              ¦ pxIndex = 0x5000 --------+      ¦  <=== Master Pointer shifted to New Task
                                                                              +--------------------------+------+
                                                                                                         |
                                                                                                         v                                                        
              +------------------------------------------------+             +------------------------------------------------+     +------------------------------------------------+     
              ¦ NODE: Task 1                                   ¦             ¦ NODE: Task 2                                   ¦       ¦ NODE: xListEnd (Anchor Wall)                   ¦  
              ¦ Memory Address: 0x4000                         ¦             ¦ Memory Address: 0x5000                         ¦       ¦ Memory Address: 0x1008                         ¦  
              +------------------------------------------------¦             +------------------------------------------------¦       +------------------------------------------------¦  
              ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦             ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦       ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦  
 ------------>¦   (0x1008)   ¦       1000       ¦   (0x5000) --+------------>¦   (0x4000)   ¦       2000       ¦   (0x1008) --+---> ¦   (0x5000)   ¦    0xFFFFFFFF    ¦   (0x4000)   +--
¦             +------------------------------------------------+             +------------------------------------------------+     +------------------------------------------------+  ¦
¦                                                                                                                                                                                       ¦                  
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
   so when new task2 comes:- (basically new task2 gets inserted between task1 and end list
    
    1.previous task1's pxNext (which was pointing to 0x1008) changes to task2 0x5000 
    2.task2's pxNext changes to xListEnd 0x1008, task2's pxPrevious changes to task1 0x4000
    3.the xListEnd's pxPrevious (which was pointing to 0x4000)changes to task2 0x5000

*/

void vListInsertEnd( xList *pxList, xListItem *pxNewListItem )//master DB, new TCB list
{
    volatile xListItem * pxIndexLocal;//acts like a temporary variable swap

   /* Insert a new list item into pxList, but rather than sort the list,
   makes the new list item the last item to be removed by a call to
   pvListGetOwnerOfNextEntry.  This means it has to be the item pointed to by
   the pxIndex member. */

   //copy anchor wall pointer from master DB (anchor wall local ptr <- master DB pointer)
   pxIndexLocal = pxList->pxIndex;//cutrrent list item (initially this will be the listEnd)

   //update newitem's double linked list pointers from anchor wall(both left and right)(newitem local ptr <- anchor wall pointer)
   pxNewListItem->pxNext = pxIndexLocal->pxNext;//TCB next list item = task in front of current task(xListEnd)
   pxNewListItem->pxPrevious = pxList->pxIndex;//TCB previous list item = current running task(xListEnd)

   //update anchor wall's double linked list pointers to newitems's(both left and right)
   pxIndexLocal->pxNext->pxPrevious = ( volatile xListItem * ) pxNewListItem;//
   pxIndexLocal->pxNext = ( volatile xListItem * ) pxNewListItem;

    //now replace the anchorwall with newitem in the master DB (anchor wall ptr <- newitem pointer)
   pxList->pxIndex = ( volatile xListItem * ) pxNewListItem;

   /* Remember which list the item is in. */
   pxNewListItem->pvContainer = ( void * ) pxList; //newitems list will point to master DB

   ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/
/*
                                         [ pxDelayedTaskList ]
                                         +-----------------------------+
                                         ¦ uxNumberOfItems = 1         ¦
                                         +-----------------------------+
                                                        ¦
                                                        V
     +-----------------------------------+    +-----------------------------------+
     ¦ NODE: Task 1                      ¦    ¦ NODE: xListEnd (Anchor Wall)      ¦
     ¦ Memory Address: 0x4000            ¦    ¦ Memory Address: 0x1008            ¦
     ¦ xItemValue = 2100                 ¦    ¦ xItemValue = 0xFFFFFFFF           ¦
     +-----------------------------------¦    +-----------------------------------¦
     ¦ pxPrevious ¦ xItemValue ¦ pxNext  ¦    ¦ pxPrevious ¦ xItemValue ¦ pxNext  ¦
 --->¦  (0x1008)  ¦    2100    ¦ (0x1008)+--->¦  (0x4000)  ¦ 0xFFFFFFFF ¦ (0x4000)+---
¦    +-----------------------------------+    +-----------------------------------+   ¦
+-------------------------------------------------------------------------------------+
*/
void vListInsert( xList *pxList, xListItem *pxNewListItem )
{
    volatile xListItem *pxIterator;
    portTickType xValueOfInsertion;

   /* Insert the new list item into the list, sorted in ulListItem order. */
   xValueOfInsertion = pxNewListItem->xItemValue;

   /* If the list already contains a list item with the same item value then
   the new list item should be placed after it.  This ensures that TCB's which
   are stored in ready lists (all of which have the same ulListItem value)
   get an equal share of the CPU.  However, if the xItemValue is the same as
   the back marker the iteration loop below will not end.  This means we need
   to guard against this by checking the value first and modifying the
   algorithm slightly if necessary. */
   if( xValueOfInsertion == portMAX_DELAY )//0xffffffff
   {
      pxIterator = pxList->xListEnd.pxPrevious;
   }
   else
   {
      /***************************** NOTE **********************************
      If you find your application is crashing here then likely causes are:
         1) Stack overflow -
            see http://www.freertos.org/Stacks-and-stack-overflow-checking.html
         2) Incorrect interrupt priority assignment, especially on Cortex-M3
            parts where numerically high priority values denote low actual
            interrupt priories, which can seem counter intuitive.  See
            configMAX_SYSCALL_INTERRUPT_PRIORITY on http://www.freertos.org/a00110.html
         3) Calling an API function from within a critical section or when
            the scheduler is suspended.
         4) Using a queue or semaphore before it has been initialised or
            before the scheduler has been started (are interrupts firing
            before vTaskStartScheduler() has been called?).
      See http://www.freertos.org/FAQHelp.html for more tips.
      **********************************************************************/

      for( pxIterator = ( xListItem * ) &( pxList->xListEnd ); pxIterator->pxNext->xItemValue <= xValueOfInsertion; pxIterator = pxIterator->pxNext )
      {
         /* There is nothing to do here, we are just iterating to the wanted insertion position. */
      }
   }

   pxNewListItem->pxNext = pxIterator->pxNext;//task1 's next will point to ListEnd
   pxNewListItem->pxNext->pxPrevious = ( volatile xListItem * ) pxNewListItem;//ListEnd will point to task1
   pxNewListItem->pxPrevious = pxIterator;//task1's previous will point to ListEnd
   pxIterator->pxNext = ( volatile xListItem * ) pxNewListItem;//ListEnd's next will point to task1

   /* Remember which list the item is in.  This allows fast removal of the
   item later. */
   pxNewListItem->pvContainer = ( void * ) pxList;//task1s pvcontainer will have Delayed DB address

   ( pxList->uxNumberOfItems )++;
}
/*-----------------------------------------------------------*/
/*Its as if like task1 is completely gone and only task2 exist

[ 0x1000: pxList (Master DB ) ]
 +---------------------------------+
 ¦ uxNumberOfItems = 0             ¦
 ¦ pxIndex = 0x1008 --------+      ¦
 +--------------------------+------+
                            ¦
                            ?
               +---------------------------------+
               ¦ 0x1008: xListEnd (Anchor Wall)  ¦
               ¦                                 ¦
         +----?¦  pxNext = 0x1008                ¦
         ¦     ¦  pxPrev = 0x1008                ¦
         +---------------------------------------+

------------ Example with task1 ------------------

                                             [ 0x1000: pxList (Master DB) ]
                                             +---------------------------------+
                                             ¦ uxNumberOfItems = 1             ¦
                                             ¦ pxIndex = 0x4000 --------+      ¦  <=== Master Pointer shifted to New Task
                                             +--------------------------+------+
                                                                        |
                                                                        v
    +---------------------------------+        +---------------------------------+   
    ¦ 0x1008: xListEnd (Anchor Wall)  ¦        ¦ 0x4000:  Task1 Container        ¦   
    +---------------------------------+        +---------------------------------+   
¦-->¦ pxPrev=0x4000 | pxNext=0x4000   +------->¦ pxPrev=0x1008 | pxNext=0x1008   ¦-->¦
¦   +---------------------------------+        +---------------------------------+   ¦
¦                                                                                    ¦
¦------------------------------------------------------------------------------------¦

------------ Example with task2 ------------------

                                                                               [ 0x1000: pxList (Master DB) ]
                                                                               +---------------------------------+
                                                                               ¦ uxNumberOfItems = 1             ¦
                                                                               ¦ pxIndex = 0x5000 --------+      ¦  <=== Master Pointer shifted to New Task
                                                                               +--------------------------+------+
                                                                                                          |
                                                                                                          v                                                        
              +------------------------------------------------+             +------------------------------------------------+     +------------------------------------------------+     
              ¦ NODE: Task 1                                   ¦             ¦ NODE: Task 2                                   ¦       ¦ NODE: xListEnd (Anchor Wall)                   ¦  
              ¦ Memory Address: 0x4000                         ¦             ¦ Memory Address: 0x5000                         ¦       ¦ Memory Address: 0x1008                         ¦  
              +------------------------------------------------¦             +------------------------------------------------¦       +------------------------------------------------¦  
              ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦             ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦       ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦  
 ------------>¦   (0x1008)   ¦       1000       ¦   (0x5000) --+------------>¦   (0x4000)   ¦       2000       ¦   (0x1008) --+---> ¦   (0x5000)   ¦    0xFFFFFFFF    ¦   (0x4000)   +--
¦             +------------------------------------------------+             +------------------------------------------------+     +------------------------------------------------+  ¦
¦                                                                                                                                                                                       ¦                  
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

                                                                                                        [ 0x1000: pxList (Master DB) ]
                                                                                                       +---------------------------------+
                                                                                                       ¦ uxNumberOfItems = 1             ¦
                                                                                                       ¦ pxIndex = 0x5000 --------+      ¦  <=== Points to remaining Task 2
                                                                                                       +--------------------------+------+
                                                                                                                         |
                                                                                                                         v                                                        
                                             +------------------------------------------------+     +------------------------------------------------+     
                                             ¦ NODE: Task 2                                   ¦       ¦ NODE: xListEnd (Anchor Wall)                   ¦  
                                             ¦ Memory Address: 0x5000                         ¦       ¦ Memory Address: 0x1008                         ¦  
                                             +------------------------------------------------¦       +------------------------------------------------¦  
                                             ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦       ¦  pxPrevious  ¦    xItemValue    ¦    pxNext    ¦  
 ------------------------------------------->¦   (0x1008)   ¦       2000       ¦   (0x1008) --+---> ¦   (0x5000)   ¦    0xFFFFFFFF    ¦   (0x5000)   +--
¦                                            +------------------------------------------------+     +------------------------------------------------+  ¦
+-------------------------------------------------------------------------------------------------------------------------------------------------------+
*/
/*
  We are breaking the relation ship of the TCB to the the master DB list
  1.Previous pointer of Endlist will point to task standing before deleted task,or if no task exist,it points to Endlist
  2.Next pointer of Endlist will point to task standing before deleted task,or if no task exist,it points to Endlist
  3.Maste DB's pointer must be replaced with eithe the task standing before or xEndList
  4.Dettach Master DB in task TCB, now task 1 doesnt belong to any DB.
  5.The number of tasks in the original DB has to be reduced
*/
unsigned portBASE_TYPE uxListRemove( xListItem *pxItemToRemove )//pxItemToRemove is task1
{
    xList * pxList;

   pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;//we are reverting back , (task2  will now point to xEndList), if no task2 then xEndList will point to itself
   pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;//same way , xEndList  will now point to task2, if no task2 then xEndList will point to itself

   /* The list item knows which list it is in.  Obtain the list from the list
   item. */
   pxList = ( xList * ) pxItemToRemove->pvContainer;//pxList contains master DB address , the task1's TCB is marked/attached to master DB

   /* Make sure the index is left pointing to a valid item. */
   if( pxList->pxIndex == pxItemToRemove )//i.e is the master DB's entry(0x1008) contains task1 list? the it has to be replaced with xEndList
   {
      //this is to remove task1 from master DB since task1 will be put in delayed list not in master DB list
      pxList->pxIndex = pxItemToRemove->pxPrevious;//no task2 will sit in Master DB' main pointer, if no task2 then xEndList will sit
   }

   pxItemToRemove->pvContainer = NULL;//now that attachement of master DB to task1 should be deleted as there is no relatioship, the tasks TCB is not owned by any Master DB
   ( pxList->uxNumberOfItems )--;

   return pxList->uxNumberOfItems;
}
/*-----------------------------------------------------------*/

