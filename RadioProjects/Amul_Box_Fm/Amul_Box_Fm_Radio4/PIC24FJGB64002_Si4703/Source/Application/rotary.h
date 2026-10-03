#ifndef __APP_ROTARY_H__
#define __APP_ROTARY_H__

#include <stdio.h>
#include "hardware.h"

#define RADIO_ROT_ENC_INACTIVE_RESP_TIME_MS    (100u)
#define RADIO_ENC_IDLE_ENCODER_STATE           (0)        /* Idle encoder state*/
#define RADIO_ENC_ZERO_ENCODER_COUNT           (0x00)     /* Zero*/ 
#define RADIO_ENC_MAXIMUM_ENCODER_COUNT        (0x0E)    
#define RADIO_ENC_MINIMUM_ENCODER_COUNT        (0x01)     /* Minimum encoder value is the same for Radio*/ 
#define RADIO_ENC_ZERO_SAMPLES                 (0u)
#define RADIO_ENC_TIMER_EXPIRED                (0u)
#define RADIO_ENC_DEBOUNCE_PERIOD_NORMAL       (1u)
#define RADIO_ENC_DEBOUNCE_PERIOD_SHORT        (1u)
#define RADIO_ENC_EDGE_DELAY                   (20u)
#define RADIO_ENC_EDGE_FASTER                  (20u)  /* 2 seconds / period (100ms)  = 20 */
#define RADIO_ENC_EDGE_DELAY_EXPIRED           (0u)
#define RADIO_ENC_USE_ENCODER_B_TO_COUNT       (0)/* Use only encoder Gray code phase A to count the detents*/

#define LEFT_TURN  1u
#define RIGHT_TURN 2u

#define EncTuneA_Pin()   _RB5             
#define EncTuneB_Pin()   _RB4             

#define EncVolA_Pin()   _RA0             
#define EncVolB_Pin()   _RA1             

typedef enum
{
   ENC_TUNE=0u,
   ENC_VOL,
   MAX_ENCODERS

}Rot_T;

typedef enum
{
   NO_RADIO_TURN=0u,
   LEFT_RADIO_TURN,
   RIGHT_RADIO_TURN

}Radio_Dir_T;

extern void Init_Encoder(void);
extern void Fast_Periodic_Encoder(void);
extern void Slow_Periodic_Encoder(void);


#endif  /* __APP_ROTARY_H__ */
