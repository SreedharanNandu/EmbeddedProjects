/*
 * bm.h
 */

#ifndef BM_H
#define BM_H

#include "system.h"

#define UART_RESET_TIMER 65530u

/**
 * BM83 commands OP code list.
 * Specified commands OP code list of BM83 .
 */
#define BM83_CMD_MAKE_CALL                                  0x00
#define BM83_CMD_MAKE_EXTENSION_CALL                        0x01
#define BM83_CMD_MMI_ACTION                                 0x02
#define BM83_CMD_EVENT_MASK_SETTING                         0x03
#define BM83_CMD_MUSIC_CONTROL                              0x04
#define BM83_CMD_CHANGE_DEVICE_NAME                         0x05
#define BM83_CMD_CHANGE_PIN_CODE                            0x06
#define BM83_CMD_SET_BTM_PARAM                              0x07
#define BM83_CMD_GET_BTM_VERSION                            0x08
#define BM83_CMD_VENDOR_AT_CMD                              0x0A
#define BM83_CMD_AVRCP_SPEC_CMD                             0x0B
#define BM83_CMD_AVRCP_GROUP_NAVIGATION                     0x0C
#define BM83_CMD_READ_LINK_STATUS                           0x0D
#define BM83_CMD_READ_PAIRING_RECORD                        0x0E
#define BM83_CMD_READ_LOCAL_BD_ADDR                         0x0F
#define BM83_CMD_READ_LOCAL_DEVICE_NAME                     0x10
#define BM83_CMD_SEND_SPP_DATA                              0x12
#define BM83_CMD_BTM_UTILITY_FUNCTION                       0x13
#define BM83_CMD_EVENT_ACK                                  0x14
#define BM83_CMD_ADDITIONAL_PROFILE_LINK_SETUP              0x15
#define BM83_CMD_READ_LINKED_DEVICE_INFO                    0x16
#define BM83_CMD_PROFILE_LINK_BACK                          0x17
#define BM83_CMD_DISCONNECT                                 0x18
#define BM83_CMD_USER_CONFIRM_SPP_REQ_REPLY                 0x1A
#define BM83_CMD_SET_HF_GAIN_LEVEL                          0x1B
#define BM83_CMD_EQ_MODE_SETTING                            0x1C
#define BM83_CMD_DSP_NR_CTRL                                0x1D
#define BM83_CMD_GPIO_CTRL                                  0x1E
#define BM83_CMD_BT_MCU_UART_RX_BUFF_SIZE                   0x1F
#define BM83_CMD_VOICE_PROMPT_CMD                           0x20
#define BM83_CMD_SET_OVERALL_GAIN                           0x23
#define BM83_CMD_READ_BTM_SETTING                           0x24
#define BM83_CMD_READ_BTM_BATTERY_CHARGE_STATUS             0x25
#define BM83_CMD_MCU_UPDATE                                 0x26
#define BM83_CMD_REPORT_BATTERY_CAPACITY                    0x27
#define BM83_CMD_LE_ANCS_SERVICE                            0x28
#define BM83_CMD_LE_SIGNALING                               0x29
#define BM83_CMD_MSPK_VENDOR                                0x2A
#define BM83_CMD_READ_MSPK_LINK_STATUS                      0x2B
#define BM83_CMD_MSPK_SYNC_AUDIO_EFFECT                     0x2C
#define BM83_CMD_LE_GATT                                    0x2D
#define BM83_CMD_LE_APP                                     0x2F
#define BM83_CMD_DSP_RUNTIME_PROGRAM                        0x30
#define BM83_CMD_READ_VENDOR_EEPROM                         0x31
#define BM83_CMD_READ_IC_VERSION                            0x32
#define BM83_CMD_READ_LINK_MODE                             0x34
#define BM83_CMD_CONFIGURE_VENDOR_PARAM                     0x35
#define BM83_CMD_MSPK_EXCHANGE_LINK_INFO                    0x37
#define BM83_CMD_MSPK_SET_GIAC                              0x38
#define BM83_CMD_READ_FEATURE_LIST                          0x39
#define BM83_CMD_PERSONAL_MSPK_GROUP_CTRL                   0x3A
#define BM83_CMD_TEST_DEVICE                                0x3B
#define BM83_CMD_READ_EEPROM                                0x3C
#define BM83_CMD_WRITE_EEPROM                               0x3D
#define BM83_CMD_LE_SIGNALING_2                             0x3E
#define BM83_CMD_PBAPC                                      0x3F
#define BM83_CMD_TWS                                        0x40
#define BM83_CMD_AVRCP_BROWSING                             0x41
#define BM83_CMD_READ_PAIRED_LINK_KEY_INFO                  0x42
#define BM83_CMD_AUDIO_TRANSCEIVER                          0x44
#define BM83_CMD_BUTTON_MMI_SETTING                         0x46
#define BM83_CMD_BUTTON_OPERATION                           0x47
#define BM83_CMD_READ_BUTTON_MMI_SETTING                    0x48
#define BM83_CMD_DFU                                        0x49
#define BM83_CMD_AVRCP_VENDOR_DEPENDENT                     0x4A
#define BM83_CMD_CONCERT_MODE_ENDLESS_GROUPING              0x4B
#define BM83_CMD_READ_RUNTIME_LATENCY                       0x4C
#define BM83_CMD_TOGGLE_AUDIO_SOURCE                        0xCC

