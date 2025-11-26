/******************************************************************************
NAME                    :MOS_cfg.h

DESCRIPTION             :This file contains a description of the macros and functions
                         that are used to configure the scheduler
            
Date                    :
Revision No(of CFG file):
****************************************************************************/
#ifndef MOS_CFG_H
#define MOS_CFG_H

/***************************************************************************  
                I N C L U D E   F I L E S 
***************************************************************************/  
#include <CG_macrodriver.h>

#include "HAL_Init.h"
#include "EAL_CAN.h"
#include "App_Uds.h"
#include "App_Can_Tp.h"

/***************************************************************************  
   P U B L I C   M A C R O   A N D   T Y P E   D E F I N I T I O N S
***************************************************************************/  



/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_SCHEDULER_TICK_RATE                                   */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  This macro must specify the rate of the scheduler ISR     */
/*               in milliseconds.                   */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_SCHEDULER_TICK_RATE   1


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_COUNTS_PER_SCHEDULER_ISR_TICK_RATE                    */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  This macro must specify the count for RTI or any timer's  */
/*       Output Compare counter that is used for scheduling        */   
/*                                       */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define  MOS_COUNTS_PER_SCHEDULER_ISR_TICK_RATE       1

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Enable_Scheduler_Interrupt()                            */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  Macro to Enable Scheduler Interrupt                       */  
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Enable_Scheduler_Interrupt() WAKEUPTM_Start() 

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Disable_Scheduler_Interrupt()                         */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  Macro to Disable Scheduler Interrupt                      */  
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Disable_Scheduler_Interrupt() WAKEUPTM_Stop() 


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_IS_WATCH_DOG_SERVICED_BY_OS                     */
/*                                                                         */
/* TYPE:         MACRO (TRUE/FALSE                                         */
/*                                                                         */
/* DESCRIPTION:  MACRO to check whether watchdog to be servied by OS or Not*/
/*                                                                         */
/*                                                              */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
//#define MOS_IS_WATCH_DOG_SERVICED_BY_OS 

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_REFRESH_WATCHDOG                                 */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  This macro must perform all operations required to update */
/*               the product specific watchdog.                            */
/*                                                                         */
/* EXAMPLE: for MC68HC11                                */            
/*#define  MOS_REFRESH_WATCHDOG \                       */            
/*          {                   \                                       */
/*            COPRST = 0x55;       \                            */
/*            COPRST = 0xAA;       \                                     */
/*          }                 \             */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#if (MOS_IS_WATCH_DOG_SERVICED_BY_OS == TRUE)
#define  MOS_REFRESH_WATCHDOG   
#endif     
         
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Disable_All_Interrupts()                              */
/*                                                                         */
/* TYPE:         MACRO                                                 */
/*                                                                         */
/* DESCRIPTION:  This Macro/function shall disable all interrupts          */
/*                                                                         */
/* EXAMPLE      #define MOS_Disable_All_Interrupts() _disable_interrupts() */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Disable_All_Interrupts()  DI() /*__asm( "di\n" )*/


  
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Enable_All_Interrupts()                               */
/*                                                                         */
/* TYPE:         MACRO/function                                            */
/*                                                                         */
/* DESCRIPTION:  This Macro/function shall disable all interrupts          */
/*                                                                         */
/* EXAMPLE      #define MOS_Enable_All_Interrupts() _Enable_interrupts()   */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Enable_All_Interrupts()  EI() /*__asm( "ei\n" )*/

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_SCHEDULER_ISR_OPERATIONS                              */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  This macro must clear the interrupt pending flag for the  */
/*               the scheduler interrupt and perform any other operations  */
/*               necessary to establish the interrupt at the next 2 or 4   */
/*               millisecond interval (e.g. set value of output compare    */
/*               register used to generate next interrupt). This macro     */
/*               is executed within the scheduler ISR.                     */
/*                                                                         */
/*       EXAMPLE: MC68HC11 using output compare 3.                  */                      
/*      {                                                                  */ 
/*       CLEARBIT(TFLG1, ~OC3F);                                         */
/*         TOC3 += TCNT_COUNTS_PER_SCHEDULER_ISR_RATE;                     */
/*      }                                              */
/*                                                  */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define  MOS_SCHEDULER_ISR_OPERATIONS    

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_NUM_FAST_RATE_TASKS                                   */
/*               MOS_NUM_SLOW_RATE_TASKS                                   */
/*               MOS_FAST_TT_TICK_RATE                                     */
/*               MOS_SLOW_TT_TICK_RATE                                     */
/*               MOS_FAST_TT_TICK_RATE_LIST                                */
/*               MOS_SLOW_TT_TICK_RATE_LIST                                */
/*                                                                         */
/* TYPE:         MACRO'S                                                   */
/*                                                                         */
/* DESCRIPTION:  These macros allow configuration of time triggered tasks. */
/*                                                                         */
/*               MOS_NUM_FAST_RATE_TASKS - Defines the number of           */
/*               tasks scheduled via the fast tick rate.  Must be the      */
/*               number of tasks in MOS_FAST_TT_TICK_RATE_LIST.  A         */
/*               value of 0 can be used to specify no tasks.  The maximum  */
/*               value is 8.                                               */
/*                                                                         */
/*               MOS_NUM_SLOW_RATE_TASKS - Defines the number of           */
/*               tasks scheduled via the slow tick rate.  Must be the      */
/*               number of tasks in MOS_SLOW_TT_TICK_RATE_LIST.  A         */
/*               value of 0 can be used to specify no tasks.               */
/*                                                                         */
/*               MOS_FAST_TT_TICK_RATE - defines the time base for         */
/*               tasks defined in MOS_FAST_TT_TICK_RATE_LIST.  The time    */
/*               base is the scheduler ISR rate (as defined in             */
/*               MOS_SCHEDULER_TICK_RATE) times this value.  For            */
/*               example, if the scheduler ISR rate is 2ms and this value  */
/*               is 4, the fast tick time base is 8ms.                     */
/*                                                                         */
/*               MOS_SLOW_TT_TICK_RATE - defines the time base for         */
/*               tasks defined in MOS_SLOW_TT_TICK_RATE_LIST.  The time    */
/*               base is the scheduler ISR rate (as defined in             */
/*               MOS_SCHEDULER_TICK_RATE) times this value.  For           */
/*               example, if the scheduler ISR rate is 2ms and this value  */
/*               is 5, the slow tick time base is 10ms.                    */
/*                                                                         */
/*                                                                         */
/*               The MOS_FAST_TT_TICK_RATE_LIST and                        */
/*               MOS_SLOW_TT_TICK_RATE_LIST macros to define a list of     */
/*               fast rate and slow rate timed task lists respectively.    */
/*               The format of each in the list defines one task as        */
/*               follows:                                                  */
/*                                                                         */
/*                {TASK_NAME, STARTUP_LOAD_VALUE,PERIODICITY}            */
/*                                                                         */
/*               TASK_NAME - the name of the task ("C" function name).     */
/*                                                                         */
/*               STARTUP_LOAD_VALUE - number of ticks that occur before    */
/*               the task is first scheduled upon scheduler startup        */
/*               following cold init, warm init, or wakeup.  The first     */
/*               tick occurs immediately when the scheduler is started, so */
/*               a value of 0 specifies immediately scheduling. A value of */
/*               10 would result in first scheduling occuring 10*time_base */
/*               units of time after the scheduler is started up. This     */
/*               parameter allows "delayed startup" on a task by task      */
/*               basis. The range of this parameter is 0..254.             */
/*                                                                         */
/*                                                                         */
/*      PERIODICITY - Rate at which the task has to be scheduled           */
/* EXAMPLE CONFIGURATION: 3 "fast rate" tasks (8ms, 16ms, 32ms) and 2      */
/*               "slow rate" tasks (10ms, 100ms) with a scheduler ISR rate */
/*               of 2ms.                                              */
/*                                                                         */
/*                                                                         */
/*       #define MOS_NUM_FAST_RATE_TASKS    3                              */
/*       #define MOS_NUM_SLOW_RATE_TASKS    2                              */
/*       #define MOS_FAST_TT_TICK_RATE      4                             */
/*       #define MOS_SLOW_TT_TICK_RATE      5                              */
/*                                                                         */
/*       #define MOS_FAST_TT_TICK_RATE_LIST        \                       */ 
/*               {task_8ms,  1, 1}, \                                      */
/*               {task_16ms, 8, 2}, \                                      */
/*               {task_32ms, 8, 4}, \                                      */
/*                                                                         */
/*       #define MOS_SLOW_TT_TICK_RATE_LIST          \                     */ 
/*               {task_10ms,  1,  1}, \                                    */
/*               {task_100ms, 8, 10},\                                     */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_NUM_FAST_RATE_TASKS    1
#define MOS_NUM_SLOW_RATE_TASKS    1

