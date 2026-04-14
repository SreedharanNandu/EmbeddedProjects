/*****************************************************************************
 *   memmap_radio.h:  
******************************************************************************/
#ifndef MEMMAP_RADIO_H
#define MEMMAP_RADIO_H

   #ifdef RADIO_START_SEC_CAL_CONST
       #pragma SET_DATA_SECTION(".cal_const")
       #undef RADIO_START_SEC_CAL_CONST
   #endif
   
   #ifdef RADIO_STOP_SEC_CAL_CONST
       #pragma SET_DATA_SECTION(".cal_const")
       #undef RADIO_STOP_SEC_CAL_CONST
   #endif
   
   #ifdef RADIO_START_SEC_VAR
      #pragma SET_DATA_SECTION(".bss_app")
      #undef RADIO_START_SEC_VAR
   #endif
   
   #ifdef RADIO_STOP_SEC_VAR
      #pragma SET_DATA_SECTION()
      #undef RADIO_STOP_SEC_VAR
   #endif
   
   #ifdef RADIO_START_SEC_INIT_VAR
      #pragma SET_DATA_SECTION(".data_app")
      #undef RADIO_START_SEC_INIT_VAR
   #endif
   
   #ifdef RADIO_STOP_SEC_INIT_VAR
      #pragma SET_DATA_SECTION()
      #undef RADIO_STOP_SEC_INIT_VAR
   #endif

#endif
