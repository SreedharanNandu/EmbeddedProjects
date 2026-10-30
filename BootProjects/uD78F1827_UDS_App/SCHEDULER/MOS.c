/******************************************************************************

NAME:         MOS.c

DESCRIPTION:  This file contains functions required for cyclic scheduler
              
Date:         
Revision No:  
****************************************************************************/
#define _MOS_C


/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
#include <CG_macrodriver.h>

#include "system_def.h"
#include "MOS.h"
#include "MOS_cfg.h"




/******************************************************************************
**                  M A C R O   D E F I N I T I O N S
******************************************************************************/

#if(MOS_NUM_FAST_RATE_TASKS > 0 || MOS_NUM_SLOW_RATE_TASKS > 0)
    #define MOS_TIMED_TASKS_CONFIGURED 1
#endif

#if MAX_SW_TIMER_ID > 8
 #error MOS_cfg.h.C: Max number of timers should be less than 9.
#endif

#if MAX_SW_TIMER_ID > 0
  #if 2*MOS_SCHEDULER_TICK_RATE < MOS_SW_TIMER_BASE
    #error MOS_cfg.h.C: MOS_SW_TIMER_BASE should be at least 2 times the MOS_SCHEDULER_TICK_RATE.
  #endif
#endif   

/*
** Bit-mapped constants used to access trigger bits in "timed_task_trigger_bits".  
** Trigger bits are used to indicate when the associated class of timed task 
** processing is to be executed. 
*/
#define FR_TIMED_TASK_0        ((u16) 0x0001)
#define FR_TIMED_TASK_1        ((u16) 0x0002) 
#define FR_TIMED_TASK_2        ((u16) 0x0004) 
#define FR_TIMED_TASK_3        ((u16) 0x0008) 
#define FR_TIMED_TASK_4        ((u16) 0x0010) 
#define FR_TIMED_TASK_5        ((u16) 0x0020) 
#define FR_TIMED_TASK_6        ((u16) 0x0040) 
#define FR_TIMED_TASK_7        ((u16) 0x0080) 
#define SLOW_TICK_TRIGGER      ((u16) 0x0100)

/*
** The maximum number of consecutive loops that set trigger bits are serviced in 
** the CheckTriggerBits function. 
*/
#define MAX_TRIGGER_BIT_LOOPS 10

#define MOS_SW_TIMER_COUNT   (MOS_SW_TIMER_BASE/MOS_SCHEDULER_TICK_RATE)
/* 
** Macros to initialize Stack Lower and Upper boundries
*/
#define MOS_STACK_BOUNDRY_VAL 0xAA

#define MOS_STACK_START_ADDRESS (MOS_STACK_UPPER_BOUND + 1) 
#define MOS_STACK_END_ADDRESS   (MOS_STACK_LOWER_BOUND - 1) 

/******************************************************************************
**               D A T A
******************************************************************************/
/* Array of function pointers that contains function pointers of the functions
** that shall called during cold initialization of the ECU.    The list must be
** terminated with NULL, and an empty list (only the NULL) is acceptable.
*/
static void (*MOS_ColdInit_List_A[])(void) = 
{
  MOS_COLD_INIT_LIST
  ((void*)0) 
};

/* Array of function pointers that contains function pointers of the functions
** that that are listed in Round Robin List.    The list must be
** terminated with NULL, and an empty list (only the NULL) is acceptable.
*/
static void (*const MOS_RoundRobin_List_A[])(void) = 
{
  MOS_ROUND_ROBIN_LIST
  (void (*)(void))0
};

/* Index for  Round Robin List*/
static u8 fl_RR_index_U8 = 0;


/* Array of function pointers that contains function pointers of the functions
** that shall called before entering sleep.    The list must be
** terminated with NULL, and an empty list (only the NULL) is acceptable.
*/
static void (*MOS_Sleep_List_A[])(void) = 
{
  MOS_SLEEP_LIST
  ((void*)0) 
};

/*
** Structure for implementing timed tasks.
*/
 typedef struct               
 {
   void  (*task_name)(void); 
   u8 startup_load_value_U8; 
   u16 periodicity_U16;        
 } TIMED_TASK_S;              



