/*filename:-App_Audio.c*/
#include "App_Audio.h"
#include "App_I2c_Intf.h"
#include "Cal_Const.h"

/* ===== CONFIG ===== */
#define MENU_TIMEOUT_SEC   5u

/* ===== TDA7468D I2C ADDRESS ===== */
#define TDA7468_ADDR  (0x88u)   // check datasheet (8-bit address)

/* ===== PARAMETERS ===== */
volatile Audio_Control_T audio;
volatile Tda_T tda;

volatile AUDIO_MENU_t currentMenu;

volatile uint32_t menuTimer;
volatile audio_effects_t audio_effect;

/* ===== APPLY SETTINGS ===== */
static void Audio_Apply(void)
{
   uint8_t buf[2];

   tda.tda_in.bits.reg_mute = audio.mute;
   tda.tda_in.bits.reg_select = audio.input;

   tda.tda_gain = audio.gain;
   tda.tda_volume = audio.volume;

   tda.tda_output = audio.mute ? 0u:1u;
   
   tda.tda_bass_treble.bits.reg_bass = audio.bass;
   tda.tda_bass_treble.bits.reg_treble = audio.treble;

   tda.tda_bass_alc.bits.reg_alc_mode = audio.alc;

   tda.tda_bass_treble.bits.reg_bass = K_Tda_Bass_Reg[audio.bass];
   tda.tda_bass_treble.bits.reg_treble = K_Tda_Treble_Reg[audio.treble];

   buf[0] = 0u; 
   buf[1] = tda.tda_in.reg;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 1u; 
   buf[1] = tda.tda_gain;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 2u; 
   buf[1] = tda.tda_surround.reg;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 3u; 
   buf[1] = tda.tda_volume;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 4u; 
   buf[1] = tda.tda_volume;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 5u; 
   buf[1] = tda.tda_bass_treble.reg;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 6u; 
   buf[1] = tda.tda_output;
   Write_I2C(TDA7468_ADDR, buf, 2u);
   buf[0] = 7u; 
   buf[1] = tda.tda_bass_alc.reg;
   Write_I2C(TDA7468_ADDR, buf, 2u);

}

/* ===== INIT ===== */
void AudioCtrl_Init(void)
{
   currentMenu = AUDIO_MENU_VOL;
   menuTimer = 0u;
   
   audio.input = 0u;
   audio.gain = 7u;
   audio.bass = 6u;
   audio.treble = 6;
   audio.volume = 7u;
   audio.mute = 0u;
   audio.alc = 0u;
   
   tda.tda_in.bits.reg_mic = 0u;
   tda.tda_in.bits.reg_mute = audio.mute;
   tda.tda_in.bits.reg_select = audio.input;

   tda.tda_gain = audio.gain;
   
   tda.tda_surround.bits.reg_gain = 3u;
   tda.tda_surround.bits.reg_mixing = 3u;
   tda.tda_surround.bits.reg_mode = 0u;
   tda.tda_surround.bits.reg_buffer_gain = 0u;

   tda.tda_volume = audio.volume;
   
   tda.tda_bass_treble.bits.reg_bass = K_Tda_Bass_Reg[audio.bass];
   tda.tda_bass_treble.bits.reg_treble = K_Tda_Treble_Reg[audio.treble];

   tda.tda_output = audio.mute ? 0u:1u;

   tda.tda_bass_alc.bits.reg_alc_mode = 0u;
   tda.tda_bass_alc.bits.reg_attack_mode = 1u;
   tda.tda_bass_alc.bits.reg_attack_time_resistor = 0u;
   tda.tda_bass_alc.bits.reg_detector = 1u;
   tda.tda_bass_alc.bits.reg_rel_current_circuit = 1u;
   tda.tda_bass_alc.bits.reg_threshold = 0u;

   
   audio_effect = NO_AUDIO_EFFECT;
   Audio_Apply();
}

