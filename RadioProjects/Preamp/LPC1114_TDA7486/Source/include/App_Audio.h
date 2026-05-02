/*filename:-App_Audio.h*/
#ifndef AUDIO_CTRL_H
#define AUDIO_CTRL_H

#include <stdint.h>


#define MAX_VOLUME         56u
#define MIN_VOLUME          0u
#define MAX_BASS           14u
#define MIN_BASS            0u
#define MAX_TREBLE         14u
#define MIN_TREBLE          0u
#define MAX_GAIN            7u
#define MIN_GAIN            0u

typedef enum
{
    AUDIO_MENU_VOL = 0u,
    AUDIO_MENU_BASS,
    AUDIO_MENU_TREBLE,
    AUDIO_MENU_GAIN,
    AUDIO_MENU_ALC,
    AUDIO_MENU_MAX
} AUDIO_MENU_t;

typedef enum
{
    NO_AUDIO_EFFECT = 0u,
    AUDIO_EFFECT1,
    AUDIO_EFFECT2,
    AUDIO_EFFECT3,
    AUDIO_EFFECT4,
    AUDIO_EFFECT5
} audio_effects_t;

typedef union
{
   struct
   {
      unsigned char reg_select:2;
      unsigned char reg_mute:1;
      unsigned char reg_mic:3;
      unsigned char reg_on_off:1;
      unsigned char reg_reserved:1;
   }bits;
   unsigned char reg;
}input_T;

typedef union
{
   struct
   {
      unsigned char reg_mode:1;
      unsigned char reg_gain:2;
      unsigned char reg_mixing:3;
      unsigned char reg_buffer_gain:1;
      unsigned char reg_reserved:1;
   }bits;
   unsigned char reg;
}surround_T;

typedef union
{
   struct
   {
      unsigned char reg_treble:4;
      unsigned char reg_bass:4;
   }bits;
   unsigned char reg;
}bass_treble_T;

typedef union
{
   struct
   {
      unsigned char reg_alc_mode:1;
      unsigned char reg_detector:1;
      unsigned char reg_rel_current_circuit:1;
      unsigned char reg_attack_time_resistor:2;
      unsigned char reg_threshold:2;
      unsigned char reg_attack_mode:1;
   }bits;
   unsigned char reg;
}bass_alc_T;

typedef struct
{
   input_T tda_in;
   uint8_t tda_gain;
   surround_T tda_surround;
   uint8_t tda_volume;
   bass_treble_T tda_bass_treble;  
   uint8_t tda_output;
   bass_alc_T tda_bass_alc; 

}Tda_T;

typedef struct
{
   unsigned char input;
   unsigned char bass;
   unsigned char treble;
   unsigned char volume;
   unsigned char mute;
   unsigned char alc;
   unsigned char gain;

}Audio_Control_T;

void AudioCtrl_Init(void);
void AudioCtrl_Task(void);

/* Button events */
void AudioCtrl_ButtonUp(void);
void AudioCtrl_ButtonDown(void);
void AudioCtrl_ButtonMenu(void);
void AudioCtrl_ButtonEffects(void);  

#endif