/* 
* Memory allocation for product specific fast/slow rate timed tasks.
*/  
 #if (MOS_NUM_FAST_RATE_TASKS > 0)                                  
 u16 fl_fast_rate_tt_counters_U16A[ MOS_NUM_FAST_RATE_TASKS ]; 
 #if (MOS_TT_FAST_TICK_RATE > 1)
 u16 fl_fast_rate_tt_tick_counter_U16;                         
#endif                                                                   
 static const TIMED_TASK_S fl_fast_rate_tt_list_A[] =               
 {                                                                  
  MOS_FAST_TT_TICK_RATE_LIST                                     
  {(void (*)(void))0, 0,0}                                       
 };                                                                 
                                                                    
#endif                                                            

#if defined(MOS_TIMED_TASKS_CONFIGURED)
const u8 fl_trigger_mask_U8A[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
#endif

#if (MOS_NUM_SLOW_RATE_TASKS > 0)

static const TIMED_TASK_S fl_slow_rate_tt_list_A[] = 
{
  MOS_SLOW_TT_TICK_RATE_LIST
  {(void (*)(void))0, 0,0}
};
u16 fl_slow_rate_tt_counters_U16A[ MOS_NUM_SLOW_RATE_TASKS ];
u16 fl_slow_rate_tt_tick_counter_U16;
#endif

#if defined(MOS_TIMED_TASKS_CONFIGURED)
u16 fl_timed_task_trigger_bits_U16;
u16 fl_copy_timed_task_trigger_bits_U16;
#endif

#if MAX_SW_TIMER_ID > 0
volatile u16 fl_time_stamp_U16;
volatile u16 fl_timer_base_counter_U16;
u16 fl_sw_timers_U16A[ MAX_SW_TIMER_ID ]; 
u8 fl_sw_timer_status_U8A[ MAX_SW_TIMER_ID ]; 
#endif

u8 fl_scheduler_max_trigger_bit_loops_U8;


vu32 fl_stack_address_vu32;

/********************************************************************************** 
               Compiler or Micro specific data 
**********************************************************************************/


/******************************************************************************
**                  L O C A L   F U N C T I O N S
******************************************************************************/
static void MOS_TraverseVoidFunctionLst(void (*const (*p_top))(void));

#if defined(MOS_TIMED_TASKS_CONFIGURED)
static void MOS_Init_TimedTask_Counters(TIMED_TASK_S const *list_ptr,  u16 *counter_ptr);
static void MOS_Check_TriggerBits(void);
#endif

static void MOS_Start_Scheduler(void);
static void MOS_Run_Scheduler(void);
#if (MOS_NUM_FAST_RATE_TASKS > 0)
static void MOS_Check_FastRate_TimedTasks(void);
static void MOS_Schedule_FastRate_TimedTask(u8 task_id);
static void MOS_Service_Fast_Rate_Counters(void);
#endif

#if (MOS_NUM_SLOW_RATE_TASKS > 0)
static void MOS_Schedule_SlowRate_TimedTasks(TIMED_TASK_S const list_ptr[],  u16 counter_ptr[]);
#endif



/******************************************************************************
* Function Name     : main
* Input Params      : None
* Output Params     : None
* Description       : This is the "main" function that shall be called from 
                      start up code. This shall handle the scheduler tasks.
******************************************************************************/
void MOS_main(void)
{

  /* Disable all Interrupts */  
   MOS_Disable_All_Interrupts();


   /*
   ** Do Cold Initialization
   */
   fl_scheduler_max_trigger_bit_loops_U8 = 0;
   MOS_TraverseVoidFunctionLst(&MOS_ColdInit_List_A[0]);

   
   while (TRUE == TRUE)
   {
      
       MOS_Start_Scheduler();

       /* Execute the list of functions before entring into sleep*/
       MOS_TraverseVoidFunctionLst(&MOS_Sleep_List_A[0]);
      
       /* Call the function that executes sleep algorithm */
       MOS_Sleep();
    } /* End while(TRUE) */
}
   
/******************************************************************************
* Function Name     : MOS_TraverseVoidFunctionLst
* Input Params      : Pointer to the top of the Function List
* Output Params     : None
* Description        : This function shall Traverse the list array of function 
*           pointers. 
******************************************************************************/
static void MOS_TraverseVoidFunctionLst(void (*const (*p_top))(void))
{
   if (p_top != ((void *) 0))
   {
      while(*p_top != ((void (*)(void)) 0) )  /* While not NULL entry.*/
      {
         (**p_top)();      /* Call function.*/
         p_top++;          /* increment pointer.*/
      }
   }
   else
   {
      /* Do nothing */
   }
}
/******************************************************************************
* Function Name     : MOS_Start_Scheduler
* Input Params      : None
* Output Params     : None
* Description        : This function performs initialization activities for  
*                 scheduling   
******************************************************************************/
static void MOS_Start_Scheduler(void)
{
  
    
   fl_RR_index_U8 = 0;
   /*
   ** Initialize fast and slow rate timed task counters.  It is necessary to 
   ** initialize fast rate counters, and the fast/slow tick counters within
   ** a critical section to allow the fast and slow ticks to start together.
   ** The tick counters are set to 1 so that both ticks will occur the next
   ** time the scheduler ISR executes.  All timed task trigger bits must also 
   ** be cleared.   It is NOT necessary to initialize slow rate timed task counters 
   ** within  the critical section since the scheduler ISR does not access these 
   ** counters. 
   */    
#if defined(MOS_TIMED_TASKS_CONFIGURED)
   MOS_Disable_Scheduler_Interrupt();
   fl_timed_task_trigger_bits_U16 = 0;
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 0)
   MOS_Init_TimedTask_Counters(&fl_fast_rate_tt_list_A[0], &fl_fast_rate_tt_counters_U16A[0]);
   #if (MOS_TT_FAST_TICK_RATE > 1)
   fl_fast_rate_tt_tick_counter_U16 = 1;
   #endif
#endif

#if (MOS_NUM_SLOW_RATE_TASKS > 0)
   fl_slow_rate_tt_tick_counter_U16 = 1; 
#endif


#if (MOS_NUM_SLOW_RATE_TASKS > 0)
   MOS_Init_TimedTask_Counters(&fl_slow_rate_tt_list_A[0], &fl_slow_rate_tt_counters_U16A[0]);
#endif

#if defined(MOS_TIMED_TASKS_CONFIGURED)
   MOS_Enable_Scheduler_Interrupt(); 
#endif

   MOS_Run_Scheduler();

}
/******************************************************************************
* Function Name     : MOS_Run_Scheduler
* Input Params      : None
* Output Params     : None
* Description        : This is the main loop of the Scheduler. This handles time
*                 tasks, Round Robin Tasks
******************************************************************************/
static void MOS_Run_Scheduler(void)
{
   /*****************************************************************/
   /*****************************************************************/
   /**********   TOP OF MAIN SCHEDULING LOOP   **********************/
   /*****************************************************************/
   /*****************************************************************/
   do 
   {
        /*****************************************************************/
        /*****************************************************************/
        /**********   SECTION II -- SERVICE THE TRIGGERABLE TASKS ********/
        /*****************************************************************/
        /*****************************************************************/
       
       #if (defined(MOS_TIMED_TASKS_CONFIGURED))
       MOS_Check_TriggerBits();
       #endif    

 
        /*****************************************************************/
        /*****************************************************************/
        /*********   SECTION  IV--SERVICE THE CONTEXT SENSITIVE  *********/
        /*********   SCHEDULING LIST                             *********/
        /*****************************************************************/
        /*****************************************************************/
        /*
        ** Find next scheduled task to run as long as end of list is
        ** not reached.
        */ 
        if(MOS_RoundRobin_List_A[fl_RR_index_U8] != NULL)
        {
           (MOS_RoundRobin_List_A[fl_RR_index_U8])();
 
           ++fl_RR_index_U8;

           /*After every task in RR check if the system is ready to sleep */
    


         }
         else
         {
            fl_RR_index_U8 = 0;
         }    

         /*****************************************************************/
         /*****************************************************************/
         /*********   SECTION V--CHECK FOR END OF CURRENT LOOP    *********/
         /*********   AND IF AT END, DO END LOOP PROCESSING.      *********/
         /*****************************************************************/
         /*****************************************************************/
        if (0 == fl_RR_index_U8)
        {                
           #if (MOS_IS_WATCH_DOG_SERVICED_BY_OS == TRUE)
            MOS_Update_WatchDog_Timer();
            #endif
        }
   } while (TRUE == TRUE);
    
}

