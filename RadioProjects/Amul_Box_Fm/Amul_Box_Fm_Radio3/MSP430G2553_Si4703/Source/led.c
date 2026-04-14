#include <stdio.h>
#include <stdint.h>
#include "type.h"
#include <msp430.h>

/*filname :- led.c*/

FIFOQueue_T BlinkQ;


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
void EnQ(FIFOQueue_T* q, BlinkRequest_T req) 
{
    if (q->size == FIFO_SIZE) 
    {
       return;
    }
    else
    {
       q->queue[q->rear] = req;
       q->rear = (q->rear + 1) % FIFO_SIZE;
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
    BlinkRequest_T req;
    BlinkRequest_T empty;

    if (q->size == 0u) 
    {
        // Return a default empty request (handle error case)
        empty.num_blinks = 0u; 
        empty.off_time = 0u; 
        empty.on_time = 0u;
        empty.id = 0u;
        return empty;
    }
    req = q->queue[q->front];
    q->front = (q->front + 1u) % FIFO_SIZE;
    q->size--;
    return req;
}


// Function to turn the LED on (GPIO operation)
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void LED_On(unsigned char id) 
{
   // Set the corresponding GPIO pin to HIGH (on)
   if(id != ROTCHANGE)
   {
      Set_GPIO2(FALSE);
   }
}

// Function to turn the LED off (GPIO operation)
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void LED_Off(unsigned char id) 
{
   // Set the corresponding GPIO pin to LOW (off)
   if(id != ROTCHANGE)
   {
      Set_GPIO2(TRUE);
   }
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
   static BlinkRequest_T req;
   static unsigned char fifoBusy;
   static unsigned short int repeat,ledOnCnt,ledOffCnt; 

   if(fifoBusy == 0u)
   {
      if(!(q->size == 0u))
      {
         req = DeQ(q);
         fifoBusy = 1u;
         repeat = 0u;
         ledOnCnt = ledOffCnt = 0;
      }
   }
   else//fifo busy
   {
      // Process the blink request
      if(repeat < req.num_blinks) 
      {
         if(ledOnCnt < req.on_time)
         {
            LED_On(req.id);
            ledOnCnt++;
         }
         if((ledOnCnt >= req.on_time) && 
            (ledOffCnt < req.off_time))
         {
            LED_Off(req.id);
            ledOffCnt++;
         }
         if((ledOffCnt >= req.off_time) & (ledOnCnt >= req.on_time))
         {
            repeat++;
            ledOnCnt=0u;
            ledOffCnt=0u;
         }
      }
      else if(repeat >= req.num_blinks) //max blink sequence reached
      {
         fifoBusy = 0u;//next in Q
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
   Led(K_blink_PwnOnOff);
}

