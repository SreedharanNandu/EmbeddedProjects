/******************************************************************************
** Methode Electronics India, Bangalore
*******************************************************************************
******************************************************************************/

#ifndef APP_SI_FM_TABLE_H
#define APP_SI_FM_TABLE_H

/*****************************************************************************
System/Project Includes                            *
******************************************************************************/

#include "system.h" 
#include "APP_Si.h"




typedef enum
{
   INDEX_POWER_UP=0                             ,
   INDEX_GET_REV                                ,
   INDEX_POWER_DOWN                             ,
   INDEX_SET_PROPERTY                           ,
   INDEX_GET_PROPERTY                           ,
   INDEX_GET_INT_STATUS                         ,
   INDEX_FM_TUNE_FREQ                           ,
   INDEX_FM_SEEK_START                          ,
   INDEX_FM_TUNE_STATUS                         ,
   INDEX_FM_RSQ_STATUS                          ,
   INDEX_FM_RDS_STATUS                          ,
   INDEX_FM_AGC_STATUS                          ,
   INDEX_FM_AGC_OVERRIDE                        ,
   INDEX_GPIO_CTL                               ,
   INDEX_GPIO_SET                               
}Si4735_FM_Index_T;

typedef enum
{
   INDEX_GPO_IEN                                ,
   INDEX_DIGITAL_OUTPUT_FORMAT                  ,
   INDEX_DIGITAL_OUTPUT_SAMPLE_RATE             ,
   INDEX_REFCLK_FREQ                            ,
   INDEX_REFCLK_PRESCALE                        ,
   INDEX_FM_DEEMPHASIS                          ,
   INDEX_FM_CHANNEL_FILTER                      ,
   INDEX_FM_BLEND_STEREO_THRESHOLD              ,
   INDEX_FM_BLEND_MONO_THRESHOLD                ,
   INDEX_FM_ANTENNA_INPUT                       ,
   INDEX_FM_MAX_TUNE_ERROR                      ,
   INDEX_FM_RSQ_INT_SOURCE                      ,
   INDEX_FM_RSQ_SNR_HI_THRESHOLD                ,
   INDEX_FM_RSQ_SNR_LO_THRESHOLD                ,
   INDEX_FM_RSQ_RSSI_HI_THRESHOLD               ,
   INDEX_FM_RSQ_RSSI_LO_THRESHOLD               ,
   INDEX_FM_RSQ_MULTIPATH_HI_THRESHOLD          ,
   INDEX_FM_RSQ_MULTIPATH_LO_THRESHOLD          ,
   INDEX_FM_RSQ_BLEND_THRESHOLD                 ,
   INDEX_FM_SOFT_MUTE_RATE                      ,
   INDEX_FM_SOFT_MUTE_SLOPE                     ,
   INDEX_FM_SOFT_MUTE_MAX_ATTENUATION           ,
   INDEX_FM_SOFT_MUTE_SNR_THRESHOLD             ,
   INDEX_FM_SOFT_MUTE_RELEASE_RATE              ,
   INDEX_FM_SOFT_MUTE_ATTACK_RATE               ,
   INDEX_FM_SEEK_BAND_BOTTOM                    ,
   INDEX_FM_SEEK_BAND_TOP                       ,
   INDEX_FM_SEEK_FREQ_SPACING                   ,
   INDEX_FM_SEEK_TUNE_SNR_THRESHOLD             ,
   INDEX_FM_SEEK_TUNE_RSSI_TRESHOLD             ,
   INDEX_FM_RDS_INT_SOURCE                      ,
   INDEX_FM_RDS_INT_FIFO_COUNT                  ,
   INDEX_FM_RDS_CONFIG                          ,
   INDEX_FM_RDS_CONFIDENCE                      ,
   INDEX_FM_AGC_ATTACK_RATE                     ,
   INDEX_FM_AGC_RELEASE_RATE                    ,
   INDEX_FM_BLEND_RSSI_STEREO_THRESHOLD         ,
   INDEX_FM_BLEND_RSSI_MONO_THRESHOLD           ,
   INDEX_FM_BLEND_RSSI_ATTACK_RATE              ,
   INDEX_FM_BLEND_RSSI_RELEASE_RATE             ,
   INDEX_FM_BLEND_SNR_STEREO_THRESHOLD          ,
   INDEX_FM_BLEND_SNR_MONO_THRESHOLD            ,
   INDEX_FM_BLEND_SNR_ATTACK_RATE               ,
   INDEX_FM_BLEND_SNR_RELEASE_RATE              ,
   INDEX_FM_BLEND_MULTIPATH_STEREO_THRESHOLD    ,
   INDEX_FM_BLEND_MULTIPATH_MONO_THRESHOLD      ,
   INDEX_FM_BLEND_MULTIPATH_ATTACK_RATE         ,
   INDEX_FM_BLEND_MULTIPATH_RELEASE_RATE        ,
   INDEX_FM_BLEND_MAX_STEREO_SEPARATION         ,
   INDEX_FM_NB_DETECT_THRESHOLD                 ,
   INDEX_FM_NB_INTERVAL                         ,
   INDEX_FM_NB_RATE                             ,
   INDEX_FM_NB_IIR_FILTER                       ,
   INDEX_FM_NB_DELAY                            ,
   INDEX_FM_HICUT_SNR_HIGH_THRESHOLD            ,
   INDEX_FM_HICUT_SNR_LOW_THRESHOLD             ,
   INDEX_FM_HICUT_ATTACK_RATE                   ,
   INDEX_FM_HICUT_RELEASE_RATE                  ,
   INDEX_FM_HICUT_MULTIPATH_TRIGGER_THRESHOLD   ,
   INDEX_FM_HICUT_MULTIPATH_END_THRESHOLD       ,
   INDEX_FM_HICUT_CUTOFF_FREQUENCY              ,
   INDEX_RX_VOLUME                              ,
   INDEX_RX_HARD_MUTE                           
              
}Si4735_FM_Prop_Index_T;

