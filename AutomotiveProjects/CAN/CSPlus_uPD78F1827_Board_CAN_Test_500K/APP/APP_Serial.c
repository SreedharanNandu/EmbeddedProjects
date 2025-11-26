
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
#include "INTF_VTMR_API.h"
#include "HAL_Timer_API.h"
#include "App_Serial.h"
#include "EAL_CAN.h" 
#include "HAL_CAN_API.h"
#include "App_Serial.h"
#include "string.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*****************************************************************************
*                                 Macro Definitions                          *
******************************************************************************/

/*****************************************************************************
*                                 Type Declarations                          *
******************************************************************************/
/*****************************************************************************
*                    Globally  accessed Variable Declarations    
******************************************************************************/
 u8 t1_enable , t2_enable , repeat_enable;
 u16 t1_cnt , t2_cnt , rpt_cnt;
 u16 K_T1 , K_T2 , K_RPT;
/**************************************************************************
 *       local prototypes
 *************************************************************************/
static void App_Chime_Task(void);
void bin_to_strhex(unsigned char *bin, unsigned int binsz, char **result);


/******************************************************************************
**      F U N C T I O N      D E F I N I T I O N S
******************************************************************************/

/******************************************************************************
* Function Name     : APP_Task_10ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_10ms(void)
{
   App_Chime_Task();
}

/******************************************************************************
* Function Name     : APP_Task_250ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_250ms(void)
{
   App_CAN_Data_Transmit_250ms();
}

/******************************************************************************
* Function Name     : APP_Task_500ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_500ms(void)
{
   App_CAN_Data_Transmit_500ms();
}

/******************************************************************************
* Function Name     : APP_Task_1s
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_1s(void)
{
   App_CAN_Data_Transmit_1s();
}

 /******************************************************************************
* Function Name     : APP_Task_100ms
* Input Params      : None
* Output Params     : None
* Description       : This fuction is calling periodically from scheduler
******************************************************************************/
void APP_Task_100ms(void)
{
   App_CAN_Data_Transmit_100ms();


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
static void App_Chime_Task(void)
{
 

   static u8 dc = 0;
   static u8 tone = 0;
   static u8 cadence = 0;
   static u8 repeat = 0;
   static u8 priority = 0;

   static Chime_T Prev_Chime_Command;
   
   
   if(
        (Prev_Chime_Command.byte[0] != Chime_Command.byte[0]) ||
        (Prev_Chime_Command.byte[1] != Chime_Command.byte[1]) ||
        (Prev_Chime_Command.byte[2] != Chime_Command.byte[2]) ||
        (Prev_Chime_Command.byte[3] != Chime_Command.byte[3]) ||
        (Prev_Chime_Command.byte[4] != Chime_Command.byte[4]) 
      )
   {
      tone = Chime_Command.bits.SC_SndTne;//750hz gong or beep, 2khz gong or beep
      cadence = Chime_Command.bits.SC_SndCdnPrd;//*10ms ,max 2550 
      repeat = Chime_Command.bits.SC_NmofRp;
      priority = Chime_Command.bits.SndPriority;

      //dc process
      if( (Chime_Command.bits.SC_SndDutCyc > 0x00u) &&
          (Chime_Command.bits.SC_SndDutCyc < 0xFFu)
        )
      {
         {   
            dc = (Chime_Command.bits.SC_SndDutCyc * 39u)/100u;//*.39, max 100 in %
            t1_enable = TRUE;
            repeat_enable = TRUE;
            K_T1  = (dc * cadence)/100; //in ms 
            K_T2  = (cadence - K_T1);//in ms    
            
            if(repeat == 0xFF)
            {
               K_RPT = 0xFFu;
            }
            else
            {
               K_RPT = 5u;
            }
            rpt_cnt = 0u;
         }
      }
      else if((repeat == 0u) && (Chime_Command.bits.SC_SndDutCyc == 0xFFu))//stop chiming
      {
         dc = 0xFF;
         t1_enable = TRUE;
         repeat_enable = TRUE;
         K_RPT = 0u;
         K_T1  = 1; //in ms 
         K_T2  = 100;//in ms    
      }
      else if((repeat == 0xFFu) && (Chime_Command.bits.SC_SndDutCyc == 0xFFu))//full long chime
      {
         {   
            dc = 0xFF;
            t1_enable  = TRUE;
            repeat_enable = TRUE;
            K_T1  = 1; //in ms 
            K_T2  = 0;//in ms    
            K_RPT = 0xFF; //permanent
            rpt_cnt = 0u;
         }
      }
      else 
      {
      }



       
      Prev_Chime_Command.byte[0] = Chime_Command.byte[0];
      Prev_Chime_Command.byte[1] = Chime_Command.byte[1];
      Prev_Chime_Command.byte[2] = Chime_Command.byte[2];
      Prev_Chime_Command.byte[3] = Chime_Command.byte[3];
      Prev_Chime_Command.byte[4] = Chime_Command.byte[4];
   
   }



   if(t1_enable == TRUE)
   {   
      if(t1_cnt < K_T1)
      {
         t1_cnt++;
         //turn ON
         Set_Buzz(1);
      }
      else//t1 expired
      {
         t1_cnt = 0;
         t2_cnt = 0;
         if((TRUE == repeat_enable) && //if continuous beep required
            (0xFF == dc) &&
            (0xFF == K_RPT))
         {
            t1_enable = TRUE;
            t2_enable = FALSE;
         }
         else
         {
            t1_enable = FALSE;
            t2_enable = TRUE;
            //turn OFF 
            Set_Buzz(0);
         }
      }
   }
   else
   {
   
   }
   
   if(t2_enable == TRUE)
   {
      if((t2_cnt < K_T2) )
      {
         t2_cnt++;
      }
      else//t2 expired
      {
         t1_cnt = 0;
         t2_cnt = 0;
         t2_enable = FALSE;
      }
   }

 
   if(TRUE == repeat_enable)
   {
      if((t1_enable == FALSE) && 
         (t2_enable == FALSE))
      {
         
         if(0xFF == K_RPT)
         {
            t1_enable = TRUE; 
         }
         else//non permanent repeat
         {
            rpt_cnt++;
            if(rpt_cnt >= K_RPT)
            {
               repeat_enable = FALSE;
            }
            else
            {
               t1_enable = TRUE;
            }
         }
      }
   }
   else
   {
      if(rpt_cnt > 0)
      {
         rpt_cnt = 0;
      }
   }

}


/****************************************************************************
*. Name:to_ascii
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
void to_ascii(char* dest, unsigned char *text , unsigned char num)
{

    const char * hex = "0123456789ABCDEF";
    int i = 0;
    for(; i < num-1; ++i){
        *dest++ = hex[(*text>>4)&0xF];
        *dest++ = hex[(*text++)&0xF];
        *dest++ = ':';
    }
    *dest++ = hex[(*text>>4)&0xF];
    *dest++ = hex[(*text)&0xF];
    *dest = 0;
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