#define MOS_FAST_TT_TICK_RATE      1 //1msec
#define MOS_SLOW_TT_TICK_RATE     10 //10msec

#define MOS_FAST_TT_TICK_RATE_LIST \
             {CANTP_MainFunction,1,1},\

#define MOS_SLOW_TT_TICK_RATE_LIST \
             {UDS_MainFunction,1,2},\
                  




/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_ROUND_ROBIN_LIST                                    */
/*                                                                         */
/* TYPE:         MACRO                                                    */
/*                                                                         */
/* DESCRIPTION:  List all the Round Robin Tasks here                       */
/*                                                                         */
/* EXAMPLE      #define MOS_ROUND_ROBIN_LIST   \                        */
/*                      Task1_RR\                                      */
/*         Task2_RR\                                                        */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define  MOS_ROUND_ROBIN_LIST \

  
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_COLD_INIT_LIST                                        */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  This macro must contain a list of all functions that are  */ 
/*               to be executed during a cold start.  The functions are    */
/*               executed sequentially from the first function in the list */
/*               until the last. By convention, functions in this list     */
/*               have the suffix "_ColdInit".                              */
/*                                                                         */
/* REQUIRED:     YES (however, the list can be NULL)                       */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_COLD_INIT_LIST  \
   Init_HW,\
   EAL_CAN_Init,\
   Init_APP,\
   
   
    
  
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_SLEEP_LIST                                            */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  This macro must contain a list of all functions that are  */ 
/*               to be executed before entering to sleep.These functions   */
/*               are executed sequentially from the first function in      */
/*               the list until the last. By convention, functions         */
/*               in this list have the suffix "_Sleep".                    */
/*                                                                         */
/* REQUIRED:     YES (however, the list can be NULL)                       */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_SLEEP_LIST  \


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Request_For_Sleep()                                  */
/*                                                                         */
/* TYPE:         Function                                                  */
/*                                                                         */
/* DESCRIPTION:  User shall assign a function that shall be called to      */
/*               check if the product is ready to go to sleep or not. This */
/*               function shall return TRUE in case the product is ready to*/
/*               go to sleep else FALSE                                    */
/*                                                                         */
/* EXAMPLE      #define MOS_Request_For_Sleep() function()                  */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define  MOS_Request_For_Sleep()     


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Sleep()                                               */
/*                                                                         */
/* TYPE:         Function                                                  */
/*                                                                         */
/* DESCRIPTION:  User shall assign a function that shall be called to      */
/*               make the product to shutdown/sleep.                       */
/*                                                                         */
/* EXAMPLE      #define MOS_SLEEP() function()                              */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Sleep()    
  
   
/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_SW_TIMER_BASE                                         */
/*                                                                         */
/* TYPE:         MACRO                                                     */
/*                                                                         */
/* DESCRIPTION:  Timer base for Generic Software timer in ms. This should  */ 
/*               be in multiples of Scheduler ISR tick rate(min is in      */
/*               Multiple of 2.                                            */
/*               for example if MOS_SCHEDULER_TICK_RATE is 2 then          */
/*               MOS_SW_TIMER_BASE can be 4,6,8,10 etc                     */
/*               Since the timer counter is 16 bit value, the maximum      */
/*               timeout dependes on the MOS_SW_TIMER_BASE value.          */
/*               In the above example, maximum time out that can be       */
/*               achieved with MOS_SW_TIMER_BASE 4 is 4*(2^16) = 4*65536   */
/*               = 262.144 s                                       */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/