/**
 * BM83 events OP code list.
 * Specified events OP code list of BM83 .
 */
#define BM83_EVT_CMD_ACK                                    0x00
#define BM83_EVT_BTM_STATE                                  0x01
#define BM83_EVT_CALL_STATUS                                0x02
#define BM83_EVT_CALL_ID                                    0x03
#define BM83_EVT_SMS_INDICATION                             0x04
#define BM83_EVT_MISS_CALL_INDICATION                       0x05
#define BM83_EVT_PHONE_MAX_BATTERY_LEVEL                    0x06
#define BM83_EVT_PHONE_BATTERY_LEVEL                        0x07
#define BM83_EVT_PHONE_ROAMING_STATUS                       0x08
#define BM83_EVT_PHONE_MAX_SIGNAL_STRENGTH                  0x09
#define BM83_EVT_PHONE_SIGNAL_STRENGTH                      0x0A
#define BM83_EVT_PHONE_SERVICE_STATUS                       0x0B
#define BM83_EVT_BATTERY_LEVEL                              0x0C
#define BM83_EVT_CHARGER_STATUS                             0x0D
#define BM83_EVT_RESET_TO_DEFAULT                           0x0E
#define BM83_EVT_VOLUME_LEVEL                               0x0F
#define BM83_EVT_EQ_MODE                                    0x10
#define BM83_EVT_LINKED_DEVICE_INFO                         0x17
#define BM83_EVT_BTM_VERSION                                0x18
#define BM83_EVT_CALL_LIST_REPORT                           0x19
#define BM83_EVT_AVRCP_SPEC_RSP                             0x1A
#define BM83_EVT_BTM_UTILITY_REQ                            0x1B
#define BM83_EVT_VENDOR_AT_CMD_RSP                          0x1C
#define BM83_EVT_UNKNOW_AT_RESULT                           0x1D
#define BM83_EVT_REPORT_LINK_STATUS                         0x1E
#define BM83_EVT_REPORT_PAIRING_RECORD                      0x1F
#define BM83_EVT_REPORT_LOCAL_BD_ADDR                       0x20
#define BM83_EVT_REPORT_LOCAL_DEVICE_NAME                   0x21
#define BM83_EVT_REPORT_SPP_DATA                            0x22
#define BM83_EVT_REPORT_LINK_BACK_STATUS                    0x23
#define BM83_EVT_RINGTONE_FINISH_INDICATION                 0x24
#define BM83_EVT_USER_CONFIRM_SSP_REQ                       0x25
#define BM83_EVT_REPORT_AVRCP_VOL_CTRL                      0x26
#define BM83_EVT_REPORT_INPUT_SIGNAL_LEVEL                  0x27
#define BM83_EVT_REPORT_IAP_INFO                            0x28
#define BM83_EVT_REPORT_AVRCP_ABS_VOL_CTRL                  0x29
#define BM83_EVT_REPORT_VOICE_PROMPT_STATUS                 0x2A
#define BM83_EVT_REPORT_TYPE_CODEC                          0x2D
#define BM83_EVT_REPORT_TYPE_BTM_SETTING                    0x2E
#define BM83_EVT_REPORT_MCU_UPDATE_REPLY                    0x2F
#define BM83_EVT_REPORT_BTM_INITIAL_STATUS                  0x30
#define BM83_EVT_REPORT_LE_ANCS_EVENT                       0x31
#define BM83_EVT_REPORT_LE_EVENT                            0x32
#define BM83_EVT_REPORT_MSPK_STATUS                         0x33
#define BM83_EVT_REPORT_MSPK_VENDOR_EVENT                   0x34
#define BM83_EVT_REPORT_MSPK_AUDIO_SETTING                  0x35
#define BM83_EVT_REPORT_MSPK_EFFECT_STATUS                  0x36
#define BM83_EVT_REPORT_VENDOR_EEPROM                       0x37
#define BM83_EVT_REPORT_IC_VERSION_INFO                     0x38
#define BM83_EVT_REPORT_CUSTOMER_GATT_ATTRIBUTE_DATA        0x39
#define BM83_EVT_REPORT_LINK_MODE                           0x3A
#define BM83_EVT_REPORT_MSPK_EXCHANGE_LINK_INFO             0x3D
#define BM83_EVT_REPORT_BT_CLK                              0x3F
#define BM83_EVT_REPORT_FEATURE_LIST                        0x40
#define BM83_EVT_REPORT_TEST_RESULT                         0x41
#define BM83_EVT_REPORT_READ_EEPROM_DATA                    0x42
#define BM83_EVT_PBAPC_EVENT                                0x43
#define BM83_EVT_AVRCP_BROWSING_EVENT                       0x44
#define BM83_EVT_REPORT_PAIRED_LINK_KEY_INFO                0x45
#define BM83_EVT_REPORT_TWS_RX_VENDOR_EVENT                 0x53
#define BM83_EVT_REPORT_TWS_LOCAL_DEVICE_STATUS             0x54
#define BM83_EVT_REPORT_TWS_VAD_DATA                        0x55
#define BM83_EVT_REPORT_TWS_RADIO_CONDITION                 0x56
#define BM83_EVT_REPORT_TWS_EAR_BUD_POSITION                0x57
#define BM83_EVT_REPORT_TWS_SECONDARY_DEVICE_STATUS         0x58
#define BM83_EVT_AUDIO_TRANSCEIVER_EVENT_STATUS             0x5A
#define BM83_EVT_READ_BUTTON_MMI_SETTING_REPLY              0x5C
#define BM83_EVT_AVRCP_VENDOR_DEPENDENT_RSP                 0x5D
#define BM83_EVT_RUNTIME_LATENCY                            0x5E

