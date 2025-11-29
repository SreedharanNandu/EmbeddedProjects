//filename:- led.c
/***********************************************************************************
System Includes
***********************************************************************************/
#include "pic12f1822.h"

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
#include "string.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/***********************************************************************************
User Includes
***********************************************************************************/
#include "led.h"
#include "lin_cfg.h"


#define MAX_BRIGHTNESS    255u
#define DC_100            100u
#define PWM_PERIOD_TICKS  10u


#define Get_DutyTicks_From_Table(x)  BrightToDuty[x]

/**********************************************************************************
Global variables
***********************************************************************************/
const unsigned char BrightToDuty[101] = 
{
    // 0–9   ? 0
    0,0,0,0,0,0,0,0,0,0,
    // 10–19 ? 1
    1,1,1,1,1,1,1,1,1,1,
    // 20–29 ? 2
    2,2,2,2,2,2,2,2,2,2,
    // 30–39 ? 3
    3,3,3,3,3,3,3,3,3,3,
    // 40–49 ? 4
    4,4,4,4,4,4,4,4,4,4,
    // 50–59 ? 5
    5,5,5,5,5,5,5,5,5,5,
    // 60–69 ? 6
    6,6,6,6,6,6,6,6,6,6,
    // 70–79 ? 7
    7,7,7,7,7,7,7,7,7,7,
    // 80–89 ? 8
    8,8,8,8,8,8,8,8,8,8,
    // 90–99 ? 9
    9,9,9,9,9,9,9,9,9,9,
    //100
    10
};

unsigned char pwmCounter;
unsigned char dutyTick;

/***********************************************************************************
     User Program Code
***********************************************************************************/

/**********************************************************************************
Function Name:  
Description:    
Parameters:     none
Return value:   none
***********************************************************************************/
void App_Notify_Frame_Received_USART(unsigned char id_index)
{
   unsigned char led_data;
   if(Lin_Msg[id_index].id == 0x42u)
   {
      memcpy(&led_data,Lin_Msg[id_index].buf_ptr,Lin_Msg[id_index].len);
      if(led_data <= DC_100)
      {
         dutyTick = Get_DutyTicks_From_Table(led_data);
      }
   }
}

/**********************************************************************************
Function Name:  
Description:    
Parameters:     none
Return value:   none
***********************************************************************************/
void App_Led_Process(void) 
{
   pwmCounter++;
   if(pwmCounter >= PWM_PERIOD_TICKS)
   {  
      pwmCounter = 0;
   }
   // Calculate the number of timer ticks that should be ON based on duty cycle
   if(pwmCounter < dutyTick) 
   {
      RA0 = 1u;  // Set pin high (LED ON)
   }
   else
   {
      RA0 = 0u;  // Set pin low (LED OFF)
   }
}