#define MOS_SW_TIMER_BASE          2





/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_RAM_START_ADDRESS                                     */
/*                                                                         */
/* TYPE:         MACRO                                                    */
/*                                                                         */
/* DESCRIPTION:  MACRO for starting address of RAM                        */
/*                                                                         */
/* EXAMPLE      #define MOS_RAM_START_ADDRESS (u16)0x4000                */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_RAM_START_ADDRESS               0xFE700


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_RAM_END_ADDRESS                                       */
/*                                                                         */
/* TYPE:         MACRO                                                    */
/*                                                                         */
/* DESCRIPTION:  MACRO for ending address of RAM                        */
/*                                                                         */
/* EXAMPLE      #define MOS_RAM_END_ADDRESS (u16)0x6000                    */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_RAM_END_ADDRESS         0xFFEDF

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_RAM_Check_Failed_Error()                              */
/*                                                                         */
/* TYPE:         Function                                                */
/*                                                                         */
/* DESCRIPTION:  User shall assign a function that shall be called when    */
/*               RAM check fails. This function shall contain              */
/*               actions to be taken when RAM is corrupted.                */
/*               Fo eg: Logging DTC or resetting Micro etc.                */
/*                                                                         */
/* EXAMPLE      #define MOS_RAM_Check_Failed_Error() function()            */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_RAM_Check_Failed_Error()          


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_STACK_LOWER_BOUND                                     */
/*               MOS_STACK_UPPER_BOUND                                     */
/*                                                         */
/*                                                                         */
/* TYPE:         MACRO                                                    */
/*                                                                         */
/* DESCRIPTION:  MOS_STACK_LOWER_BOUND: Address immediately below stack    */
/*               MOS_STACK_UPPER_BOUND:   Address immediately above stack      */
/*                                                         */
/*                                                                         */
/* EXAMPLE      #define MOS_STACK_LOWER_BOUND (u16)0x35FF                   */
/*              #define MOS_STACK_UPPER_BOUND (u16)0x3800               */
/*                                                         */
/*            0x35FF -----------( Stack upper bound)                  */
/*            0x3600 -----------( Stack starting address)                */
/*                       |                                       */
/*                    |                                       */
/*                    |                                       */
/*            0x37FF -----------( Stack Ending Address)               */
/*            0x3800 -----------( Stack Lower Bound)                   */
/*                                                         */
/*                                                         */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_STACK_LOWER_BOUND             0xF0000

