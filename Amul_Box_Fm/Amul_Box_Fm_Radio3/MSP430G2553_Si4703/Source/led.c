#include <stdio.h>
#include <stdint.h>
#include "type.h"
#include <msp430.h>

/*filname :- led.c*/

FIFOQueue_T BlinkQ;

// Create some test blink requests (num_id , num_blinks, on_time, off_time)
const BlinkRequest_T blink_ModeChange = {BLINK_MODE_CHANGE  , 1u,   100u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_Store =      {STORE              , 1u,  2000u/SAMPLE_PER,  100u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_PwnOnOff =   {PWNONOFF           , 5u,   250u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
const BlinkRequest_T blink_RotChange =  {ROTCHANGE          , 1u,   20u/SAMPLE_PER,   20u/SAMPLE_PER}; // blinks, on and off

// Initialize the FIFO queue
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Init_Fifo(FIFOQueue_T* q) 
{
    q->front = 0;
    q->rear = 0;
    q->size = 0;
}

// Check if the FIFO queue is empty
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
int Fifo_Empty(FIFOQueue_T* q) 
{
    return q->size == 0;
}

// Check if the FIFO queue is full
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
int Fifo_Full(FIFOQueue_T* q) 
{
    return q->size == FIFO_SIZE;
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
    if (Fifo_Full(q)) 
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

    if (Fifo_Empty(q)) 
    {
        // Return a default empty request (handle error case)
        empty.num_blinks = 0; 
        empty.off_time=0; 
        empty.on_time = 0;
        empty.id = 0;
        return empty;
    }
    req = q->queue[q->front];
    q->front = (q->front + 1) % FIFO_SIZE;
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
   static uint16_t repeat,ledOnCnt,ledOffCnt; 

   if(fifoBusy == 0u)
   {
      if(!Fifo_Empty(q)) 
      {
         req = DeQ(q);
         fifoBusy = 1;
         repeat = 0;
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
            ledOnCnt=0;
            ledOffCnt=0;
         }
      }
      else if(repeat >= req.num_blinks) //max blink sequence reached
      {
         fifoBusy = 0;//next in Q
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