/**
 * BM83 MMI actions list.
 * Specified MMI actions list of BM83 .
 */
#define BM83_MMI_ADD_REMOVE_SCO_LINK                        0x01
#define BM83_MMI_ENABLE_TEST_MODE                           0x03
#define BM83_MMI_ACCEPT_CALL                                0x04
#define BM83_MMI_REJECT_CALL                                0x05
#define BM83_MMI_ENDCALL_OR_TRANSFER_TO_HEADSET             0x06
#define BM83_MMI_MIC_MUTE_TOGGLE                            0x07
#define BM83_MMI_MUTE_MIC                                   0x08
#define BM83_MMI_UNMUTE_MIC                                 0x09
#define BM83_MMI_VOICE_DIAL                                 0x0A
#define BM83_MMI_CANCEL_VOICE_DIAL                          0x0B
#define BM83_MMI_LAST_NUMBER_REDIAL                         0x0C
#define BM83_MMI_ACTIVE_CALL_HOLD_ACCEPT_HELD_CALL          0x0D
#define BM83_MMI_VOICE_TRANSFER                             0x0E
#define BM83_MMI_QUERY_CALL_LIST                            0x0F
#define BM83_MMI_THREE_WAY_CALL                             0x10
#define BM83_MMI_RELEASE_CALL                               0x11
#define BM83_MMI_ACCEPT_WAITING_HOLD_CALL_RLS_ACTIVE_CALL   0x12
#define BM83_MMI_DISCONNECT_HF_LINK                         0x17
#define BM83_MMI_ENABLE_AEC                                 0x1E
#define BM83_MMI_DISABLE_AEC                                0x1F
#define BM83_MMI_SWITCH_AEC                                 0x20
#define BM83_MMI_ENABLE_AEC_RX_NOISE_REDUCTION              0x21
#define BM83_MMI_DISABLE_AEC_RX_NOISE_REDUCTION             0x22
#define BM83_MMI_SWITCH_AEC_RX_NOISE_REDUCTION              0x23
#define BM83_MMI_INC_MIC_GAIN                               0x24
#define BM83_MMI_DEC_MIC_GAIN                               0x25
#define BM83_MMI_SWITCH_PRIMARY_SECONDARY_HF_DEVICE         0x26
#define BM83_MMI_LOCK_BUTTON                                0x3A
#define BM83_MMI_DISCONNECT_A2DP                            0x3B
#define BM83_MMI_NEXT_AUDIO_EFFECT                          0x3C
#define BM83_MMI_PREVIOUS_AUDIO_EFFECT                      0x3D
#define BM83_MMI_REPORT_CURRENT_EQ_MODE                     0x3F
#define BM83_MMI_REPORT_CURRENT_AUDIO_EFFECT_STATUS         0x40
#define BM83_MMI_TOGGLE_AUDIO_PLAYBACK                      0x41
#define BM83_MMI_POWER_ON_BUTTON_PRESS                      0x51
#define BM83_MMI_POWER_ON_BUTTON_RELEASE                    0x52
#define BM83_MMI_POWER_OFF_BUTTON_PRESS                     0x53
#define BM83_MMI_POWER_OFF_BUTTON_RELEASE                   0x54
#define BM83_MMI_RESET_EEPROM_SETTING                       0x56
#define BM83_MMI_FORCE_SPEAKER_GAIN_TOGGLE                  0x57
#define BM83_MMI_TOGGLE_BUTTON_IND                          0x58
#define BM83_MMI_COMBINE_FUNCTION_0                         0x59
#define BM83_MMI_COMBINE_FUNCTION_1                         0x5A
#define BM83_MMI_COMBINE_FUNCTION_2                         0x5B
#define BM83_MMI_COMBINE_FUNCTION_3                         0x5C
#define BM83_MMI_ENTER_PAIRING_MODE                         0x5D
#define BM83_MMI_SWITCH_POWER_OFF                           0x5E
#define BM83_MMI_DISABLE_LED                                0x5F
#define BM83_MMI_BUZZER_TOGGLE                              0x60
#define BM83_MMI_DISABLE_BUZZER                             0x61
#define BM83_MMI_ENABLE_BUZZER                              0x62
#define BM83_MMI_TONE_CHANGE                                0x63
#define BM83_MMI_SHS_SEND_USER_DATA_1                       0x72
#define BM83_MMI_SHS_SEND_USER_DATA_2                       0x73
#define BM83_MMI_SHS_SEND_USER_DATA_3                       0x74
#define BM83_MMI_SHS_SEND_USER_DATA_4                       0x75
#define BM83_MMI_SHS_SEND_USER_DATA_5                       0x76
#define BM83_MMI_REPORT_CURRENT_RX_NR_STATUS                0x77
#define BM83_MMI_REPORT_CURRENT_TX_NR_STATUS                0x78
#define BM83_MMI_CANCEL_ALL_BT_PAGING                       0x7A
#define BM83_MMI_DISCONNECT_SPP_LINK                        0x7D
#define BM83_MMI_ENABLE_A2DP_MIX_LINE_IN                    0x80
#define BM83_MMI_DISABLE_A2DP_MIX_LINE_IN                   0x81
#define BM83_MMI_INCREASE_LINE_IN_INPUT_GAIN                0x82
#define BM83_MMI_DECREASE_LINE_IN_INPUT_GAIN                0x83
#define BM83_MMI_TRIGGER_MSPK_CENTRAL                       0xE0
#define BM83_MMI_TRIGGER_MSPK_PERIPHERAL                    0xE1
#define BM83_MMI_MSPK_ONE_KEY_CONNECT                       0xE2
#define BM83_MMI_CANCEL_MSPK_CREATION                       0xE3
#define BM83_MMI_TERMINATE_MSPK_LINK                        0xE4
#define BM83_MMI_TERMINATE_MSPK_CONNECTION                  0xE5
#define BM83_MMI_MSPK_CENTRAL_ENTER_AUX_IN_44p1K_PCM_MODE   0xE6
#define BM83_MMI_MSPK_CENTRAL_ENTER_AUX_IN_48K_PCM_MODE     0xE7
#define BM83_MMI_MSPK_CENTRAL_EXIT_AUX_IN_PCM_MODE          0xE8
#define BM83_MMI_MSPK_SWITCH_CHANNEL                        0xEC
#define BM83_MMI_MSPK_POWER_OFF_ALL_SPEAKERS                0xED
#define BM83_MMI_MSPK_ERASE_CONCERT_MODE_RECORD             0xEF
#define BM83_MMI_MSPK_ENTER_STEREO_MODE                     0xF4
#define BM83_MMI_MSPK_ENTER_CONCERT_MODE                    0xF5
#define BM83_MMI_MSPK_ADD_THIRD_SPK                         0xF6
#define BM83_MMI_MSPK_SOUND_SYNC                            0xF7
#define BM83_MMI_MSPK_CSB_CONNECTED_MODE_SWITCH             0xF8
#define BM83_MMI_MSPK_BACK_TO_LAST_MODE                     0xF9
#define BM83_MMI_MSPK_ONE_KEY_CONNECT_WITH_LINK_BACK        0xFA
#define BM83_MMI_MSPK_ONE_KEY_OP_STEREO_MODE                0xFB
#define BM83_MMI_MSPK_ONE_KEY_OP_CONCERT_MODE               0xFC

