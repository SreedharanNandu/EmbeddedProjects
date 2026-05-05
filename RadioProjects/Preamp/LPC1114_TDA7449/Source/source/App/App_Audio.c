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

 
   tda.tda_in.bits.input_sel = audio.input;
   tda.tda_gain.bits.input_gain = audio.gain;
   tda.tda_volume.bits.gain = K_Tda_Vol_Reg[audio.volume];
   tda.tda_treble.bits.gain = K_Tda_Treble_Reg[audio.treble];
   tda.tda_bass.bits.gain = K_Tda_Bass_Reg[audio.bass];
   tda.tda_spk_attn_l.bits.gain = 0u;
   tda.tda_spk_attn_r.bits.gain = 0u;
 
 
 
   buf[0] = 0u; 
   buf[1] = tda.tda_in.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);
   buf[0] = 1u; 
   buf[1] = tda.tda_gain.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);
   buf[0] = 2u; 
   buf[1] = tda.tda_volume.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);
   buf[0] = 4u; 
   buf[1] = tda.tda_bass.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);
   buf[0] = 5u; 
   buf[1] = tda.tda_treble.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);
   buf[0] = 6u; 
   buf[1] = tda.tda_spk_attn_r.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);
   buf[0] = 7u; 
   buf[1] = tda.tda_spk_attn_l.reg;
   Write_I2C(TDA7449_ADDR, buf, 2u);

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
   audio.treble = 10;
   audio.volume = 7u;
   audio.mute = 0u;
   
      
   tda.tda_in.bits.input_sel = audio.input;
   tda.tda_gain.bits.input_gain = audio.gain;
   tda.tda_volume.bits.gain = K_Tda_Vol_Reg[audio.volume];
   tda.tda_treble.bits.gain = K_Tda_Treble_Reg[audio.treble];
   tda.tda_bass.bits.gain = K_Tda_Bass_Reg[audio.bass];
   tda.tda_spk_attn_l.bits.gain = 0u;
   tda.tda_spk_attn_r.bits.gain = 0u;

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