/******************************************************************************
* Function Name     : MOS_Init_TimedTask_Counters
* Input Params      : Pointer to the time task list
*                 Pointer to the time task counter array   
* Output Params     : None
* Description        : This function shall initialize the timed tasks counters
******************************************************************************/
#if ((MOS_NUM_FAST_RATE_TASKS > 0) || (MOS_NUM_SLOW_RATE_TASKS > 0))
static void MOS_Init_TimedTask_Counters(TIMED_TASK_S const *p_list_ptr,  u16 *p_counter_ptr)
{
   if(p_list_ptr != NULL)
   {
      while (p_list_ptr->task_name != ((void (*)(void)) 0u))
      {
         /*
         ** Load the counter with the startup load value + 1.  The plus 1 is due
         ** to the fact that triggering is done immediately upon entry to the 
         ** scheduler and timed task servicing uses decrement before test.  So,
         ** if the startup value in sched.cfg is 0, it will get loaded with 1
         ** here and decremented upon triggering, so scheduling will occur
         ** immediately upon scheduler startup.  Protect for overflow by not
         ** adding 1 if the startup value is 255. 
         */
         if (p_list_ptr->startup_load_value_U8 == 255U)
         {
            if(p_counter_ptr != NULL)
            {
               *p_counter_ptr = 255U;
            }
         }
         else
         {
            if(p_counter_ptr != NULL)
            {
               *p_counter_ptr = p_list_ptr->startup_load_value_U8 + 1U;
            }
         }
         ++p_list_ptr;
         ++p_counter_ptr;
      }
   }
}
#endif

