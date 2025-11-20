//filename :-App_Serial.c
/******************************************************************************
**                  S Y S T E M     I N C L U D E S
******************************************************************************/

#include <CG_macrodriver.h>
#include "system_def.h"
#include "HAL_PortPin_API.h"

/******************************************************************************
**                  P R O J E C T   I N C L U D E S
******************************************************************************/
#include "HAL_Timer_API.h"
#include "HAL_Init.h"
#include "HAL_ADC_API.h"
#include "HAL_USART.h"
#include "HAL_Timer_API.h"
#include "App_Serial.h"

#include "string.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/
#define FILTER_LEN                7u
#define STOP_BEEP_COUNT           300u
#define LIN_DEFAULT_COUNT         10u
#define _XS_CNT                   40u
#define LIN_PERIODIC_MSG_CNT      4u //5ms x 4times x 5ms gap = 100ms
#define MAX_BUZZ                  10u

/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
const buzzerPattern_t Buzzer[BUZZER_ZONE_MAX] = 
{
   {0xFFFFu, 0u},    // Always ON   [BUZZER_FEATURE_CONT]  BUZZER_ZONE0      
   {5u, 10u},        //  ON,  OFF   [BUZZER_FEATURE_TONE1] BUZZER_ZONE1      
   {5u, 60u},        // ON,  OFF   [BUZZER_FEATURE_TONE2]  BUZZER_ZONE2     
   {5u, 90u},        // ON,  OFF   [BUZZER_FEATURE_TONE3]  BUZZER_ZONE3     
   {0u,  0u},        // Always OFF  [BUZZER_FEATURE_MUTE]  BUZZER_ZONE4
};

// Each row is {min_distance, max_distance} in cm
const u8 Buzzer_Range_Map[BUZZER_ZONE_MAX][2] = 
{
   {  0u, 30u},   // L0
   { 31u, 60u},   // L1
   { 61u, 99u},   // L2
   {100u, 139u},  // L3
   {150u, 255u}   // L4
};


const buzzerZone_t buzzerZoneMap[BUZZER_ZONE_MAX][BUZZER_ZONE_MAX] = 
{
   //     R0             R1             R2          R3            R4
   { BUZZER_ZONE0, BUZZER_ZONE0, BUZZER_ZONE0, BUZZER_ZONE0, BUZZER_ZONE0 }, // L0 
   { BUZZER_ZONE0, BUZZER_ZONE1, BUZZER_ZONE1, BUZZER_ZONE1, BUZZER_ZONE1 }, // L1 
   { BUZZER_ZONE0, BUZZER_ZONE1, BUZZER_ZONE2, BUZZER_ZONE2, BUZZER_ZONE2 }, // L2 
   { BUZZER_ZONE0, BUZZER_ZONE1, BUZZER_ZONE2, BUZZER_ZONE3, BUZZER_ZONE3 }, // L3 
   { BUZZER_ZONE0, BUZZER_ZONE1, BUZZER_ZONE2, BUZZER_ZONE3, BUZZER_ZONE4 }  // L4 
};


volatile u8 buzzer_state;
volatile u16 buzzer_timer;
volatile buzzerZone_t curr_zone[SENSOR_MAX]={BUZZER_ZONE4,BUZZER_ZONE4},final_zone;
volatile u16 stop_count;
volatile buzzerZone_t prev_zone;
volatile u8 distance_valid[SENSOR_MAX];
volatile u8 Lin_Comm_Count;
volatile u8 Lin_Comm_Count0;
volatile u8 dist_dbg[SENSOR_MAX];
u8 Distance_Received[7];

u8 Lin_Next_Transmit_Msg_Timer0;

u16 SwV_in_mV,PinV_in_mV;

/**************************************************************************
 *       local prototypes
 *************************************************************************/
static void Init_Buzz(void);
static void App_Chime_Task(void);
void bin_to_strhex(unsigned char *bin, unsigned int binsz, char **result);


/******************************************************************************
**      F U N C T I O N      D E F I N I T I O N S
******************************************************************************/

/******************************************************************************
* Function Name     : Init_APP
* Input Params      : None
* Output Params     : None
* Description       : This fuction is called initially
******************************************************************************/
void Init_APP(void)
{
   prev_zone = BUZZER_ZONE4;//No beep
   LIN_Initialize();
   Init_Buzz();
   
}

/******************************************************************************
* Function Name     : APP_Task_10ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_10ms(void)
{
   App_Dist_Task();
   App_Chime_Task();
   App_Read_Sw();
}

/******************************************************************************
* Function Name     : App_Read_Sw
* Input Params      : None
* Output Params     : None
* Description       : This fuction reads switch
******************************************************************************/
void App_Read_Sw(void)
{
   u16 tempAdc = 0u;
   if(HAL_ADC_Read_Counts(&tempAdc))
   {
      if(tempAdc != 0xFFFFu)
      {
         PinV_in_mV = ((u32)tempAdc * 825u)/256u;
         SwV_in_mV = (tempAdc * 93u)/5u;
      }
   }
}

