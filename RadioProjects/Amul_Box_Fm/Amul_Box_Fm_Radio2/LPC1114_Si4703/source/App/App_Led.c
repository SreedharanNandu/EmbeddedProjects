#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "APP_Led.h"
#include "gpio.h"
#include "timer16.h"
#include "App_Si.h"


#define SAMPLE_PER             TMR16_THR_10MS


FIFOQueue_T BlinkQ;

// Create some test blink requests (num_blinks, on_time, off_time)
BlinkRequest_T blink_ModeChange = {1u,   100u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
BlinkRequest_T blink_Store =      {1u,  2000u/SAMPLE_PER,  100u/SAMPLE_PER}; // blinks, on and off
BlinkRequest_T blink_PwnOnOff =   {5u,   250u/SAMPLE_PER,  250u/SAMPLE_PER}; // blinks, on and off
BlinkRequest_T blink_RotChange =  {1u,   20u/SAMPLE_PER,   20u/SAMPLE_PER}; // blinks, on and off

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
        empty.num_blinks = 0; empty.off_time=0; empty.on_time = 0;
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
void LED_On(void) 
{
  // Set the corresponding GPIO pin to HIGH (on)
  GPIOSetValue(0,9,0);
  GPIOSetValue(1,8,0);
}

// Function to turn the LED off (GPIO operation)
/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void LED_Off(void) 
{
  // Set the corresponding GPIO pin to LOW (off)
  GPIOSetValue(0,9,1);
  GPIOSetValue(1,8,1);
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

   if(fifoBusy == 0)
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
            LED_On();
            ledOnCnt++;
         }
         if((ledOnCnt >= req.on_time) && 
            (ledOffCnt < req.off_time))
         {
            LED_Off();
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