/******************************************************************************
* Function Name     : MOS_Check_TriggerBits
* Input Params      : None
* Output Params     : None
* Description        : This function shall check the trigger bits for all the 
*                 timed tasks and schedule them accordingly.   
*****************************************************************************/
static void MOS_Check_TriggerBits(void)
{
#if (defined(MOS_TIMED_TASKS_CONFIGURED))
   u8 l_max_trigger_bit_loops_U8;
#endif

/*
** Get copy of trigger bits that will be checked first.
*/
   MOS_Disable_All_Interrupts();
#if (defined(MOS_TIMED_TASKS_CONFIGURED))
   fl_copy_timed_task_trigger_bits_U16 = fl_timed_task_trigger_bits_U16;
#endif
   MOS_Enable_All_Interrupts();

/*
** If timed tasks are defined, a loop is used to increase scheduling accuracy by
** staying in this function and scheduling timed tasks rather than return and
** process a RR task. Note that the maximum number of loops is MAX_TRIGGER_BIT_LOOPS
** This is done to avoid starvation of RR and  RAM check tasks and to update the watchdog.
*/
#if (defined(MOS_TIMED_TASKS_CONFIGURED))
  l_max_trigger_bit_loops_U8 = 0;     
  while ((l_max_trigger_bit_loops_U8 < MAX_TRIGGER_BIT_LOOPS) && (fl_copy_timed_task_trigger_bits_U16 != 0))
  {
     ++l_max_trigger_bit_loops_U8;
#endif   /* BEGINNING OF CHECK TRIGGER BIT LOOP */

/*
** Schedule fast rate timed tasks as necessary.
*/
#if (MOS_NUM_FAST_RATE_TASKS > 0)
     MOS_Check_FastRate_TimedTasks();
#endif

#if (MOS_NUM_SLOW_RATE_TASKS > 0)
/*** Service the slow rate timed tasks if the slow rate tick has occurred*/
     if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, SLOW_TICK_TRIGGER) != 0u)
     { 
        /*
         ** Servicing of the slow rate timed tasks is required.  Reload the slow tick
         ** counter to establish the next slow tick point and service the slow rate
         ** task list.
         */
         MOS_Disable_All_Interrupts();
         fl_slow_rate_tt_tick_counter_U16 = MOS_SLOW_TT_TICK_RATE;
         CLEARBIT(fl_timed_task_trigger_bits_U16, SLOW_TICK_TRIGGER);
         MOS_Enable_All_Interrupts();
         MOS_Schedule_SlowRate_TimedTasks(&fl_slow_rate_tt_list_A[0], &fl_slow_rate_tt_counters_U16A[0]);
     }
#endif


