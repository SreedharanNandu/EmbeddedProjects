#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "type.h"
#include <msp430.h>

#if !defined(LOG_OPTION)
   char vol_buf[7];
   char ch_buf[5];
   char freq_buf[9];
#endif


/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
void Log_Printf(Send_Uart_Type_T id)
{
   switch(id)
   {
      case SEND_WELCOME:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_Welcome_Text[0],23u);
           break;
      case SEND_VOL:
           Send_Uart("\r\n",2u);
           #if defined(LOG_OPTION)
           Send_Uart((unsigned char*)&K_Vol_Text[si_vol][0],7u);
           #else
           Send_Uart((unsigned char*)Get_Vol_Text(si_vol),7u);
           #endif
           break;
      case SEND_CH:
           Send_Uart("\r\n",2u);
           #if defined(LOG_OPTION)
           Send_Uart((unsigned char*)&K_Ch_Text[preset_index][0],5u);
           #else
           Send_Uart((unsigned char*)Get_Ch_Text(preset_index), 5u);
           #endif
           break;
      case SEND_SAVED:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_Saved_Text[0],5u);
           break;
      case SEND_PRESET:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_Preset_Text[0],11u);
           break;
      case SEND_MANUAL:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_Manual_Text[0],11u);
           break;
      case SEND_FREQ:
           Send_Uart("\r\n",2u);
           #if defined(LOG_OPTION)
           Send_Uart((unsigned char*)&K_Freq_Text[(fm_freq-K_Fm_Min_Freq)/10u][0],8u);
           #else
           Send_Uart((unsigned char*)Get_Freq_Text(fm_freq), 8u);
           #endif
           break;
      case SEND_PWR_ON:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_PwrON_Text[0],8u);
           break;
      case SEND_PWR_OFF:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_PwrOFF_Text[0],9u);
           break;
      case SEND_MUTE:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_Mute_Text[0],5u);
           break;
      case SEND_VOL_SEL_ON:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_VolSelON_Text[0],10u);
           break;
      case SEND_VOL_SEL_OFF:
           Send_Uart("\r\n",2u);
           Send_Uart((unsigned char*)&K_VolSelOFF_Text[0],11u);
           break;
   default:
           break;
   }
}   

#if !defined(LOG_OPTION)

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
char* Get_Vol_Text(unsigned char vol)
{
    sprintf(vol_buf, "Vol %d ", vol);  
    return vol_buf;
}

/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
char* Get_Ch_Text(unsigned char ch)
{
    sprintf(ch_buf, "Ch %d ", ch);       
    return ch_buf;
}


/*******************************************************************************
 Func Name    :
 Arguments    :
 Return       :
 Description  :   
*******************************************************************************/
char* Get_Freq_Text(unsigned short int fm_freq)
{
    unsigned short int mhz ;
    unsigned short int fraction;  

    if(fm_freq)
    {
        mhz      = fm_freq / 100u;
        fraction = (fm_freq % 100u) / 10u;
    }
    if(mhz < 100u)
    {
       sprintf(freq_buf, "%d.%dMhz", mhz, fraction);    
    }
    else
    {
       sprintf(freq_buf, "%d.%dMhz", mhz, fraction);    
    }
    return freq_buf;
}

#endif

