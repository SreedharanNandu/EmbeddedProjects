/******************************************************************************
NAME:         MOS.h

DESCRIPTION:  This file contains definitions exported from MOS.c
              for public use.        
              
Date:      
Revision No: 
****************************************************************************/

#ifndef MOS_H
#define MOS_H



/***************************************************************************  
                I N C L U D E   F I L E S 
***************************************************************************/  
#include "system_def.h"
#include "MOS_cfg.h"

/***************************************************************************  
   P U B L I C   M A C R O   A N D   T Y P E   D E F I N I T I O N S
***************************************************************************/  
/*
** Values returned from MOS_CheckTimer().
*/
#define   MOS_TIMER_CLEAR       0x00
#define   MOS_TIMER_EXPIRED     0x01
#define   MOS_TIMER_RUNNING     0x80

#define RESETVECT             0x0000u
#define STARTUP_CODE          (*(volatile V_MEMROM1_FAR vuint16 V_MEMROM2_FAR *)(RESETVECT))
/* Jump to sub routine */
#define JSR(x)                (*((void(*)(void))(x)))()
#define JSR_RESET()           JSR(STARTUP_CODE)


/***************************************************************************  
   P U B L I C   F U N C T I O N   P R O T O T Y P E S 
***************************************************************************/  

/*
**    Function prototype for the main function in the kernel.  This        
**    can be invoked directly from the interrupt vector table or from      
**    startup code.                                                        
*/
extern void main(void);
#if (MOS_IS_WATCH_DOG_SERVICED_BY_OS == TRUE)
/* 
** Function to update/refresh WatchDog time                                 
*/
extern void MOS_Update_WatchDog_Timer(void);

#endif
/* 
** Function that shall be called in ISR used for generation Scheduler tick.                                 
*/

extern void MOS_Scheduler_ISR(void);

#if MAX_SW_TIMER_ID > 0

/* 
** Function to start the timer.                                 
*/
extern void  MOS_StartTimer(u16 p_timeout_time_U16, u8 p_timer_id_u8);

/* 
** Function to check the timer status.                                 
*/
extern u8 MOS_CheckTimer(u8 p_timer_id_u8);

/* 
** Function to clear the timer.                                 
*/
extern void MOS_CleartTimer(u8 p_timer_id_u8);
#endif
/******************************************************************************
**                  R E V I S I O N     H I S T O R Y  
*******************************************************************************
** Please update in the following format:
**
** Revision No      Date              Author:
** Description of change:Initial Release
*******************************************************************************/


#endif