/**
 * BM83 command ACK status bytes.
 * Specified command ACK status bytes of BM83 .
 */
#define BM83_ACK_STATUS_CMD_COMPLETE                        0x00
#define BM83_ACK_STATUS_CMD_DISALLOW                        0x01
#define BM83_ACK_STATUS_CMD_UNKNOWN                         0x02
#define BM83_ACK_STATUS_CMD_PARAM_ERR                       0x03
#define BM83_ACK_STATUS_CMD_BTM_BUSY                        0x04
#define BM83_ACK_STATUS_CMD_BTM_FULL                        0x05

/**
 * BM83 BTM status bytes.
 * Specified BTM status bytes of BM83 .
 */
#define BM83_BTM_STATE_POWER_OFF                            0x00
#define BM83_BTM_STATE_PAIRING_STATE                        0x01
#define BM83_BTM_STATE_POWER_ON                             0x02
#define BM83_BTM_STATE_PAIRING_SUCCESSFULL                  0x03
#define BM83_BTM_STATE_PAIRING_FAILED                       0x04
#define BM83_BTM_STATE_HFHS_LINK_ESTABLISHED                0x05
#define BM83_BTM_STATE_A2DP_LINK_ESTABLISHED                0x06
#define BM83_BTM_STATE_HF_LINK_DISCONNECTED                 0x07
#define BM83_BTM_STATE_A2DP_LINK_DISCONNECTED               0x08
#define BM83_BTM_STATE_SCO_LINK_CONNECTED                   0x09
#define BM83_BTM_STATE_SCO_LINK_DISCONNECTED                0x0A
#define BM83_BTM_STATE_AVRCP_LINK_ESTABLISHED               0x0B
#define BM83_BTM_STATE_AVRCP_LINK_DISCONNECTED              0x0C
#define BM83_BTM_STATE_STANDARD_SPP_CONNECTED               0x0D
#define BM83_BTM_STATE_STANDARD_SPP_IAP_DISCONNECTED        0x0E
#define BM83_BTM_STATE_STANDBY_STATE                        0x0F
#define BM83_BTM_STATE_IAP_CONNECTED                        0x10
#define BM83_BTM_STATE_ACL_DISCONNECTED                     0x11
#define BM83_BTM_STATE_MAP_CONNECTED                        0x12
#define BM83_BTM_STATE_MAP_OPERATION_FORBIDDEN              0x13
#define BM83_BTM_STATE_MAP_DISCONNECTED                     0x14
#define BM83_BTM_STATE_ACL_CONNECTED                        0x15
#define BM83_BTM_STATE_SPP_IAP_DISCONNECTED_NO_PROFILE      0x16
#define BM83_BTM_STATE_LINK_BACK_ACL                        0x17
#define BM83_BTM_STATE_INQUIRY_STATE                        0x18
#define BM83_BTM_STATE_UNKNOWN_AUDIO_SOURCE                 0x80
#define BM83_BTM_STATE_AUX_IN_AUDIO_SOURCE                  0x81
#define BM83_BTM_STATE_A2DP_AUDIO_SOURCE                    0x82