typedef enum
{
   POWER_UP                               = 0x01,
   GET_REV                                = 0x10,
   POWER_DOWN                             = 0x11,
   SET_PROPERTY                           = 0x12,
   GET_PROPERTY                           = 0x13,
   GET_INT_STATUS                         = 0x14,
   FM_TUNE_FREQ                           = 0x20,
   FM_SEEK_START                          = 0x21,
   FM_TUNE_STATUS                         = 0x22,
   FM_RSQ_STATUS                          = 0x23,
   FM_RDS_STATUS                          = 0x24,
   FM_AGC_STATUS                          = 0x27,
   FM_AGC_OVERRIDE                        = 0x28,
   GPIO_CTL                               = 0x80,
   GPIO_SET                               = 0x81
}Si4735_FM_Cmd_T;
typedef enum
{
   GPO_IEN                                = 0x0001,
   DIGITAL_OUTPUT_FORMAT                  = 0x0102,
   DIGITAL_OUTPUT_SAMPLE_RATE             = 0x0104,
   REFCLK_FREQ                            = 0x0201,
   REFCLK_PRESCALE                        = 0x0202,
   FM_DEEMPHASIS                          = 0x1100,
   FM_CHANNEL_FILTER                      = 0x1102,
   FM_BLEND_STEREO_THRESHOLD              = 0x1105,
   FM_BLEND_MONO_THRESHOLD                = 0x1106,
   FM_ANTENNA_INPUT                       = 0x1107,
   FM_MAX_TUNE_ERROR                      = 0x1108,
   FM_RSQ_INT_SOURCE                      = 0x1200,
   FM_RSQ_SNR_HI_THRESHOLD                = 0x1201,
   FM_RSQ_SNR_LO_THRESHOLD                = 0x1202,
   FM_RSQ_RSSI_HI_THRESHOLD               = 0x1203,
   FM_RSQ_RSSI_LO_THRESHOLD               = 0x1204,
   FM_RSQ_MULTIPATH_HI_THRESHOLD          = 0x1205,
   FM_RSQ_MULTIPATH_LO_THRESHOLD          = 0x1206,
   FM_RSQ_BLEND_THRESHOLD                 = 0x1207,
   FM_SOFT_MUTE_RATE                      = 0x1300,
   FM_SOFT_MUTE_SLOPE                     = 0x1301,
   FM_SOFT_MUTE_MAX_ATTENUATION           = 0x1302,
   FM_SOFT_MUTE_SNR_THRESHOLD             = 0x1303,
   FM_SOFT_MUTE_RELEASE_RATE              = 0x1304,
   FM_SOFT_MUTE_ATTACK_RATE               = 0x1305,
   FM_SEEK_BAND_BOTTOM                    = 0x1400,
   FM_SEEK_BAND_TOP                       = 0x1401,
   FM_SEEK_FREQ_SPACING                   = 0x1402,
   FM_SEEK_TUNE_SNR_THRESHOLD             = 0x1403,
   FM_SEEK_TUNE_RSSI_TRESHOLD             = 0x1404,
   FM_RDS_INT_SOURCE                      = 0x1500,
   FM_RDS_INT_FIFO_COUNT                  = 0x1501,
   FM_RDS_CONFIG                          = 0x1502,
   FM_RDS_CONFIDENCE                      = 0x1503,
   FM_AGC_ATTACK_RATE                     = 0x1700,
   FM_AGC_RELEASE_RATE                    = 0x1701,
   FM_BLEND_RSSI_STEREO_THRESHOLD         = 0x1800,
   FM_BLEND_RSSI_MONO_THRESHOLD           = 0x1801,
   FM_BLEND_RSSI_ATTACK_RATE              = 0x1802,
   FM_BLEND_RSSI_RELEASE_RATE             = 0x1803,
   FM_BLEND_SNR_STEREO_THRESHOLD          = 0x1804,
   FM_BLEND_SNR_MONO_THRESHOLD            = 0x1805,
   FM_BLEND_SNR_ATTACK_RATE               = 0x1806,
   FM_BLEND_SNR_RELEASE_RATE              = 0x1807,
   FM_BLEND_MULTIPATH_STEREO_THRESHOLD    = 0x1808,
   FM_BLEND_MULTIPATH_MONO_THRESHOLD      = 0x1809,
   FM_BLEND_MULTIPATH_ATTACK_RATE         = 0x180A,
   FM_BLEND_MULTIPATH_RELEASE_RATE        = 0x180B,
   FM_BLEND_MAX_STEREO_SEPARATION         = 0x180C,
   FM_NB_DETECT_THRESHOLD                 = 0x1900,
   FM_NB_INTERVAL                         = 0x1901,
   FM_NB_RATE                             = 0x1902,
   FM_NB_IIR_FILTER                       = 0x1903,
   FM_NB_DELAY                            = 0x1904,
   FM_HICUT_SNR_HIGH_THRESHOLD            = 0x1A00,
   FM_HICUT_SNR_LOW_THRESHOLD             = 0x1A01,
   FM_HICUT_ATTACK_RATE                   = 0x1A02,
   FM_HICUT_RELEASE_RATE                  = 0x1A03,
   FM_HICUT_MULTIPATH_TRIGGER_THRESHOLD   = 0x1A04,
   FM_HICUT_MULTIPATH_END_THRESHOLD       = 0x1A05,
   FM_HICUT_CUTOFF_FREQUENCY              = 0x1A06,
   RX_VOLUME                              = 0x4000,
   RX_HARD_MUTE                           = 0x4001
              
}Si4735_FM_Prop_T;


