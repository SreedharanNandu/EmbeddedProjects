#ifndef __APP_ROTARY_H__
#define __APP_ROTARY_H__

#include <stdio.h>
#include "System.h" 
#include "gpio.h"

#define LEFT_TURN  1u
#define RIGHT_TURN 2u

#define EncA_Pin()   GPIOGetValue(PORT1,4)             
#define EncB_Pin()   GPIOGetValue(PORT1,5)             

#define EncA1_Pin()   GPIOGetValue(PORT1,0)             
#define EncB1_Pin()   GPIOGetValue(PORT1,1)             


#define RADIO_ROT_ENC_INACTIVE_RESP_TIME_MS    (100u)
#define RADIO_ENC_IDLE_ENCODER_STATE  0        ///* Idle encoder state*/
#define RADIO_ENC_MAXIMUM_ENCODER_STATE  (2 )       ///* Maximum 2 edge per detent encoder state*/
#define RADIO_ENC_MINIMUM_ENCODER_STATE  (-2 )      ///* Minimum 2 edge per detent encoder state*/
#define RADIO_ENC_ZERO_ENCODER_COUNT     (0x00)    ///* Zero*/ 
#define RADIO_ENC_MAXIMUM_ENCODER_COUNT  (0x0E)    
#define RADIO_ENC_MINIMUM_ENCODER_COUNT  (0x01)    ///* Minimum encoder value is the same for Radio*/ 
#define RADIO_ENC_ZERO_SAMPLES   (0u)
#define RADIO_ENC_TIMER_EXPIRED  (0u)
#define RADIO_ENC_DEBOUNCE_PERIOD_NORMAL  (1u)
#define RADIO_ENC_DEBOUNCE_PERIOD_SHORT  (1u)
#define RADIO_ENC_EDGE_DELAY         (20u)
#define RADIO_ENC_EDGE_FASTER        (20u)  /* 2 seconds / period (100ms)  = 20 */
#define RADIO_ENC_EDGE_DELAY_EXPIRED (0u)
#define RADIO_ENC_USE_ENCODER_B_TO_COUNT (0)/* Use only encoder Gray code phase A to count the detents*/

typedef enum
{
   NO_RADIO_TURN=0,
   LEFT_RADIO_TURN,
   RIGHT_RADIO_TURN

}Radio_Dir_T;

extern void Init_Encoder(void);
extern void Init_Encoder1(void);
extern void Fast_Periodic_Encoder(void);
extern void Fast_Periodic_Encoder1(void);
extern void Slow_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder1(void);


#endif  /* __APP_ROTARY_H__ */
