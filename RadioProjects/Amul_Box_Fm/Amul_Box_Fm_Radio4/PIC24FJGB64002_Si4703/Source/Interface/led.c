/*****************************************************************************
** File Name    : led.c
** Initial Date :   
** Created by   : Nandu
** Description  : User Interface Buzer and LED Control 
**              : 
** Notes        : 
*****************************************************************************/
#include "led.h"
#include "io.h"
#include "radio.h"



volatile LED_Struct_T ledStruct[NUM_OF_LEDS],
                      ledStructQue[NUM_OF_LEDS];

static unsigned char Monitor_LED_Timer(unsigned long *userTimer,
                                       unsigned long interval);


/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void Start_LED(LED_Type_T ledID,
               unsigned char mode,
               unsigned char count,
               unsigned long onTime,
               unsigned long offTime,
               unsigned char startState
               )
{
   
   if(ledStruct[ledID].active > 0u) 
   { 
      ledStructQue[ledID].active = (unsigned char)1u;
      ledStructQue[ledID].mode = mode;
      ledStructQue[ledID].count = count-(unsigned char)1u;
      ledStructQue[ledID].onTime = (unsigned long)onTime;
      ledStructQue[ledID].offTime = (unsigned long)offTime;
      ledStructQue[ledID].state = startState;
      ledStructQue[ledID].startedFlag = (unsigned char)0u;
   } 
   else
   {
      ledStructQue[ledID].active = (unsigned char)0u; // in case there is something enabled to run
      ledStruct[ledID].active = (unsigned char)1u;
      ledStruct[ledID].mode = mode;
      ledStruct[ledID].onTime = onTime;
      ledStruct[ledID].offTime = offTime;
      ledStruct[ledID].count = count-(unsigned char)1u;
      ledStruct[ledID].state = startState;
      ledStruct[ledID].startedFlag = (unsigned char)0u;
   }
} 