/**
 * BM83 Volume control mask bytes.
 * Specified Volume control mask bytes of BM83 .
 */
#define BM83_VOLUME_MASK_A2DP                               0x01
#define BM83_VOLUME_MASK_HF                                 0x02
#define BM83_VOLUME_MASK_LINE_IN                            0x04
#define BM83_VOLUME_MASK_PCM                                0x08
#define BM83_VOLUME_MASK_LINE_IN_INPUT                      0x10  
/**
 * BM83 header byte.
 * Specified header byte of BM83 .
 */
#define BM83_START_HEADER                                   0xAA

/**
 * BM83 driver buffer size.
 * Specified size of driver ring buffer.
 * Increase buffer size if needed.
 */
#define DRV_BUFFER_SIZE                                         64

// SPP Commands list
#define COMMAND_PLAY                "play"          // play music
#define COMMAND_PAUSE               "pause"         // pause music
#define COMMAND_STOP                "stop"          // stop music
#define COMMAND_NEXT                "next"          // next song
#define COMMAND_PREVIOUS            "prev"          // previous song
#define COMMAND_VOLUME_UP           "up"            // volume up
#define COMMAND_VOLUME_DOWN         "down"          // volume down
#define COMMAND_UNMUTE              "unmute"        // unmute output
#define COMMAND_MUTE                "mute"          // mute output
#define COMMAND_MODE_UP             "mode"          // switch equalizer mode
#define COMMAND_CALL                "call"          // call command followed by the nuber
#define COMMAND_VOICE               "voice"         // start voice recognition app