/* ===== BUTTON HANDLERS ===== */
void AudioCtrl_ButtonUp(void)
{
    menuTimer = 0u;
    switch(currentMenu)
    {
        case AUDIO_MENU_VOL:
            if(audio.volume > MIN_VOLUME) 
            {
               audio.volume--;
            }
            break;
        case AUDIO_MENU_BASS:
            if(audio.bass < MAX_BASS) 
            {
               audio.bass++;
            }
            break;
        case AUDIO_MENU_TREBLE:
            if(audio.treble < MAX_TREBLE) 
            {
               audio.treble++;
            }
            break;
        case AUDIO_MENU_GAIN:
            if(audio.gain < MAX_GAIN) 
            {
               audio.gain++;
            }
            break;
        case AUDIO_MENU_ALC:
            audio.alc = 1u;
            break;
        default: 
            break;
    }

    Audio_Apply();
}

void AudioCtrl_ButtonDown(void)
{
    menuTimer = 0u;
    switch(currentMenu)
    {
        case AUDIO_MENU_VOL:
            if(audio.volume < MAX_VOLUME) 
            {
               audio.volume++;
            }
            break;
        case AUDIO_MENU_BASS:
            if(audio.bass > MIN_BASS)
            {
               audio.bass--;
            }
            break;
        case AUDIO_MENU_TREBLE:
            if(audio.treble > MIN_TREBLE)
            {
               audio.treble--;
            }
            break;
        case AUDIO_MENU_GAIN:
            if(audio.gain > MIN_GAIN)
            {
               audio.gain--;
            }
            break;
        case AUDIO_MENU_ALC:
            audio.alc = 0u;
            break;
        default: 
            break;
    }
    Audio_Apply();
}

void AudioCtrl_ButtonMenu(void)
{
    currentMenu++;

    if(currentMenu >= AUDIO_MENU_MAX)
    {
       currentMenu = AUDIO_MENU_VOL;
    }
    menuTimer = 0u;
}


void AudioCtrl_ButtonEffects(void)
{
   tda.tda_surround.bits.reg_buffer_gain = 0u;
   tda.tda_surround.bits.reg_gain = 3u;
   switch(audio_effect)
   {
     case AUDIO_EFFECT1:
          tda.tda_surround.bits.reg_mode = 1u;
          tda.tda_surround.bits.reg_mixing = 0u;
          audio_effect = AUDIO_EFFECT2;
          break;
     case AUDIO_EFFECT2:
          tda.tda_surround.bits.reg_mode = 1u;
          tda.tda_surround.bits.reg_mixing = 1u;
          audio_effect = AUDIO_EFFECT3;
          break;
     case AUDIO_EFFECT3:
          tda.tda_surround.bits.reg_mode = 1u;
          tda.tda_surround.bits.reg_mixing = 2u;
          audio_effect = AUDIO_EFFECT4;
          break;
     case AUDIO_EFFECT4:
          tda.tda_surround.bits.reg_mode = 1u;
          tda.tda_surround.bits.reg_mixing = 4u;
          audio_effect = AUDIO_EFFECT5;
          break;
     case AUDIO_EFFECT5:
          tda.tda_surround.bits.reg_mode = 1u;
          tda.tda_surround.bits.reg_mixing = 5u;
          audio_effect = NO_AUDIO_EFFECT;
          break;
     case NO_AUDIO_EFFECT:
          tda.tda_surround.bits.reg_mode = 0u;
          tda.tda_surround.bits.reg_mixing = 3u;
          audio_effect = AUDIO_EFFECT1;
          break;
     default:
          break;
   }
   Audio_Apply();
}

/* ===== MAIN TASK ===== */
void AudioCtrl_Task(void)
{
    menuTimer++;

    if(menuTimer > MENU_TIMEOUT_SEC)
    {
        currentMenu = AUDIO_MENU_VOL;
    }
}


