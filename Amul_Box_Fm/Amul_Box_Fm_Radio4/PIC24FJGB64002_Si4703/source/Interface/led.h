
#ifndef LED_H
#define LED_H

#include "hwio.h"
#include "types.h"
#include "pwm.h"





#define TURN_ON       0x80
#define TURN_OFF      0x00
#define BLINK_ON      0xC0
#define BLINK_OFF     0x40


#define LED_MODE_MASK    0xC0

#define LED_TASK_PERIOD  10u //10ms

// blink times
#define BLINK_RESOLUTION   10  // 10ms blink resolution
#define BLINK_100MS     100  // 100ms blink ON/OFF time
#define BLINK_125MS     125  // 125ms blink ON/OFF time
#define BLINK_200MS     200  // 200ms blink ON/OFF time
#define BLINK_250MS     250  // 250ms blink ON/OFF time
#define BLINK_300MS     300  // 300ms blink ON/OFF time
#define BLINK_400MS     400  // 400ms blink ON/OFF time
#define BLINK_500MS     500  // 500ms blink ON/OFF time
#define BLINK_1SEC     1000 // 1 second blink ON/OFF time
#define BLINK_2SECS     2000 // 2 seconds blink ON/OFF time
#define BLINK_3SECS     3000 // 3 seconds blink ON/OFF time

#define SLOW_RATE BLINK_400MS   // slow VIP LED/beeper blinnk/beep rate
#define MEDIUM_RATE BLINK_200MS   // medium VIP LED/beeper blink/beep rate
#define FAST_RATE BLINK_100MS   // fast VIP LED/beeper blink/beep rate

#define SLOW_FLASH BLINK_500MS   // NiGEL slow flash rate
#define MED_FLASH BLINK_250MS   // NiGEL medium flash rate
#define FAST_FLASH BLINK_125MS   // NiGEL fast flash rate

typedef enum
{
   LED_A=0,
   LED_B,
   NUM_OF_LEDS
}LED_Type_T;


typedef struct
{
   unsigned char active; //active, no-active
   unsigned char mode;  //on,off, blink
   unsigned long onTime;
   unsigned long offTime;
   unsigned char count;
   unsigned char state;
   unsigned long startTick;
   unsigned char startedFlag;
   unsigned char buzzerFlag;

} LED_Struct_T; 

extern volatile LED_Struct_T ledStruct[NUM_OF_LEDS],
                             ledStructQue[NUM_OF_LEDS];

void Init_LED(void);
void LED_Control(void);
void Start_LED(LED_Type_T ledID,
               unsigned char mode,
               unsigned char count,
               unsigned long onTime,
               unsigned long offTime,
               unsigned char startState,
               unsigned char buzz_on);

void LED_On(LED_Type_T type);
void LED_Off(LED_Type_T type);


#endif 
//EOF