/**
 * BM83  command/event packet structure object.
 *  Command/event packet structure object definition of BM83 .
 */
typedef struct
{
    unsigned char opcode;
    unsigned char param_buf[ 256 ];
    unsigned char param_len;
    
} Bm83_packet_t;

/**
 * BM83  context object.
 *  Context object definition of BM83 .
 */
typedef struct
{
    
    Bm83_packet_t command_packet;//258
    Bm83_packet_t event_packet;//258

} Bm83_t;


/**
 * BM83  return value data.
 *  Predefined enum values for driver return values.
 */
typedef enum
{
    BTAUDIO3_ERROR = 0,
    BTAUDIO3_OK = 1

} Bm83_return_value_t;

typedef Bm83_return_value_t err_t;
/**
 * BM83  music control values.
 *  Predefined enum values for music control.
 */
typedef enum
{
    BM83_STOP_FAST_FORWARD_OR_REWIND = 0,
    BM83_FAST_FORWARD,
    BM83_FAST_FORWARD_WITH_REPEAT,
    BM83_REWIND,
    BM83_REWIND_WITH_REPEAT,
    BM83_PLAY,
    BM83_PAUSE,
    BM83_PLAY_PAUSE_TOGGLE,
    BM83_STOP,
    BM83_NEXT_SONG,
    BM83_PREVIOUS_SONG

} Bm83_music_control_t;

