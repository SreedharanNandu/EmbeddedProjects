/******************************************************************************
** Methode Electronics India, Bangalore
*******************************************************************************
******************************************************************************/

#ifndef APP_SI_AM_TABLE_H
#define APP_SI_AM_TABLE_H

/*****************************************************************************
System/Project Includes                            *
******************************************************************************/

#include "system.h" 
#include "APP_Si.h"




typedef enum
{
   INDEX_AM_POWER_UP=0                          ,
   INDEX_AM_GET_REV                             ,
   INDEX_AM_POWER_DOWN                          ,
   INDEX_AM_SET_PROPERTY                        ,
   INDEX_AM_GET_PROPERTY                        ,
   INDEX_AM_GET_INT_STATUS                      ,
   INDEX_AM_TUNE_FREQ                           ,
   INDEX_AM_SEEK_START                          ,
   INDEX_AM_TUNE_STATUS                         ,
   INDEX_AM_RSQ_STATUS                          ,
   INDEX_AM_AGC_STATUS                          ,
   INDEX_AM_AGC_OVERRIDE                        ,
   INDEX_AM_GPIO_CTL                            ,
   INDEX_AM_GPIO_SET                               
}Si4735_AM_Index_T;

typedef enum
{
   INDEX_AM_GPO_IEN                             ,
   INDEX_AM_DIGITAL_OUTPUT_FORMAT               ,
   INDEX_AM_DIGITAL_OUTPUT_SAMPLE_RATE          ,
   INDEX_AM_REFCLK_FREQ                         ,
   INDEX_AM_REFCLK_PRESCALE                     ,
   INDEX_AM_DEEMPHASIS                          ,
   INDEX_AM_CHANNEL_FILTER                      ,
   INDEX_AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN   ,
   INDEX_AM_MODE_AFC_SW_PULL_IN_RANGE           ,
   INDEX_AM_MODE_AFC_SW_LOCK_IN_RANGE           ,
   INDEX_AM_RSQ_INTERRUPTS                      ,
   INDEX_AM_RSQ_SNR_HIGH_THRESHOLD              ,
   INDEX_AM_RSQ_SNR_LOW_THRESHOLD	            ,
   INDEX_AM_RSQ_RSSI_HIGH_THRESHOLD             ,
   INDEX_AM_RSQ_RSSI_LOW_THRESHOLD              ,
   INDEX_AM_SOFT_MUTE_RATE                      ,
   INDEX_AM_SOFT_MUTE_SLOPE                     ,
   INDEX_AM_SOFT_MUTE_MAX_ATTENUATION           ,
   INDEX_AM_SOFT_MUTE_SNR_THRESHOLD             ,
   INDEX_AM_SEEK_BAND_BOTTOM                    ,
   INDEX_AM_SEEK_BAND_TOP                       ,
   INDEX_AM_SEEK_FREQ_SPACING                   ,
   INDEX_AM_SEEK_SNR_THRESHOLD                  ,
   INDEX_AM_SEEK_RSSI_THRESHOLD                 ,
   INDEX_AM_RX_VOLUME                           ,
   INDEX_AM_RX_HARD_MUTE                           
              
}Si4735_AM_Prop_Index_T;

typedef enum
{
  AM_POWER_UP          = 0x01,
  AM_GET_REV           = 0x10,
  AM_POWER_DOWN        = 0x11,
  AM_SET_PROPERTY      = 0x12,
  AM_GET_PROPERTY      = 0x13,
  AM_GET_INT_STATUS    = 0x14,
  AM_TUNE_FREQ         = 0x40,
  AM_SEEK_START        = 0x41,
  AM_TUNE_STATUS       = 0x42,
  AM_RSQ_STATUS        = 0x43,
  AM_AGC_STATUS        = 0x47,
  AM_AGC_OVERRIDE      = 0x48,
  AM_GPIO_CTL          = 0x80,
  AM_GPIO_SET          = 0x81

}Si4735_AM_Cmd_T;

typedef enum
{
   AM_GPO_IEN                             = 0x0001,
   AM_DIGITAL_OUTPUT_FORMAT               = 0x0102,
   AM_DIGITAL_OUTPUT_SAMPLE_RATE          = 0x0104,
   AM_REFCLK_FREQ                         = 0x0201,
   AM_REFCLK_PRESCALE                     = 0x0202,
   AM_DEEMPHASIS                          = 0x3100,
   AM_CHANNEL_FILTER                      = 0x3102,
   AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN   = 0x3103,
   AM_MODE_AFC_SW_PULL_IN_RANGE           = 0x3104,
   AM_MODE_AFC_SW_LOCK_IN_RANGE           = 0x3105,
   AM_RSQ_INTERRUPTS                      = 0x3200,
   AM_RSQ_SNR_HIGH_THRESHOLD              = 0x3201,
   AM_RSQ_SNR_LOW_THRESHOLD	              = 0x3202,
   AM_RSQ_RSSI_HIGH_THRESHOLD             = 0x3203,
   AM_RSQ_RSSI_LOW_THRESHOLD              = 0x3204,
   AM_SOFT_MUTE_RATE                      = 0x3300,
   AM_SOFT_MUTE_SLOPE                     = 0x3301,
   AM_SOFT_MUTE_MAX_ATTENUATION           = 0x3302,
   AM_SOFT_MUTE_SNR_THRESHOLD             = 0x3303,
   AM_SEEK_BAND_BOTTOM                    = 0x3400,
   AM_SEEK_BAND_TOP                       = 0x3401,
   AM_SEEK_FREQ_SPACING                   = 0x3402,
   AM_SEEK_SNR_THRESHOLD                  = 0x3403,
   AM_SEEK_RSSI_THRESHOLD                 = 0x3404,
   AM_RX_VOLUME                           = 0x4000,
   AM_RX_HARD_MUTE                        = 0x4001
              
}Si4735_AM_Prop_T;