/******************************************************************************
* Function Name     : APP_Task_5ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_5ms(void)
{
   
   if(Lin_Periodic_Timer0 < LIN_PERIODIC_MSG_CNT)
   {
      Lin_Periodic_Timer0++;
   }
   else
   {
      Lin_Periodic_Timer0 = 0;
      //next frame transmit offset timer i.e between frames (not period)
      if( Lin_Next_Transmit_Msg_Timer0 > 1)//5msec skip 
      {
         Lin_Next_Transmit_Msg_Timer0--;
      }
      else if(Lin_Next_Transmit_Msg_Timer0 == 1)//10msec over after checksum of either 14 or 5e transmitted previously,not next one
      {
         Lin_Next_Transmit_Msg_Timer0--;
         Lin_Next_Index0++;
         if(Lin_Next_Index0 >= NO_OF_TX_ID)//mean all frames for that period is over,wait for next schedule/period
         {
            Lin_Next_Index0 = 0;
         }
         LIN_Send_Wakeup_Signal0();
      }
      else //10msec over all frames i mean 0x14 and 0x5E are transmittd
      {
      }
   }
   
}
 /******************************************************************************
* Function Name     : APP_Task_100ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_100ms(void)
{
   static u8 _xs_cnter;
   
   if(_xs_cnter < _XS_CNT)
   {
      _xs_cnter++;
      Set_Led(0);
   }
   else 
   {
      if(_xs_cnter >= _XS_CNT)
      {
         _xs_cnter=0u;
         Set_Led(1);
      }
   }   
   



}
/****************************************************************************
*. Name:App_Dist_Task
*.
*.   Description:
*.
*.   Warning: None
*.
*. Shared Variables: 
*.
*. Parameters: 
*.
*. Return Value: 
*.
*.
******************************************************************************/
void App_Dist_Task(void)
{
   u8 i=0u,j=0u;
   u8 dist[SENSOR_MAX]={0u,0u};
   u8 temp = 0u;
   static u8 prev_dist[SENSOR_MAX];
   
   if(Lin_Comm_Count >= LIN_DEFAULT_COUNT)
   {
      Distance_Received[5] = 0u;
      Distance_Received[6] = 0u;
      Set_Led1(1);
   }
   else
   {
      Lin_Comm_Count++;
      Set_Led1(0);
   }
   
   if(Lin_Comm_Count0 >= LIN_DEFAULT_COUNT)
   {
      Set_Led2(1);
   }
   else
   {
      Lin_Comm_Count0++;
      Set_Led2(0);
   }

   for(j=0;j<3;j++)
   {
      Lin_Tx_Msg[0].buf_ptr[j] = Distance_Received[j];
      lin_tx_data[0].transmit_data[j] = Distance_Received[j] ;
   }
   
   if(Distance_Received[3] != 0)
   {
      lin_tx_data[0].transmit_data[3] = Lin_Tx_Msg[0].buf_ptr[3] = Distance_Received[5];
   }
   else
   {
      if(Distance_Received[3] == 0)
      {
         lin_tx_data[0].transmit_data[3] = Lin_Tx_Msg[0].buf_ptr[3] = 0xFFu;
      }
   }
   if(Distance_Received[4] != 0)
   {
      lin_tx_data[0].transmit_data[4] = Lin_Tx_Msg[0].buf_ptr[4] = Distance_Received[5];
   }
   else
   {
      if(Distance_Received[4] == 0)
      {
         lin_tx_data[0].transmit_data[4] = Lin_Tx_Msg[0].buf_ptr[4] = 0xFFu;
      }
   }
   if(Distance_Received[5] != 0)
   {
      lin_tx_data[0].transmit_data[5] = Lin_Tx_Msg[0].buf_ptr[5] = Distance_Received[6];
   }
   else
   {
      if(Distance_Received[5] == 0)
      {
         lin_tx_data[0].transmit_data[5] = Lin_Tx_Msg[0].buf_ptr[5] = 0xFFu;
      }
   }
   if(Distance_Received[6] != 0)
   {
      lin_tx_data[0].transmit_data[6] = Lin_Tx_Msg[0].buf_ptr[6] = Distance_Received[6];
   }
   else
   {
      if(Distance_Received[6] == 0)
      {
         lin_tx_data[0].transmit_data[6] = Lin_Tx_Msg[0].buf_ptr[6] = 0xFFu;
      }
   }

   
   
   //estimate current zones of sensors
   for(j=0;j<(u8)SENSOR_MAX;j++)
   {
      if(j == (u8)SENSOR_L)
      {
         if((Distance_Received[5] != 0u))
         {
            temp = Distance_Received[5];
            distance_valid[j] = 1;
         }
         else
         {
            distance_valid[j] = 0;
         }
      }
      else 
      {
         if(j == (u8)SENSOR_R)
         {
            if((Distance_Received[6] != 0u))
            {
               temp = Distance_Received[6];
               distance_valid[j] = 1;
            }
            else
            {
               distance_valid[j] = 0;
            }
         }
      }
      if(distance_valid[j])
      {
         dist[j] = temp;
         if(dist[j] != prev_dist[j])
         {
            for(i=0u;i<BUZZER_ZONE_MAX;++i) 
            {
               if((dist[j] >= Buzzer_Range_Map[i][0] )&& 
                  (dist[j] <= Buzzer_Range_Map[i][1])) 
               {
                  curr_zone[j] = (buzzerZone_t)i;
                  dist_dbg[j] = temp;
                  break;
               }
            }
            prev_dist[j] = dist[j];
         }
      }
   }

   if(distance_valid[SENSOR_L] && distance_valid[SENSOR_R])
   {
      //calculate one final combined zone for buzz
      final_zone = buzzerZoneMap[curr_zone[SENSOR_L]][curr_zone[SENSOR_R]];
      //final zone changed
      if(final_zone != prev_zone)
      {
         if(stop_count != 0u)
         {
            stop_count = 0u;
         }
         //final zone within range
         if(final_zone < BUZZER_ZONE_MAX) 
         {
            buzzer_timer = 0u;
            buzzer_state = 0u;
         }
         prev_zone = final_zone;
      }
      else//same final zone
      {   
         //count for like a variable threshold and then stop beeping 
         if(stop_count >= STOP_BEEP_COUNT)
         {
            if(final_zone != BUZZER_ZONE4)
            {
               final_zone = BUZZER_ZONE4;
               buzzer_timer = 0u;
               buzzer_state = 0u;
            }
         }
         else
         {
            stop_count++; 
         }
      }
   }
   else
   {
      if(final_zone != BUZZER_ZONE4)
      {
         prev_zone = final_zone = BUZZER_ZONE4;
         buzzer_timer = 0u;
         buzzer_state = 0u;
      }
   }
}