/**
 * BM83  equalizer mode selection values.
 *  Predefined enum values for equalizer mode selection.
 */
typedef enum
{
    BTAUDIO2_EQ_MODE_OFF = 0,
    BTAUDIO2_EQ_MODE_SOFT,
    BTAUDIO2_EQ_MODE_BASS,
    BTAUDIO2_EQ_MODE_TREBLE,
    BTAUDIO2_EQ_MODE_CLASSIC,
    BTAUDIO2_EQ_MODE_ROCK,
    BTAUDIO2_EQ_MODE_JAZZ,
    BTAUDIO2_EQ_MODE_POP,
    BTAUDIO2_EQ_MODE_DANCE,
    BTAUDIO2_EQ_MODE_RNB,
    BTAUDIO2_EQ_MODE_USER1

} Bm83_eq_mode_t;

typedef enum
{
   SYNC_BYTE_RX_STATE=0,
   LEN_STATE_RX_STATE1,
   LEN_STATE_RX_STATE2,
   OPCODE_RX_STATE,
   DATA_RX_STATE,
   CHECKSUM_RX_STATE
}bt83_rx_t;

typedef enum
{
   BT_INIT=0,
   BT_RUN,
   BT_PAIR
}Bt_State_T;


extern unsigned short int uart_pkt_timeout;
extern Bm83_t Bm83;
extern unsigned char rx_complete;



void Bm83_Uart_Timeout_Check(void);
void Bm83_Assemble_Uart_Data(unsigned char data);
void Bm83_Init(void);
void Bm83_Task(void);
unsigned char Bm83_Event_Handler(Bm83_t *ctx );
void Bm83_Default_Cfg(Bm83_t *ctx );
void Bm83_Generic_Write(unsigned char *data_buf, unsigned short int len );
unsigned char Bm83_Generic_Read(Bm83_t *ctx, unsigned char *data_buf, unsigned short int max_len );
void Bm83_Set_MFB_Pin(unsigned char state );
void Bm83_Set_RST_Pin(unsigned char state );
void Bm83_Set_Test_Mode(Bm83_t *ctx );
void Bm83_Mute(void );
void Bm83_UnMute(void);
void Bm83_Send_Command(Bm83_t *ctx );

void Bm83_Clear_Events(Bm83_t *ctx );
void Bm83_Send_Mmi_Action(Bm83_t *ctx, unsigned char database_index, unsigned char action );
void Bm83_Power_On_Device(Bm83_t *ctx );
void Bm83_Power_Off_Device(Bm83_t *ctx );
void Bm83_Enter_Pairing_Mode(Bm83_t *ctx );

void Bm83_Set_Device_Name(Bm83_t *ctx, unsigned char *device_name );
void Bm83_Music_Control(Bm83_t *ctx, Bm83_music_control_t action );

void Bm83_Volume_Up(Bm83_t *ctx, unsigned char database_index, unsigned char mask );
void Bm83_Volume_Down(Bm83_t *ctx, unsigned char database_index, unsigned char mask );

void Bm83_Set_Eq_Mode( Bm83_t *ctx, Bm83_eq_mode_t eq_mode );

#endif // BM_H


