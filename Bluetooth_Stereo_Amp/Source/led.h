#ifndef __LED_H__
#define __LED_H__

#include <stdio.h>

// Define FIFO queue size (Can be adjusted based on your requirements)
#define FIFO_SIZE 10


#define Led(x)                  EnQ(&BlinkQ,x)


// Structure to hold the blink request
typedef struct 
{
    unsigned short int num_blinks;   // Number of blinks
    unsigned short int on_time;      // On time in milliseconds
    unsigned short int off_time;     // Off time in milliseconds
} BlinkRequest_T;

// FIFO queue structure
typedef struct 
{
    BlinkRequest_T queue[FIFO_SIZE];
    unsigned char front;
    unsigned char rear;
    unsigned char size;
} FIFOQueue_T;

extern const BlinkRequest_T blink_PwnOnOff;
extern const BlinkRequest_T blink_PwnOn;
extern const BlinkRequest_T blink_PwnOff;
extern const BlinkRequest_T blink_a2dpConnect;
extern const BlinkRequest_T blink_a2dpDisconnect;


extern FIFOQueue_T BlinkQ;



void Led_Task(void);

void Init_Fifo(FIFOQueue_T* q) ;
void EnQ(FIFOQueue_T* q, BlinkRequest_T req); 
BlinkRequest_T DeQ(FIFOQueue_T* q); 
void Led_Process(FIFOQueue_T* q);
void Led_Init(void); 

#endif  /* __LED_H__ */