/****************************************************************************
*. Name:App_Chime_Task
*.
*.   Description:
*.
*.   Warning: None
*.
*. Shared Variables: 
*.
*. Parameters: 
*.
*. Return Value: 
*.
*.
******************************************************************************/
void App_Chime_Task(void)
{
   static u8 temp_zone;
   buzzerPattern_t *ptr;
   
   
   if((0u == buzzer_timer) && 
      (0u == buzzer_state))
   {
      temp_zone = (u8)final_zone;
      // MUTE
      Set_Buzz(0u);

   }
   ptr = &Buzzer[temp_zone];

   // Handle special cases
   if((ptr->on_time_10ms == 0u) && 
      (ptr->off_time_10ms == 0u))
   {
       // MUTE
       Set_Buzz(0u);
       buzzer_state = 0u;
       buzzer_timer = 0u;
       return;
   }

   if(ptr->on_time_10ms == 0xFFFFu) 
   {
       // CONTINUOUS HIGH
       Set_Buzz(1u);
       buzzer_state = 1u;
       buzzer_timer = 0u;
       return;
   }

   // Toggle logic
   if(buzzer_state) 
   {
       if (buzzer_timer < ptr->on_time_10ms) 
       {
           buzzer_timer++;
       }
       else 
       {
           buzzer_state = 0u;
           buzzer_timer = 0u;
           Set_Buzz(0u);
       }
   }
   else 
   {
       if (buzzer_timer < ptr->off_time_10ms) 
       {
           buzzer_timer++;
       }
       else
       {
           buzzer_state = 1u;
           buzzer_timer = 0u;
           Set_Buzz(1u);
       }
   }
}

/****************************************************************************
*. Name:
*. Description:
*. Warning: None
*. Shared Variables: 
*. Parameters: 
*. Return Value: 
*.
******************************************************************************/
static void Init_Buzz(void)
{
   u8 i;
   u16 delay_buzz = 500u;
   Set_Buzz(1);
   for(i=0;i<MAX_BUZZ;i++)
   {
      while(delay_buzz--);
   }
   Set_Buzz(0);
}
/******************************************************************************
**                  R E V I S I O N     H I S T O R Y
*******************************************************************************
** Please update in the following format:
** Revision No            :
** Date(dd/mm/yy)         :
** Author                 : 
** Description of change  :
**
 
*******************************************************************************/