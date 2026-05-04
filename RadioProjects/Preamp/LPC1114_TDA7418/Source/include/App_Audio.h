/*filename:-App_Audio.h*/
#ifndef AUDIO_CTRL_H
#define AUDIO_CTRL_H

#include <stdint.h>


//#define CHIP_TDA_7468
#define CHIP_TDA_7418

/* ===== CONFIG ===== */
#define MENU_TIMEOUT_SEC   5u

/* ===== TDA7418 I2C ADDRESS ===== */
#define TDA7418_ADDR  (0x88u)   // check datasheet (8-bit address)



#define MAX_VOLUME         95u
#define MIN_VOLUME          0u
#define MAX_BASS           30u
#define MIN_BASS            0u
#define MAX_MIDDLE         30u
#define MIN_MIDDLE          0u
#define MAX_TREBLE         30u
#define MIN_TREBLE          0u
#define MAX_GAIN           15u
#define MIN_GAIN            0u




typedef enum
{
    AUDIO_MENU_VOL = 0u,
    AUDIO_MENU_BASS,
    AUDIO_MENU_MIDDLE,
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
      unsigned char source_selector:3;
      unsigned char input_gain:4;
      unsigned char diffin_mode:1;
   }bits;
   unsigned char reg;
}input_T;

typedef union
{
   struct
   {
      unsigned char attn:4;
      unsigned char filt_center_freq:2;
      unsigned char shape:1;
      unsigned char loudness_soft_step:1;
   }bits;
   unsigned char reg;
}loudness_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char soft_step:1;
   }bits;
   unsigned char reg;
}volume_T;

typedef union
{
   struct
   {
      unsigned char gain:5;
      unsigned char treble_center_freq:2;
      unsigned char must_be_one:1;
   }bits;
   unsigned char reg;
}treble_T;

typedef union
{
   struct
   {
      unsigned char gain:5;
      unsigned char mid_q_factor:2;
      unsigned char mid_soft_step:1;
   }bits;
   unsigned char reg;
}middle_T;

typedef union
{
   struct
   {
      unsigned char gain:5;
      unsigned char bass_q_factor:2;
      unsigned char bass_soft_step:1;
   }bits;
   unsigned char reg;
}bass_T;

typedef union
{
   struct
   {
      unsigned char mid_center_freq:2;
      unsigned char bass_center_freq:2;
      unsigned char bass_dc_mode:1;
      unsigned char smoothing_filter:1;
      unsigned char reserved:2;
   }bits;
   unsigned char reg;
}mid_bass_fc_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char soft_step:1;
   }bits;
   unsigned char reg;
}tda_spk_attn_lf_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char soft_step:1;
   }bits;
   unsigned char reg;
}tda_spk_attn_lr_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char soft_step:1;
   }bits;
   unsigned char reg;
}tda_spk_attn_rr_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char soft_step:1;
   }bits;
   unsigned char reg;
}tda_spk_attn_rf_T;

typedef union
{
   struct
   {
      unsigned char gain:7;
      unsigned char soft_step:1;
   }bits;
   unsigned char reg;
}tda_sub_attn_T;

typedef union
{
   struct
   {
      unsigned char soft_mute:1;
      unsigned char soft_mute_time:2;
      unsigned char soft_step_time:3;
      unsigned char az_function:1;
      unsigned char reserved:1;
   }bits;
   unsigned char reg;
}tda_soft_mute_step_T;

typedef union
{
   struct
   {
      unsigned char testing_mode:1;
      unsigned char mute_pin_config_bit0:1;
      unsigned char test_multiplexer:3;
      unsigned char reserved:1;
      unsigned char schlock:1;
      unsigned char mute_pin_config_bit1:1;
   }bits;
   unsigned char reg;
}tda_testing_T;

typedef struct
{
   input_T tda_in;
   loudness_T tda_loudness;
   volume_T tda_volume;
   treble_T tda_treble;
   middle_T tda_middle;
   bass_T tda_bass;
   mid_bass_fc_T tda_mid_bass_fc;
   tda_spk_attn_lf_T tda_spk_attn_lf;
   tda_spk_attn_lr_T tda_spk_attn_lr;
   tda_spk_attn_rr_T tda_spk_attn_rr;
   tda_spk_attn_rf_T tda_spk_attn_rf;
   tda_sub_attn_T tda_sub_attn;
   tda_soft_mute_step_T tda_soft_mute_step;
   tda_testing_T tda_testing;

}tda_T;

typedef struct
{
   unsigned char input;
   unsigned char bass;
   unsigned char middle;
   unsigned char treble;
   signed char volume;
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

