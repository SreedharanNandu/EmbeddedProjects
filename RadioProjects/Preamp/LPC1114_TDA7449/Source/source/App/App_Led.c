/*filename:-App_Led.c*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "APP_Led.h"
#include "gpio.h"
#include "Cal_Const.h"



FIFOQueue_T BlinkQ;
BlinkRequest_T Led_Req;
unsigned char FifoBusy;
uint16_t Repeat_Blink,LedOnCnt,LedOffCnt; 


// Initialize the FIFO queue
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Fifo(FIFOQueue_T* q) 
{
    q->front = 0u;
    q->rear = 0u;
    q->size = 0u;
}



// Enqueue a blink request
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void EnQ(FIFOQueue_T* q, BlinkRequest_T temp_req) 
{
    if (q->size == FIFO_SIZE) 
    {
       return;
    }
    else
    {
       q->queue[q->rear] = temp_req;
       q->rear = (q->rear + 1u) % FIFO_SIZE;
       q->size++;
    }
}

// Dequeue a blink request
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
BlinkRequest_T DeQ(FIFOQueue_T* q) 
{
    BlinkRequest_T temp_req;
    BlinkRequest_T empty;

    if (q->size == 0u) 
    {
        // Return a default empty request (handle error case)
        empty.num_blinks = 0u; empty.off_time=0u; empty.on_time = 0u;
        return empty;
    }
    temp_req = q->queue[q->front];
    q->front = (q->front + 1u) % FIFO_SIZE;
    q->size--;
    return temp_req;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void LED_Toggle(void) 
{
  static unsigned char toggle=0u;
  toggle = !toggle;
  GPIOSetValue(0u,3u,toggle);
}


// Function to process the FIFO queue and blink the LED
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Led_Process(FIFOQueue_T* q) 
{
   if(FifoBusy == 0u)
   {
      if(!(q->size == 0u))
      {
         Led_Req = DeQ(q);
         FifoBusy = 1u;
         Repeat_Blink = 0u;
         LedOnCnt = LedOffCnt = 0u;
      }
   }
   else//fifo busy
   {
      // Process the blink request
      if(Repeat_Blink < Led_Req.num_blinks) 
      {
         if(LedOnCnt < Led_Req.on_time)
         {
            LED_On();
            LedOnCnt++;
         }
         if((LedOnCnt >= Led_Req.on_time) && 
            (LedOffCnt < Led_Req.off_time))
         {
            LED_Off();
            LedOffCnt++;
         }
         if((LedOffCnt >= Led_Req.off_time) & (LedOnCnt >= Led_Req.on_time))
         {
            Repeat_Blink++;
            LedOnCnt=0u;
            LedOffCnt=0u;
         }
      }
      else if(Repeat_Blink >= Led_Req.num_blinks) //max blink sequence reached
      {
         FifoBusy = 0u;//next in Q
      }
   }
}
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Led_Task(void)
{
   Led_Process(&BlinkQ);
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Led_Init(void)
{
   Init_Fifo(&BlinkQ);
   // Add requests to the FIFO queue
   Led(blink_PwnOnOff);
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void PowerUpInit_LED(void)
{
   unsigned char i;
   for(i=0u;i<5u;i++)
   {
       LED_On();
       Delay_Ms(100u);
       LED_Off();
       Delay_Ms(100u);
   }
}