typedef enum
{
   CMD_SIZE_POWER_UP                               = 3,
   CMD_SIZE_GET_REV                                = 1,
   CMD_SIZE_POWER_DOWN                             = 1,
   CMD_SIZE_SET_PROPERTY                           = 6,
   CMD_SIZE_GET_PROPERTY                           = 4,
   CMD_SIZE_GET_INT_STATUS                         = 1,
   CMD_SIZE_FM_TUNE_FREQ                           = 5,
   CMD_SIZE_FM_SEEK_START                          = 2,
   CMD_SIZE_FM_TUNE_STATUS                         = 2,
   CMD_SIZE_FM_RSQ_STATUS                          = 2,
   CMD_SIZE_FM_RDS_STATUS                          = 2,
   CMD_SIZE_FM_AGC_STATUS                          = 1,
   CMD_SIZE_FM_AGC_OVERRIDE                        = 3,
   CMD_SIZE_GPIO_CTL                               = 2,
   CMD_SIZE_GPIO_SET                               = 2,
}Si4735_FM_Cmd_Size_T;

typedef enum
{
   CMD_SIZE_GPO_IEN                                = 2,
   CMD_SIZE_DIGITAL_OUTPUT_FORMAT                  = 2,
   CMD_SIZE_DIGITAL_OUTPUT_SAMPLE_RATE             = 2,
   CMD_SIZE_REFCLK_FREQ                            = 2,
   CMD_SIZE_REFCLK_PRESCALE                        = 2,
   CMD_SIZE_FM_DEEMPHASIS                          = 2,
   CMD_SIZE_FM_CHANNEL_FILTER                      = 2,
   CMD_SIZE_FM_BLEND_STEREO_THRESHOLD              = 2,
   CMD_SIZE_FM_BLEND_MONO_THRESHOLD                = 2,
   CMD_SIZE_FM_ANTENNA_INPUT                       = 2,
   CMD_SIZE_FM_MAX_TUNE_ERROR                      = 2,
   CMD_SIZE_FM_RSQ_INT_SOURCE                      = 2,
   CMD_SIZE_FM_RSQ_SNR_HI_THRESHOLD                = 2,
   CMD_SIZE_FM_RSQ_SNR_LO_THRESHOLD                = 2,
   CMD_SIZE_FM_RSQ_RSSI_HI_THRESHOLD               = 2,
   CMD_SIZE_FM_RSQ_RSSI_LO_THRESHOLD               = 2,
   CMD_SIZE_FM_RSQ_MULTIPATH_HI_THRESHOLD          = 2,
   CMD_SIZE_FM_RSQ_MULTIPATH_LO_THRESHOLD          = 2,
   CMD_SIZE_FM_RSQ_BLEND_THRESHOLD                 = 2,
   CMD_SIZE_FM_SOFT_MUTE_RATE                      = 2,
   CMD_SIZE_FM_SOFT_MUTE_SLOPE                     = 2,
   CMD_SIZE_FM_SOFT_MUTE_MAX_ATTENUATION           = 2,
   CMD_SIZE_FM_SOFT_MUTE_SNR_THRESHOLD             = 2,
   CMD_SIZE_FM_SOFT_MUTE_RELEASE_RATE              = 2,
   CMD_SIZE_FM_SOFT_MUTE_ATTACK_RATE               = 2,
   CMD_SIZE_FM_SEEK_BAND_BOTTOM                    = 2,
   CMD_SIZE_FM_SEEK_BAND_TOP                       = 2,
   CMD_SIZE_FM_SEEK_FREQ_SPACING                   = 2,
   CMD_SIZE_FM_SEEK_TUNE_SNR_THRESHOLD             = 2,
   CMD_SIZE_FM_SEEK_TUNE_RSSI_TRESHOLD             = 2,
   CMD_SIZE_FM_RDS_INT_SOURCE                      = 2,
   CMD_SIZE_FM_RDS_INT_FIFO_COUNT                  = 2,
   CMD_SIZE_FM_RDS_CONFIG                          = 2,
   CMD_SIZE_FM_RDS_CONFIDENCE                      = 2,
   CMD_SIZE_FM_AGC_ATTACK_RATE                     = 2,
   CMD_SIZE_FM_AGC_RELEASE_RATE                    = 2,
   CMD_SIZE_FM_BLEND_RSSI_STEREO_THRESHOLD         = 2,
   CMD_SIZE_FM_BLEND_RSSI_MONO_THRESHOLD           = 2,
   CMD_SIZE_FM_BLEND_RSSI_ATTACK_RATE              = 2,
   CMD_SIZE_FM_BLEND_RSSI_RELEASE_RATE             = 2,
   CMD_SIZE_FM_BLEND_SNR_STEREO_THRESHOLD          = 2,
   CMD_SIZE_FM_BLEND_SNR_MONO_THRESHOLD            = 2,
   CMD_SIZE_FM_BLEND_SNR_ATTACK_RATE               = 2,
   CMD_SIZE_FM_BLEND_SNR_RELEASE_RATE              = 2,
   CMD_SIZE_FM_BLEND_MULTIPATH_STEREO_THRESHOLD    = 2,
   CMD_SIZE_FM_BLEND_MULTIPATH_MONO_THRESHOLD      = 2,
   CMD_SIZE_FM_BLEND_MULTIPATH_ATTACK_RATE         = 2,
   CMD_SIZE_FM_BLEND_MULTIPATH_RELEASE_RATE        = 2,
   CMD_SIZE_FM_BLEND_MAX_STEREO_SEPARATION         = 2,
   CMD_SIZE_FM_NB_DETECT_THRESHOLD                 = 2,
   CMD_SIZE_FM_NB_INTERVAL                         = 2,
   CMD_SIZE_FM_NB_RATE                             = 2,
   CMD_SIZE_FM_NB_IIR_FILTER                       = 2,
   CMD_SIZE_FM_NB_DELAY                            = 2,
   CMD_SIZE_FM_HICUT_SNR_HIGH_THRESHOLD            = 2,
   CMD_SIZE_FM_HICUT_SNR_LOW_THRESHOLD             = 2,
   CMD_SIZE_FM_HICUT_ATTACK_RATE                   = 2,
   CMD_SIZE_FM_HICUT_RELEASE_RATE                  = 2,
   CMD_SIZE_FM_HICUT_MULTIPATH_TRIGGER_THRESHOLD   = 2,
   CMD_SIZE_FM_HICUT_MULTIPATH_END_THRESHOLD       = 2,
   CMD_SIZE_FM_HICUT_CUTOFF_FREQUENCY              = 2,
   CMD_SIZE_RX_VOLUME                              = 2,
   CMD_SIZE_RX_HARD_MUTE                           = 2
              
}Si4735_FM_Prop_Size_T;

              
typedef enum
{
   RESP_SIZE_POWER_UP                               = 1,
   RESP_SIZE_GET_REV                                = 9,
   RESP_SIZE_POWER_DOWN                             = 1,
   RESP_SIZE_SET_PROPERTY                           = 0,
   RESP_SIZE_GET_PROPERTY                           = 4,
   RESP_SIZE_GET_INT_STATUS                         = 1,
   RESP_SIZE_FM_TUNE_FREQ                           = 1,
   RESP_SIZE_FM_SEEK_START                          = 1,
   RESP_SIZE_FM_TUNE_STATUS                         = 8,
   RESP_SIZE_FM_RSQ_STATUS                          = 8,
   RESP_SIZE_FM_RDS_STATUS                          = 13,
   RESP_SIZE_FM_AGC_STATUS                          = 3,
   RESP_SIZE_FM_AGC_OVERRIDE                        = 1,
   RESP_SIZE_GPIO_CTL                               = 1,
   RESP_SIZE_GPIO_SET                               = 1
              
}Si4735_FM_Resp_Size_T;


typedef struct
{
   Si4735_FM_Cmd_Size_T  cmd_size;
   Si4735_FM_Resp_Size_T resp_size;

}Si4735_FM_Table_T;

extern const Si4735_FM_Table_T Si4735_FM_Table[];




/*****************************************************************************
*                             Macro Definitions                             
******************************************************************************/


#endif