#if (defined(MOS_TIMED_TASKS_CONFIGURED))
     /*
      ** Get copy of trigger bits in preparation for another loop.
      */
      fl_copy_timed_task_trigger_bits_U16 = fl_timed_task_trigger_bits_U16;
   }  /* END OF CHECK TRIGGER BIT LOOP */

   if (l_max_trigger_bit_loops_U8 > fl_scheduler_max_trigger_bit_loops_U8)
   {
      fl_scheduler_max_trigger_bit_loops_U8 = l_max_trigger_bit_loops_U8;
   } 
#endif  

}

/******************************************************************************
* Function Name     : MOS_Check_FastRate_TimedTasks
* Input Params      : None
* Output Params     : None
* Description        : This function shall check the trigger bits for all the 
*                 timed tasks and schedule them accordingly.   
*****************************************************************************/
static void MOS_Check_FastRate_TimedTasks(void)
{
#if (MOS_NUM_FAST_RATE_TASKS > 0)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_0) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(0);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 1)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_1) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(1);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 2)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_2) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(2);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 3)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_3) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(3);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 4)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_4) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(4);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 5)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_5) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(5);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 6)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_6) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(6);
   }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 7)
   if (TESTBIT(fl_copy_timed_task_trigger_bits_U16, FR_TIMED_TASK_7) != 0u)
   {
      MOS_Schedule_FastRate_TimedTask(7);
   }
#endif
}
/******************************************************************************
* Function Name     : MOS_Schedule_FastRate_TimedTask
* Input Params      : Task ID
* Output Params     : None
* Description        : This function shall schedule the Fast rate time task 
*                 based on the input parameter.   
*****************************************************************************/
#if (MOS_NUM_FAST_RATE_TASKS > 0)
static void MOS_Schedule_FastRate_TimedTask(u8 task_id)
{
    TIMED_TASK_S const *list_ptr;
    u16       *counter_ptr;
    u16       l_reload_value_U16;
    
    list_ptr    = &fl_fast_rate_tt_list_A[ task_id ];
    counter_ptr = &fl_fast_rate_tt_counters_U16A[ task_id ];
   
    /*
   * Fetch the periodicity value(reload value)
   */
    l_reload_value_U16 =  (u8)list_ptr->periodicity_U16;
    
    
    /*
    ** Clamp reload value to 1...a value of 0 is not allowed (would cause counter underflow
    ** due to decrement before test).
    */
    if (l_reload_value_U16 == 0)
    {
      l_reload_value_U16 = 1;
    }
    
    list_ptr->task_name();

    /*
    ** Perform reload and clear trigger bit.
    */
    MOS_Disable_All_Interrupts();
    *counter_ptr = l_reload_value_U16; 
    CLEARBIT(fl_timed_task_trigger_bits_U16, fl_trigger_mask_U8A[task_id]);
    MOS_Enable_All_Interrupts();
  
    
    
}
#endif

/******************************************************************************
* Function Name     : MOS_Schedule_SlowRate_TimedTasks
* Input Params      : Address of the top of SR timed tasks
* Output Params     : Address of SR counter
* Description        : This function shall schedule the Slow rate time task 
*                 based on the input parameters.   
*****************************************************************************/
#if (MOS_NUM_SLOW_RATE_TASKS > 0)
static void MOS_Schedule_SlowRate_TimedTasks(TIMED_TASK_S const *list_ptr,  u16 *counter_ptr)
{
   if(list_ptr != NULL)
   {
      /*
      ** Traverse the list of Slow Rate Timed Tasks.  Decrement the associated counter
      ** and Schedule the task when counter reaches 0.
      */
      while (list_ptr->task_name !=  ((void (*)(void)) 0u))
      {
           /*
           ** There is no test before decrement (to avoid underflow) in servicing
           ** timed task counters because a counter will never be zero upon
           ** entry to this function. 
           */

           if(counter_ptr != NULL)
           {
              --*counter_ptr;
              if (*counter_ptr == 0u)
              {
                 /*
               ** Reload the corresponding task's counter with its periodicity
               */
                 list_ptr->task_name();
                 *counter_ptr = list_ptr->periodicity_U16;
                 if (*counter_ptr == 0U)
                 {
                    /*
                    ** 0 is not a valid reload value, clamp it to 1.
                    */
                    *counter_ptr = 1U;
                 }
              }
              ++list_ptr;
              ++counter_ptr;
          }
       }
   }
}
#endif