typedef enum
{
   CMD_SIZE_AM_POWER_UP                              = 3,
   CMD_SIZE_AM_GET_REV                               = 1,
   CMD_SIZE_AM_POWER_DOWN                            = 1,
   CMD_SIZE_AM_SET_PROPERTY                          = 6,
   CMD_SIZE_AM_GET_PROPERTY                          = 4,
   CMD_SIZE_AM_GET_INT_STATUS                        = 1,
   CMD_SIZE_AM_TUNE_FREQ                             = 6,
   CMD_SIZE_AM_SEEK_START                            = 6,
   CMD_SIZE_AM_TUNE_STATUS                           = 2,
   CMD_SIZE_AM_RSQ_STATUS                            = 2,
   CMD_SIZE_AM_AGC_STATUS                            = 1,
   CMD_SIZE_AM_AGC_OVERRIDE                          = 3,
   CMD_SIZE_AM_GPIO_CTL                              = 2,
   CMD_SIZE_AM_GPIO_SET                              = 2


}Si4735_AM_Cmd_Size_T;

typedef enum
{
   CMD_SIZE_AM_GPO_IEN                             = 2,
   CMD_SIZE_AM_DIGITAL_OUTPUT_FORMAT               = 2,
   CMD_SIZE_AM_DIGITAL_OUTPUT_SAMPLE_RATE          = 2,
   CMD_SIZE_AM_REFCLK_FREQ                         = 2,
   CMD_SIZE_AM_REFCLK_PRESCALE                     = 2,
   CMD_SIZE_AM_DEEMPHASIS                          = 2,
   CMD_SIZE_AM_CHANNEL_FILTER                      = 2,
   CMD_SIZE_AM_AUTOMATIC_VOLUME_CONTROL_MAX_GAIN   = 2,
   CMD_SIZE_AM_MODE_AFC_SW_PULL_IN_RANGE           = 2,
   CMD_SIZE_AM_MODE_AFC_SW_LOCK_IN_RANGE           = 2,
   CMD_SIZE_AM_RSQ_INTERRUPTS                      = 2,
   CMD_SIZE_AM_RSQ_SNR_HIGH_THRESHOLD              = 2,
   CMD_SIZE_AM_RSQ_SNR_LOW_THRESHOLD               = 2,
   CMD_SIZE_AM_RSQ_RSSI_HIGH_THRESHOLD             = 2,
   CMD_SIZE_AM_RSQ_RSSI_LOW_THRESHOLD              = 2,
   CMD_SIZE_AM_SOFT_MUTE_RATE                      = 2,
   CMD_SIZE_AM_SOFT_MUTE_SLOPE                     = 2,
   CMD_SIZE_AM_SOFT_MUTE_MAX_ATTENUATION           = 2,
   CMD_SIZE_AM_SOFT_MUTE_SNR_THRESHOLD             = 2,
   CMD_SIZE_AM_SEEK_BAND_BOTTOM                    = 2,
   CMD_SIZE_AM_SEEK_BAND_TOP                       = 2,
   CMD_SIZE_AM_SEEK_FREQ_SPACING                   = 2,
   CMD_SIZE_AM_SEEK_SNR_THRESHOLD                  = 2,
   CMD_SIZE_AM_SEEK_RSSI_THRESHOLD                 = 2,
   CMD_SIZE_AM_RX_VOLUME                           = 2,
   CMD_SIZE_AM_RX_HARD_MUTE                        = 2,
              
}Si4735_AM_Prop_Size_T;



              
typedef enum
{
   RESP_SIZE_AM_POWER_UP                              = 1,
   RESP_SIZE_AM_GET_REV                               = 9,
   RESP_SIZE_AM_POWER_DOWN                            = 1,
   RESP_SIZE_AM_SET_PROPERTY                          = 0,
   RESP_SIZE_AM_GET_PROPERTY                          = 4,
   RESP_SIZE_AM_GET_INT_STATUS                        = 1,
   RESP_SIZE_AM_TUNE_FREQ                             = 1,
   RESP_SIZE_AM_SEEK_START                            = 1,
   RESP_SIZE_AM_TUNE_STATUS                           = 8,
   RESP_SIZE_AM_RSQ_STATUS                            = 6,
   RESP_SIZE_AM_AGC_STATUS                            = 3,
   RESP_SIZE_AM_AGC_OVERRIDE                          = 1,
   RESP_SIZE_AM_GPIO_CTL                              = 1,
   RESP_SIZE_AM_GPIO_SET                              = 1

}Si4735_AM_Resp_Size_T;


typedef struct
{
   Si4735_AM_Cmd_Size_T  cmd_size;
   Si4735_AM_Resp_Size_T resp_size;

}Si4735_AM_Table_T;
extern const Si4735_AM_Table_T Si4735_AM_Table[];



/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/


#endif
