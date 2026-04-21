#ifndef __APP_ROTARY_H__
#define __APP_ROTARY_H__

#include <stdio.h>

#define RADIO_ROT_ENC_INACTIVE_RESP_TIME_MS    (100u)
#define RADIO_ENC_IDLE_ENCODER_STATE           (0)        ///* Idle encoder state*/
#define RADIO_ENC_MAXIMUM_ENCODER_STATE        (4)       ///* Maximum 2 edge per detent encoder state*/
#define RADIO_ENC_MINIMUM_ENCODER_STATE        (-4)      ///* Minimum 2 edge per detent encoder state*/
#define RADIO_ENC_ZERO_ENCODER_COUNT           (0x00)    ///* Zero*/ 
#define RADIO_ENC_MAXIMUM_ENCODER_COUNT        (0x0E)    
#define RADIO_ENC_MINIMUM_ENCODER_COUNT        (0x01)    ///* Minimum encoder value is the same for Radio*/ 
#define RADIO_ENC_ZERO_SAMPLES                 (0u)
#define RADIO_ENC_TIMER_EXPIRED                (0u)
#define RADIO_ENC_DEBOUNCE_PERIOD_NORMAL       (1u)
#define RADIO_ENC_DEBOUNCE_PERIOD_SHORT        (1u)
#define RADIO_ENC_EDGE_DELAY                   (20u)
#define RADIO_ENC_EDGE_FASTER                  (20u)  /* 2 seconds / period (100ms)  = 20 */
#define RADIO_ENC_EDGE_DELAY_EXPIRED           (0u)
#define RADIO_ENC_USE_ENCODER_B_TO_COUNT       (0)/* Use only encoder Gray code phase A to count the detents*/

typedef enum
{
   ENC1=0u,
   ENC2,
   MAX_ENCODERS
}Encoder_T;

typedef enum
{
   NO_RADIO_TURN=0,
   LEFT_RADIO_TURN,
   RIGHT_RADIO_TURN

}Radio_Dir_T;


extern void Init_Encoder(void);
extern void Fast_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder(void);


#endif  /* __APP_ROTARY_H__ */