#define MOS_STACK_UPPER_BOUND             0xF0001

/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Stack_Check_OnInit_Error()                            */
/*                                                                         */
/* TYPE:         Function                                                */
/*                                                                         */
/* DESCRIPTION:  User shall assign a function that shall be called when    */
/*               Stack check fails on start up. This function shall contain*/
/*               actions to be taken when stack is corrupted.              */
/*               Fo eg: Logging DTC or resetting Micro etc.                */
/*                                                                         */
/* EXAMPLE      #define MOS_Stack_Check_OnInit_Error() function()          */
/*                                                                         */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Stack_Check_OnInit_Error()          


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Stack_Underflow_Error()                               */
/*                                                                         */
/* TYPE:         Macro to a function                                       */
/*                                                                         */
/* DESCRIPTION:  This function shall perform error action when Stack       */
/*               Underflow occur                                           */    
/*                                                                        */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Stack_Underflow_Error()         


/*-------------------------------------------------------------------------*/
/*                                                                         */
/* NAME:         MOS_Stack_Overflow_Error()                                */
/*                                                                         */
/* TYPE:         Macro to a function                                       */
/*                                                                         */
/* DESCRIPTION:  This function shall perform error action when Stack       */
/*               Overflow occur                                           */    
/*                                                                        */
/*                                                                         */
/* REQUIRED:     YES                                                       */
/*                                                                         */
/*-------------------------------------------------------------------------*/
#define MOS_Stack_Overflow_Error()             


#endif  /*EOF*/

/******************************************************************************
**                  R E V I S I O N     H I S T O R Y   FOR MOS CFG UPDATES
*******************************************************************************
** Please update in the following format:
**
** Revision No      Date              Author:
** Description of change:Initial Release
**
*******************************************************************************/


