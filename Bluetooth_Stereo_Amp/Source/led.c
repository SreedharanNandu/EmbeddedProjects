#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "led.h"
#include "p30f3010.h"
#include "ports.h"




FIFOQueue_T BlinkQ;

// Create some test blink requests (num_blinks, on_time, off_time)
const BlinkRequest_T blink_PwnOnOff = {3,   20,  20}; // blinks, on and off
const BlinkRequest_T blink_PwnOn =    {0xFFFF,   1,  0}; // on
const BlinkRequest_T blink_PwnOff =   {0xFFFF,   0,  1}; // off
const BlinkRequest_T blink_a2dpConnect = {3,   5,  10}; // off
const BlinkRequest_T blink_a2dpDisconnect = {3,   5,  10}; // off

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
   RB3 = 0;
   RE0 = 0;
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
   RB3 = 1;
   RE0 = 1;
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
      if(req.num_blinks == 0xFFFF) 
      {
         if((req.on_time == 1) && (req.off_time == 0))
         {
            LED_On();
         }
         if((req.off_time == 1)&& (req.on_time == 0))
         {
            LED_Off();
         }
         fifoBusy = 0;//next in Q
      }
      // Process the blink request
      else if(req.num_blinks != 0xFFFF) 
      {
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
         else 
         {
            if(repeat >= req.num_blinks) //max blink sequence reached
            {
               fifoBusy = 0;//next in Q
               repeat = 0;
            }
         }
      }
      else
      {
            //do nothing
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

