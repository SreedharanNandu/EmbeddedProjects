#ifndef __APP_LED_H__
#define __APP_LED_H__

#include <stdio.h>
#include "timer16.h"

// Define FIFO queue size (Can be adjusted based on your requirements)
#define FIFO_SIZE 10

#define SAMPLE_PER             TMR16_THR_10MS

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


extern FIFOQueue_T BlinkQ;



extern void Led_Task(void);
extern void LED_On(void);
extern void LED_Off(void);
extern void PowerUpInit_LED(void);
extern void Init_Fifo(FIFOQueue_T* q) ;
extern void EnQ(FIFOQueue_T* q, BlinkRequest_T req); 
extern BlinkRequest_T DeQ(FIFOQueue_T* q); 
extern void Led_Process(FIFOQueue_T* q);
extern void Led_Init(void); 
extern void LED_Toggle(void) ;

#endif  /* __APP_LED_H__ */
