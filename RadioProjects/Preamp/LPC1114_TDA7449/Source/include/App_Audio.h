/*filename:-App_Audio.h*/
#ifndef AUDIO_CTRL_H
#define AUDIO_CTRL_H

#include <stdint.h>


//#define CHIP_TDA_7468
//#define CHIP_TDA_7418
#define CHIP_TDA_7449

/* ===== CONFIG ===== */
#define MENU_TIMEOUT_SEC   5u

/* ===== TDA7418 I2C ADDRESS ===== */
#define TDA7449_ADDR  (0x88u)   // check datasheet (8-bit address)



#define MAX_VOLUME         47u
#define MIN_VOLUME          0u
#define MAX_BASS           14u
#define MIN_BASS            0u
#define MAX_TREBLE         14u
#define MIN_TREBLE          0u
#define MAX_GAIN           15u
#define MIN_GAIN            0u




typedef enum
{
    AUDIO_MENU_VOL = 0u,
    AUDIO_MENU_BASS,
    AUDIO_MENU_TREBLE,
    AUDIO_MENU_GAIN,
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
      unsigned char input_sel:2;
      unsigned char reserved:6;
   }bits;
   unsigned char reg;
}input_T;

typedef union
{
   struct
   {
      unsigned char input_gain:4;
      unsigned char reserved:4;
   }bits;
   unsigned char reg;
}gain_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char reserved:1;
   }bits;
   unsigned char reg;
}volume_T;

typedef union
{
   struct
   {
      unsigned char gain:4;
      unsigned char reserved:4;
   }bits;
   unsigned char reg;
}treble_T;

typedef union
{
   struct
   {
      unsigned char gain:4;
      unsigned char reserved:4;
   }bits;
   unsigned char reg;
}bass_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char reserved:1;
   }bits;
   unsigned char reg;
}tda_spk_attn_T;



typedef struct
{
   input_T tda_in;
   gain_T tda_gain;
   volume_T tda_volume;
   bass_T tda_bass;
   treble_T tda_treble;
   tda_spk_attn_T tda_spk_attn_l;
   tda_spk_attn_T tda_spk_attn_r;

}tda_T;

typedef struct
{
   unsigned char input;
   unsigned char bass;
   unsigned char treble;
   unsigned char volume;
   unsigned char mute;
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