/******************************************************************************
* Function Name     : MOS_Scheduler_ISR
* Input Params      : void
* Output Params     : void
* Description        : Interrupt function taken in response to a periodic interrupt. 
****************************************************************************/
void MOS_Scheduler_ISR(void)
{
    /* Interact with the Timer hardware to clear the interrupt and make    */
    /* way for another one.  It is better to do this in the beginning,     */
    /* because this will decrease the probabilities of an interrupt being  */
    /* missed due to the ISR taking more time to execute.                  */ 
    MOS_SCHEDULER_ISR_OPERATIONS ;
    



   
  /***************************************************************************
   **  Service fast and slow timed tasks as configured. 
   ***************************************************************************/
#if (MOS_NUM_FAST_RATE_TASKS > 0)
    MOS_Service_Fast_Rate_Counters();
#endif 

#if (MOS_NUM_SLOW_RATE_TASKS > 0) 

#if (MOS_SLOW_TT_TICK_RATE > 1)
   --fl_slow_rate_tt_tick_counter_U16;
   if (fl_slow_rate_tt_tick_counter_U16 == 0)
#endif
   {
      SETBIT(fl_timed_task_trigger_bits_U16, SLOW_TICK_TRIGGER);
   }
#endif

}

/******************************************************************************
* Function Name     : MOS_Service_Fast_Rate_Counters
* Input Params      : None
* Output Params     : None
* Description        : This function shall called from Scheduler ISR to update
*           Fast Rate Time Task counters    
*****************************************************************************/
#if (MOS_NUM_FAST_RATE_TASKS > 0)
static void MOS_Service_Fast_Rate_Counters(void)
{
#if (MOS_TT_FAST_TICK_RATE > 1)
   --fl_fast_rate_tt_tick_counter_U16;
   if (fl_fast_rate_tt_tick_counter_U16 == 0)
   {
      fl_fast_rate_tt_tick_counter_U16 = MOS_TT_FAST_TICK_RATE;
#endif
      
#if (MOS_NUM_FAST_RATE_TASKS > 0)
      --fl_fast_rate_tt_counters_U16A[0];
      if (fl_fast_rate_tt_counters_U16A[0] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_0);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 1)
      --fl_fast_rate_tt_counters_U16A[1];
      if (fl_fast_rate_tt_counters_U16A[1] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_1);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 2)
      --fl_fast_rate_tt_counters_U16A[2];
      if (fl_fast_rate_tt_counters_U16A[2] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_2);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 3)
      --fl_fast_rate_tt_counters_U16A[3];
      if (fl_fast_rate_tt_counters_U16A[3] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_3);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 4)
      --fl_fast_rate_tt_counters_U16A[4];
      if (fl_fast_rate_tt_counters_U16A[4] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_4);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 5)
      --fl_fast_rate_tt_counters_U16A[5];
      if (fl_fast_rate_tt_counters_U16A[5] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_5);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 6)
      --fl_fast_rate_tt_counters_U16A[6];
      if (fl_fast_rate_tt_counters_U16A[6] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_6);
      }
#endif

#if (MOS_NUM_FAST_RATE_TASKS > 7)
      --fl_fast_rate_tt_counters_U16A[7];
      if (fl_fast_rate_tt_counters_U16A[7] == 0)
      {
         SETBIT(fl_timed_task_trigger_bits_U16, FR_TIMED_TASK_7);
      }
#endif

#if (MOS_TT_FAST_TICK_RATE > 1)
   }
#endif
}
#endif
/******************************************************************************
* Function Name     : MOS_Update_WatchDog_Timer
* Input Params      : None
* Output Params     : None
* Description        : This function shall Refresh the watchdog timer 
*****************************************************************************/
#if (MOS_IS_WATCH_DOG_SERVICED_BY_OS == TRUE)
void MOS_Update_WatchDog_Timer(void)
{
   MOS_REFRESH_WATCHDOG;
}
#endif

/******************************************************************************
**                  R E V I S I O N     H I S T O R Y  
*******************************************************************************
** Please update in the following format:
**
** Revision No :                   Author:
** Description of change:
**   
*******************************************************************************/
