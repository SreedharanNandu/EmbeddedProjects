/*filename:-App_Audio.c*/
#include "App_Audio.h"
#include "App_I2c_Intf.h"
#include "Cal_Const.h"


/* ===== PARAMETERS ===== */
volatile Audio_Control_T audio;
volatile tda_T tda;

volatile AUDIO_MENU_t currentMenu;

volatile uint32_t menuTimer;
volatile audio_effects_t audio_effect;

/* ===== APPLY SETTINGS ===== */
/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
static void Audio_Apply(void)
{
   uint8_t buf[2];

 
   tda.tda_in.bits.input_gain = audio.gain;
   tda.tda_volume.bits.gain = K_Tda_Vol_Reg[audio.volume];
   tda.tda_treble.bits.gain = K_Tda_Treble_Reg[audio.treble];
   tda.tda_bass.bits.gain = K_Tda_Bass_Reg[audio.bass];
   tda.tda_middle.bits.gain = K_Tda_Middle_Reg[audio.middle];
   tda.tda_spk_attn_lf.bits.gain = 15u;
   tda.tda_spk_attn_lr.bits.gain = 15u;
   tda.tda_spk_attn_rr.bits.gain = 15u;
   tda.tda_spk_attn_rf.bits.gain = 15u;
   tda.tda_soft_mute_step.bits.soft_mute=1u;
 
 
 
   buf[0] = 0u; 
   buf[1] = tda.tda_in.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 1u; 
   buf[1] = tda.tda_loudness.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 2u; 
   buf[1] = tda.tda_volume.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 3u; 
   buf[1] = tda.tda_treble.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 4u; 
   buf[1] = tda.tda_middle.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 5u; 
   buf[1] = tda.tda_bass.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 6u; 
   buf[1] = tda.tda_mid_bass_fc.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 7u; 
   buf[1] = tda.tda_spk_attn_lf.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 8u; 
   buf[1] = tda.tda_spk_attn_lr.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 9u; 
   buf[1] = tda.tda_spk_attn_rr.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 10u; 
   buf[1] = tda.tda_spk_attn_rf.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 11u; 
   buf[1] = tda.tda_sub_attn.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 12u; 
   buf[1] = tda.tda_soft_mute_step.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);
   buf[0] = 13u; 
   buf[1] = tda.tda_testing.reg;
   Write_I2C(TDA7418_ADDR, buf, 2u);

}



/* ===== INIT ===== */
/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
void AudioCtrl_Init(void)
{
   currentMenu = AUDIO_MENU_VOL;
   menuTimer = 0u;
   
   audio.input = 3u;
   audio.gain = 5u;
   audio.bass = 10u;
   audio.middle = 1u;
   audio.treble = 10;
   audio.volume = 80u;
   audio.mute = 0u;
   
      
   tda.tda_in.bits.source_selector = audio.input;
   tda.tda_in.bits.input_gain = audio.gain;
   tda.tda_in.bits.diffin_mode = 1u;
   tda.tda_loudness.bits.attn = 0u;
   tda.tda_loudness.bits.filt_center_freq = 0u;
   tda.tda_loudness.bits.shape = 0u;
   tda.tda_loudness.bits.loudness_soft_step = 0u;
   tda.tda_volume.bits.gain = K_Tda_Vol_Reg[audio.volume];
   tda.tda_volume.bits.soft_step = 0u;
   tda.tda_treble.bits.gain = K_Tda_Treble_Reg[audio.treble];
   tda.tda_treble.bits.treble_center_freq = 1u;
   tda.tda_middle.bits.gain = K_Tda_Middle_Reg[audio.middle];
   tda.tda_middle.bits.mid_q_factor = 0u;
   tda.tda_middle.bits.mid_soft_step = 0u;
   tda.tda_bass.bits.gain = K_Tda_Bass_Reg[audio.bass];
   tda.tda_bass.bits.bass_q_factor = 0u;
   tda.tda_bass.bits.bass_soft_step = 0u;
   tda.tda_mid_bass_fc.bits.mid_center_freq = 0u;
   tda.tda_mid_bass_fc.bits.bass_center_freq = 0u;
   tda.tda_mid_bass_fc.bits.bass_dc_mode = 1u;
   tda.tda_mid_bass_fc.bits.smoothing_filter = 1u;
   tda.tda_spk_attn_lf.bits.gain = 15u;
   tda.tda_spk_attn_lf.bits.soft_step = 0u;
   tda.tda_spk_attn_lr.bits.gain = 15u;
   tda.tda_spk_attn_lr.bits.soft_step = 0u;
   tda.tda_spk_attn_rr.bits.gain = 15u;
   tda.tda_spk_attn_rr.bits.soft_step = 0u;
   tda.tda_spk_attn_rf.bits.gain = 15u;
   tda.tda_spk_attn_rf.bits.soft_step = 0u;
   tda.tda_sub_attn.bits.gain = 0u;
   tda.tda_sub_attn.bits.soft_step = 0u;
   tda.tda_soft_mute_step.bits.az_function=0u;
   tda.tda_soft_mute_step.bits.soft_mute=1u;
   tda.tda_soft_mute_step.bits.soft_mute_time=0u;
   tda.tda_soft_mute_step.bits.soft_step_time=0u;
   tda.tda_testing.reg = 0u;

   Audio_Apply();
}

/* ===== BUTTON HANDLERS ===== */
/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
void AudioCtrl_ButtonUp(void)
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
            if(audio.bass < MAX_BASS) 
            {
               audio.bass++;
            }
            break;
        case AUDIO_MENU_MIDDLE:
            if(audio.middle < MAX_MIDDLE) 
            {
               audio.middle++;
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
        default: 
            break;
    }

    Audio_Apply();
}

/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
void AudioCtrl_ButtonDown(void)
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
            if(audio.bass > MIN_BASS)
            {
               audio.bass--;
            }
            break;
        case AUDIO_MENU_MIDDLE:
            if(audio.middle > MIN_MIDDLE)
            {
               audio.middle--;
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
        default: 
            break;
    }
    Audio_Apply();
}

/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
void AudioCtrl_ButtonMenu(void)
{
    currentMenu++;

    if(currentMenu >= AUDIO_MENU_MAX)
    {
       currentMenu = AUDIO_MENU_VOL;
    }
    menuTimer = 0u;
}


/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
void AudioCtrl_ButtonEffects(void)
{
   switch(audio_effect)
   {
     case AUDIO_EFFECT1:
          audio_effect = AUDIO_EFFECT2;
          break;
     case AUDIO_EFFECT2:
          audio_effect = AUDIO_EFFECT3;
          break;
     case AUDIO_EFFECT3:
          audio_effect = AUDIO_EFFECT4;
          break;
     case AUDIO_EFFECT4:
          audio_effect = AUDIO_EFFECT5;
          break;
     case AUDIO_EFFECT5:
          audio_effect = NO_AUDIO_EFFECT;
          break;
     case NO_AUDIO_EFFECT:
          audio_effect = AUDIO_EFFECT1;
          break;
     default:
          break;
   }
   Audio_Apply();
}

/* ===== MAIN TASK ===== */
/*******************************************************************************
 Func Name    : 
 Arguments    : none
 Return       : none
 Description  : 
*******************************************************************************/
void AudioCtrl_Task(void)
{
    menuTimer++;

    if(menuTimer > MENU_TIMEOUT_SEC)
    {
        currentMenu = AUDIO_MENU_VOL;
    }
}