/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
static void LED_Action(LED_Type_T index)
{
   switch (ledStruct[index].mode)
   {
    case TURN_ON:
         ledStruct[index].active = (unsigned char)0u;
         ledStruct[index].startedFlag = (unsigned char)0u;
         ledStruct[index].state = (unsigned char)TURN_ON;
         LED_On(index);
         break;
   
    case TURN_OFF:
         ledStruct[index].active = (unsigned char)0u;
         ledStruct[index].startedFlag = (unsigned char)0u;
         ledStruct[index].state = (unsigned char)TURN_OFF;
         LED_Off(index);
         break;
   
   default: //(blink)
         if(ledStruct[index].startedFlag == (unsigned char)0u)
         {
            ledStruct[index].startedFlag = (unsigned char)1u;
            ledStruct[index].startTick = 0u;
            if(ledStruct[index].state == (unsigned char)TURN_ON)
            {
               LED_On(index);
            }
            else  
            {
               LED_Off(index);
            }
         }
         else
         {
            if(ledStruct[index].state == (unsigned char)TURN_ON)
            {
               if(Monitor_LED_Timer( (unsigned long*)&(ledStruct[index].startTick), (unsigned long)ledStruct[index].onTime))
               {  
                  //timeout 
                  ledStruct[index].state = (unsigned char)TURN_OFF;
                  ledStruct[index].startTick = 0;
                  LED_On(index);
               }
            }
            else //TURN OFF
            {
               if(Monitor_LED_Timer( (unsigned long*)&(ledStruct[index].startTick), (unsigned long)ledStruct[index].offTime))
               {    
                  //blink off  
                  if(ledStruct[index].count !=(unsigned char)0u)
                  {  
                      //continue blink.
                     if(ledStruct[index].count != (unsigned char)0xffu)  //0xff is continue mode.
                     {
                        ledStruct[index].count-=(unsigned char)1u;  
                     }
                     ledStruct[index].state = (unsigned char)TURN_ON;
                     ledStruct[index].startTick = 0u;
                     LED_Off(index);
                  }
                  else
                  {   //stop blink
                     ledStruct[index].active = (unsigned char)0u;
                  }   
               }
            }
         }     
         break;
   }
   
   if(ledStruct[index].active==(unsigned char)0u)
   { 
      if(ledStructQue[index].active)
      { 
         ledStruct[index].active = ledStructQue[index].active;
         ledStructQue[index].active = (unsigned char)0u;
         ledStruct[index].mode = ledStructQue[index].mode;
         ledStruct[index].count = ledStructQue[index].count;
         ledStruct[index].onTime = ledStructQue[index].onTime;
         ledStruct[index].offTime = ledStructQue[index].offTime;
         ledStruct[index].state =ledStructQue[index].state;
         ledStruct[index].startedFlag = (unsigned char)0u;
      }
   }
}    
/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void Init_LED(void)
{
   ledStructQue[LED_A].startTick = (unsigned char)0u;
   ledStructQue[LED_A].active = (unsigned char)0u;
   ledStructQue[LED_A].mode = 0u;
   ledStructQue[LED_A].count = 0u;
   ledStructQue[LED_A].onTime = 0u;
   ledStructQue[LED_A].offTime = 0u;
   ledStructQue[LED_A].state = 0u;
   ledStructQue[LED_A].startedFlag = (unsigned char)0u;
   ledStructQue[LED_A].active = (unsigned char)0u; // in case there is something enabled to run

   ledStruct[LED_A].startTick = (unsigned char)0u;
   ledStruct[LED_A].active = (unsigned char)0u;
   ledStruct[LED_A].mode = 0u;
   ledStruct[LED_A].onTime = 0u;
   ledStruct[LED_A].offTime = 0u;
   ledStruct[LED_A].count = 0u;
   ledStruct[LED_A].state = 0u;
   ledStruct[LED_A].startedFlag = (unsigned char)0u;

   ledStructQue[LED_B].startTick = (unsigned char)0u;
   ledStructQue[LED_B].active = (unsigned char)0u;
   ledStructQue[LED_B].mode = 0u;
   ledStructQue[LED_B].count = 0u;
   ledStructQue[LED_B].onTime = 0u;
   ledStructQue[LED_B].offTime = 0u;
   ledStructQue[LED_B].state = 0u;
   ledStructQue[LED_B].startedFlag = (unsigned char)0u;
   ledStructQue[LED_B].active = (unsigned char)0u; // in case there is something enabled to run

   ledStruct[LED_B].startTick = (unsigned char)0u;
   ledStruct[LED_B].active = (unsigned char)0u;
   ledStruct[LED_B].mode = 0u;
   ledStruct[LED_B].onTime = 0u;
   ledStruct[LED_B].offTime = 0u;
   ledStruct[LED_B].count = 0u;
   ledStruct[LED_B].state = 0u;
   ledStruct[LED_B].startedFlag = (unsigned char)0u;


}
/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void LED_Control(void)
{
   volatile unsigned char i;
   for(i=0u; i<(unsigned char)NUM_OF_LEDS;i+=(unsigned char)1u)
   {
      if(ledStruct[i].active)
      {
         LED_Action((LED_Type_T)i);
      }
   }           
}
/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
static unsigned char Monitor_LED_Timer(unsigned long *userTimer,
                                       unsigned long interval)
{
  unsigned char status=0u;

   
   if(*userTimer >= (interval/LED_TASK_PERIOD))
   {
      status = 1u;
   }
   else
   {
     *userTimer += 1u;
   }
   return status;
}



/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void LED_On(LED_Type_T type) 
{
   if(type == LED_A)
   {
      Set_GPIO1(1u);
   }
   else if(type == LED_B)
   {
      Set_GPIO2(1u);
   }
}

/*****************************************************************************
* Function   : 
* Created By : 
* Date       : 
* Overview   : 
* Argument   : 
* Output     : 
* Note       :    
*****************************************************************************/
void LED_Off(LED_Type_T type)
{
   if(type == LED_A)
   {
      Set_GPIO1(0u);
   }
   else if(type == LED_B)
   {
      Set_GPIO2(0u);
   }
}


//EOF
