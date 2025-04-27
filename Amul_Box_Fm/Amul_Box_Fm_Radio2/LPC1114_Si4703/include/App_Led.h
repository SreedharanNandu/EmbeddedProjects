#ifndef __APP_LED_H__
#define __APP_LED_H__

#include <stdio.h>

// Define FIFO queue size (Can be adjusted based on your requirements)
#define FIFO_SIZE 10


#define Led(x)                  EnQ(&BlinkQ,x)


// Structure to hold the blink request
typedef struct 
{
    uint8_t num_blinks;    // Number of blinks
    uint16_t on_time;      // On time in milliseconds
    uint16_t off_time;     // Off time in milliseconds
} BlinkRequest_T;

// FIFO queue structure
typedef struct 
{
    BlinkRequest_T queue[FIFO_SIZE];
    uint8_t front;
    uint8_t rear;
    uint8_t size;
} FIFOQueue_T;

extern BlinkRequest_T blink_ModeChange;
extern BlinkRequest_T blink_PwnOnOff;
extern BlinkRequest_T blink_Store;
extern BlinkRequest_T blink_RotChange;

extern FIFOQueue_T BlinkQ;



void Led_Task(void);

void Init_Fifo(FIFOQueue_T* q) ;
void EnQ(FIFOQueue_T* q, BlinkRequest_T req); 
BlinkRequest_T DeQ(FIFOQueue_T* q); 
void Led_Process(FIFOQueue_T* q);
void Led_Init(void); 

#endif  /* __APP_LED_H__ */
