using System;
using System.Text;
using System.Runtime.InteropServices;

namespace FlashTool
{
    static class icsneo40
    {
        public const int NEODEVICE_BLUE = 1;
        public const int NEODEVICE_DW_VCAN = 4;
        public const int NEODEVICE_FIRE = 8;
        public const int NEODEVICE_VCAN3 = 16;
        public const int NEODEVICE_ANY = 65535;

        //// Configuration Array constants
        //// high speed CAN neoVI / valuecan baud rate constants
        public const short NEO_CFG_MPIC_HS_CAN_CNF1 = 512 + 10;
        public const short NEO_CFG_MPIC_HS_CAN_CNF2 = 512 + 9;
        public const short NEO_CFG_MPIC_HS_CAN_CNF3 = 512 + 8;
        public const short NEO_CFG_MPIC_HS_CAN_MODE = 512 + 54;

        //// med speed CAN
        public const short NEO_CFG_MPIC_MS_CAN_CNF1 = 512 + 22;
        public const short NEO_CFG_MPIC_MS_CAN_CNF2 = 512 + 21;
        public const short NEO_CFG_MPIC_MS_CAN_CNF3 = 512 + 20;

        public const short NEO_CFG_MPIC_SW_CAN_CNF1 = 512 + 34;
        public const short NEO_CFG_MPIC_SW_CAN_CNF2 = 512 + 33;
        public const short NEO_CFG_MPIC_SW_CAN_CNF3 = 512 + 32;

        public const short NEO_CFG_MPIC_LSFT_CAN_CNF1 = 512 + 46;
        public const short NEO_CFG_MPIC_LSFT_CAN_CNF2 = 512 + 45;
        public const short NEO_CFG_MPIC_LSFT_CAN_CNF3 = 512 + 44;

        //// Protocols
        public const short SPY_PROTOCOL_CUSTOM = 0;
        public const short SPY_PROTOCOL_CAN = 1;
        public const short SPY_PROTOCOL_GMLAN = 2;
        public const short SPY_PROTOCOL_J1850VPW = 3;
        public const short SPY_PROTOCOL_J1850PWM = 4;
        public const short SPY_PROTOCOL_ISO9141 = 5;
        public const short SPY_PROTOCOL_Keyword2000 = 6;
        public const short SPY_PROTOCOL_GM_ALDL_UART = 7;
        public const short SPY_PROTOCOL_CHRYSLER_CCD = 8;
        public const short SPY_PROTOCOL_CHRYSLER_SCI = 9;
        public const short SPY_PROTOCOL_FORD_UBP = 10;
        public const short SPY_PROTOCOL_BEAN = 11;
        public const short SPY_PROTOCOL_LIN = 12;

        ////device ID's
        public const short NETID_DEVICE = 0;
        public const short NETID_HSCAN = 1;
        public const short NETID_MSCAN = 2;
        public const short NETID_SWCAN = 3;
        public const short NETID_LSFTCAN = 4;
        public const short NETID_FORDSCP = 5;
        public const short NETID_J1708 = 6;
        public const short NETID_AUX = 7;
        public const short NETID_JVPW = 8;
        public const short NETID_ISO = 9;
        public const short NETID_LIN = 9;
        public const short NETID_ISOPIC = 10;
        public const short NETID_MAIN51 = 11;
        public const short NETID_SCI = 13;
        public const short NETID_ISO2 = 14;
        public const short NETID_FIRE_HSCAN1 = 41;
        public const short NETID_FIRE_HSCAN2 = 42;
        public const short NETID_FIRE_MSCAN1 = 43;
        public const short NETID_FIRE_MSCAN2 = 44;
        public const short NETID_FIRE_HSCAN3 = 44;
        public const short NETID_FIRE_SWCAN = 45;
        public const short NETID_FIRE_LSFT = 46;

        //Public Const NETID_FIRE_LIN1 = 47
        // Changed from 47 to 16 per FIRE-LIN application example received from Intrepid 2010.12.01
        public const short NETID_FIRE_LIN1 = 16;

        public const short NETID_FIRE_LIN2 = 48;
        public const short NETID_FIRE_LIN3 = 49;
        public const short NETID_FIRE_LIN4 = 50;

        public enum icsSpyDataStatusBitfield
        {
            icsSpyStatusGlobalError = 1,
            icsSpyStatusTx = 2,
            icsSpyStatusXtdFrame = 4,
            icsSpyStatusRemoteFrame = 8,
            icsSpyStatusErrCRCError = 16,
            icsSpyStatusCANErrorPassive = 32,
            icsSpyStatusErrIncompleteFrame = 64,
            icsSpyStatusErrLostArbitration = 128,
            icsSpyStatusErrUndefined = 256,
            icsSpyStatusErrCANBusOff = 512,
            icsSpyStatusErrCANErrorWarning = 1024,
            icsSpyStatusBusShortedPlus = 2048,
            icsSpyStatusBusShortedGnd = 4096,
            icsSpyStatusCheckSumError = 8192,
            icsSpyStatusErrBadMessageBitTimeError = 16384,
            icsSpyStatusIFRData = 32768,
            icsSpyStatusHardwareCommError = 65536,
            icsSpyStatusExpectedLengthError = 131072,
            icsSpyStatusIncomingNoMatch = 262144,
            icsSpyStatusBreak = 524288,
            icsSpyStatusAVT_VSIRecOverflow = 1048576,
            icsSpyStatusTestTrigger = 2097152,
            icsSpyStatusAudioCommentType = 4194304,
            icsSpyStatusGPSDataValue = 8388608,
            icsSpyStatusAnalogDigitalInputValue = 16777216,
            icsSpyStatusTextCommentType = 33554432,
            icsSpyStatusNetworkMessageType = 67108864,
            icsSpyStatusVSI_TxUnderRun = 134217728,
            icsSpyStatusVSI_IFR_CRCBit = 268435456,
            icsSpyStatusInitMessage = 536870912,
            icsSpyStatusHighSpeed = 1073741824
        }
        public enum InterpidErrMsg
        {
            NEOVI_ERROR_DLL_TX_BUFFER_OVERFLOW = 0,
            NEOVI_ERROR_DLL_ERROR_BUFFER_OVERFLOW = 1,
            NEOVI_ERROR_DLL_USB_SEND_DATA_ERROR = 2,
            NEOVI_ERROR_DLL_ISO_DATA_BUFFER_ALLOC = 3,
            NEOVI_ERROR_DLL_ISO_DATA_READ_BUFFER = 4,
            NEOVI_ERROR_DLL_ISO_DATA_ZERO_PACKETS = 5,
            NEOVI_ERROR_DLL_RX_MSG_BUFFER_OVERFLOW = 6,
            NEOVI_ERROR_DLL_STOP_ISO_STREAM = 7,
            NEOVI_ERROR_DLL_INVALID_NETID = 8,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_RX_THREAD = 9,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_TX_THREAD = 10,
            NEOVI_ERROR_DLL_MAIN_PIC_BUFFER_OVERFLOW = 11,
            NEOVI_ERROR_DLL_INVALID_DEVICE_RESPONSE = 12,
            NEOVI_ERROR_DLL_ISOTX_DATA_BUFFER_ALLOC = 13,
            NEOVI_ERROR_DLL_RX_CMD_BUFFER_OVERFLOW = 14,
            NEOVI_ERROR_DLL_RS232_RX_BUFFER_OVERFLOW = 15,
            NEOVI_ERROR_DLL_RS232_ERR_READCOMERR = 16,
            NEOVI_ERROR_DLL_RS232_ERR_READ = 17,
            NEOVI_ERROR_DLL_RS232_BUFFER_ALLOC = 18,
            NEOVI_ERROR_DLL_RS232_TX_BUFFER_OVERFLOW = 19,
            NEOVI_ERROR_DLL_RS232_MISC_ERROR = 20,
            NEOVI_ERROR_DLL_RS232_FIND_WRITE = 21,
            NEOVI_ERROR_DLL_RS232_FIND_BUFFER_ALLOC = 22,
            NEOVI_ERROR_DLL_RS232_FIND_CLEARCOMM = 23,
            NEOVI_ERROR_DLL_RS232_FIND_READCOMM = 24,
            NEOVI_ERROR_DLL_RS232_FIND_TIMEOUT = 25,
            NEOVI_ERROR_DLL_RS232_ERR_BREAK = 26,
            NEOVI_ERROR_DLL_RS232_ERR_FRAME = 27,
            NEOVI_ERROR_DLL_RS232_ERR_IOE = 28,
            NEOVI_ERROR_DLL_RS232_ERR_OVERRUN = 29,
            NEOVI_ERROR_DLL_RS232_ERR_PARITY = 30,
            NEOVI_ERROR_DLL_RS232_TXBUFFER_ALLOC = 31,
            NEOVI_ERROR_DLL_USB_TX_RS232_ERROR = 32,
            NEOVI_ERROR_DLL_RS232_CREATE_FILE = 33,
            NEOVI_ERROR_DLL_RS232_GET_COMM_STATE = 34,
            NEOVI_ERROR_DLL_RS232_SET_COMM_STATE = 35,
            NEOVI_ERROR_DLL_RS232_START_COMM_RX_THREAD = 36,
            NEOVI_ERROR_DLL_RS232_START_COMM_TX_THREAD = 37,
            NEOVI_ERROR_DLL_SYNC_COUNT_ERR = 38,
            NEOVI_ERROR_DLL_RX_MSG_FRAME_ERR = 39,
            NEOVI_ERROR_DLL_RX_MSG_FIFO_OVER = 40,
            NEOVI_ERROR_DLL_RX_MSG_CHK_SUM_ERR = 41,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_BULKIN_THREAD = 42,
            NEOVI_ERROR_DLL_BULKIN_ERR_READ = 43,
            NEOVI_ERROR_DLL_MAIN51_RX_FIFO_OVERFLOW = 44,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW = 45,
            NEOVI_ERROR_DLL_MAIN51_DEV_FIFO_OVERFLOW = 46,
            NEOVI_ERROR_DLL_RESET_STATUS_CHANGED = 47,
            NEOVI_ERROR_DLL_ISO_LONG_CACHE_OVERFLOW = 48,
            NEOVI_ERROR_DLL_ISORX_LONG_BUFFER_ALLOC = 49,
            NEOVI_ERROR_DLL_J1708_LONG_CACHE_OVERFLOW = 50,
            NEOVI_ERROR_DLL_J1708_LONG_BUFFER_ALLOC = 51,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_DEVICE = 52,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_HSCAN = 53,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_MSCAN = 54,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_SWCAN = 55,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_LSFTCAN = 56,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_FORDSCP = 57,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_J1708 = 58,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_AUX = 5,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_JVPW = 60,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_ISO = 61,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_ISOPIC = 62,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_MAIN51 = 63,
            NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_HOST = 64,
            NEOVI_ERROR_DLL_READ_ENTIRE_DEEPROM_ERROR = 65,
            NEOVI_ERROR_DLL_WRITE_ENTIRE_DEEPROM_ERROR = 66,
            NEOVI_ERROR_DLL_USB_PORT_ALREADY_OPEN = 67,
            NEOVI_ERROR_DLL_JVPW_TX_REPORT_FIFO_ERR_IN = 68,
            NEOVI_ERROR_DLL_ISOJ_TX_REPORT_FIFO_ERR_IN = 69,
            NEOVI_ERROR_DLL_JVPW_TX_REPORT_FIFO_ERR_OUT = 70,
            NEOVI_ERROR_DLL_ISOJ_TX_REPORT_FIFO_ERR_OUT = 71,
            NEOVI_ERROR_DLL_MAIN51_TX_IN_FROM_HOST_FIFO = 72,
            NEOVI_ERROR_DLL_MAIN51_TX_HOST_CHKSUM = 73,
            NEOVI_ERROR_DLL_ISOJ_TX_HOST_MISSED_BYTE = 74,
            NEOVI_ERROR_DLL_NEOVI_NO_RESPONSE = 75,
            NEOVI_ERROR_DLL_RX_SOCKET_FIFO_OVER = 76,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_TXSOCKET_THREAD = 77,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_RXSOCKET_THREAD = 78,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_TXSOCKET_CLIENT_THREAD = 78,
            NEOVI_ERROR_DLL_PROBLEM_STOPPING_RXSOCKET_CLIENT_THREAD = 79,
            NEOVI_ERROR_DLL_TCP_CLIENT_TX = 80,
            NEOVI_ERROR_DLL_TCP_CLIENT_RX = 81,
            NEOVI_ERROR_DLL_TCP_CLIENT_RX_SOCK = 82,
            NEOVI_ERROR_DLL_UNABLE_CONNECT_TO_SRVR = 83,
            NEOVI_ERROR_DLL_UNABLE_CREATE_CLIENT_SOCK = 84,
            NEOVI_ERROR_DLL_UNABLE_WSASTARTUP = 85,
            NEOVI_ERROR_DLL_SOCK_CL_RD_BUFFER_ALLOC = 86,
            NEOVI_ERROR_DLL_SOCK_CL_TX_BUFFER_ALLOC = 87,
            NEOVI_ERROR_DLL_SOCK_SRVR_RX_BUFFER_ALLOC = 88,
            NEOVI_ERROR_DLL_SOCK_SRVR_TX_BUFFER_ALLOC = 89,
            NEOVI_ERROR_DLL_ILLEGAL_TX_NETWORK = 90,
            NEOVI_ERROR_DLL_MAIN51_TX_HOST_OVERRUN = 91,
            NEOVI_ERROR_DLL_OPEN_GET_COMM_TIMEOUT = 92,
            NEOVI_ERROR_DLL_OPEN_SET_COMM_TIMEOUT = 93,
            NEOVI_ERROR_DLL_OPEN_READ_DEVICE_TYPE = 94,
            NEOVI_ERROR_DLL_OPEN_READ_DEVICE_TYPE_TOUT = 95,
            NEOVI_ERROR_DLL_CLOSE_PURGE_COMM = 96,
            NEOVI_ERROR_DLL_TX_COM_FIFO_OVERFLOW = 97,
            NEOVI_ERROR_DLL_GET_USBSERIAL_DEVICES = 98,
            NEOVI_ERROR_DLL_USB_TX_RS232_BCOUNT = 99,
            NEOVI_ERROR_DLL_OPEN_INBOOTLOADER = 100,
            NEOVI_ERROR_DLL_TOO_MANY_PERIODICS = 101,
            NEOVI_ERROR_DLL_PROBLEM_FIRMWARE_INFO = 102,
            NEOVI_ERROR_DLL_NRED_ODDNUMBYTES = 103,
            NEOVI_ERROR_DLL_NRED_UNKNOWN_RED_NETID = 104,
            NEOVI_ERROR_DLL_RED_NOT_SUPPORTED = 105,
            NEOVI_ERROR_DLL_RED_BL_START_INDEX = 106,
            NEOVI_ERROR_DLL_3G_BL_FAILURE = 107,
            NEOVI_ERROR_DLL_RED_BL_END_INDEX = 116,
            NEOVI_ERROR_DLL_RED_FAILED_TO_ENTER_BL = 117,
            NEOVI_ERROR_DLL_RED_REQ_SERIAL_NUMBER = 118,
            NEOVI_ERROR_DLL_RED_AUTHENTICATE = 119,
            NEOVI_ERROR_DLL_RED_APP_VERSION = 120,
            NEOVI_ERROR_DLL_RED_SET_BAUD_COMM_FAILURE = 121,
            NEOVI_ERROR_DLL_RED_INVALID_BAUD_SPECIFIED = 122,
            NEOVI_ERROR_DLL_RED_READ_BAUD_COMM_FAILURE = 123,
            NEOVI_ERROR_DLL_RED_FAILED_TO_SAVE_EEPROM = 124,
            NEOVI_ERROR_DLL_RED_FAILED_TO_UPDATE_WAVEFORM_CHANNEL = 125,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_UNKNOWN_NETWORK = 126,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_UNKNOWN_NETWORK = 127,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_UNKNOWN_NETWORK = 128,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_HSCAN1 = 129,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_HSCAN1 = 130,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_HSCAN1 = 131,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_HSCAN1 = 132,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_HSCAN2 = 133,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_HSCAN2 = 134,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_HSCAN2 = 135,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_HSCAN2 = 136,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_MSCAN1 = 137,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_MSCAN1 = 138,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_MSCAN1 = 139,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_MSCAN1 = 140,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_HSCAN3 = 141,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_HSCAN3 = 142,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_HSCAN3 = 143,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_HSCAN3 = 144,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_SWCAN = 145,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_SWCAN = 146,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_SWCAN = 147,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_SWCAN = 148,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LSFTCAN = 149,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LSFTCAN = 150,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LSFTCAN = 151,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LSFTCAN = 152,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN1 = 153,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN1 = 154,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN1 = 155,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN1 = 156,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN2 = 157,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN2 = 158,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN2 = 159,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN2 = 160,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN3 = 161,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN3 = 162,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN3 = 163,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN3 = 164,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN4 = 165,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN4 = 166,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN4 = 167,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN4 = 168,
            NEOVI_ERROR_DLL_USB_PURGE_FAILED = 169,
            NEOVI_ERROR_FIRE_COMM_BAD_PACKET = 170,
            NEOVI_ERROR_FIRE_CGI_COMM_BAD_PACKET = 171,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_HSCAN1 = 172,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_HSCAN2 = 173,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_HSCAN3 = 174,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_MSCAN = 175,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_SWCAN = 176,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LSFTCAN = 177,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN1 = 178,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN2 = 179,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN3 = 180,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN4 = 181,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_UNKNOWN_NETWORK = 182,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_JVPW = 183,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_JVPW = 184,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_JVPW = 185,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_JVPW = 186,
            NEOVI_ERROR_DLL_INTERNAL_SERIAL_NO_DOES_NOT_MATCH_REGISTRY_SERIAL_NO = 187,
            NEOVI_ERROR_DLL_JVPW_LONG_CACHE_OVERFLOW = 188,
            NEOVI_ERROR_DLL_FAILED_TO_SET_LICENSE = 189,
            NEOVI_ERROR_DLL_3G_DEVICE_LICENSE_NEEDS_TO_BE_UPGRADED = 190,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_HSCAN = 191,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_MSCAN = 192,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_HSCAN2 = 193,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_HSCAN3 = 194,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LSFT = 195,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_SW = 196,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN1 = 197,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN2 = 198,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN3 = 199,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN4 = 200,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_JVPW = 201,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_KYW = 202,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_J1708 = 203,
            NEOVI_ERROR_DLL_MAIN51_RTC_INVALID = 204,
            NEOVI_ERROR_DLL_MAIN51_LOADED_DEFAULT_SETTINGS = 205,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_CGI = 206,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_CGI = 207,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_CGI = 208,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_CGI = 209,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_CGI = 210,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_CGI = 211,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_JVPW = 212,
            NEOVI_ERROR_DLL_INVALID_SCRIPT_LOCATION = 213,
            NEOVI_ERROR_DLL_SDCARD_NOT_INSERTED = 214,
            NEOVI_ERROR_DLL_SDCARD_NOT_FORMATTED = 215,
            NEOVI_ERROR_DLL_SDCARD_WRITE_ERROR = 216,
            NEOVI_ERROR_DLL_SDCARD_READ_ERROR = 217,
            NEOVI_ERROR_DLL_SCRIPT_START_ERROR = 218,
            NEOVI_ERROR_DLL_SCRIPT_INVALID_FUNCBLOCK_INDEX = 219,
            NEOVI_ERROR_DLL_SCRIPT_ERROR_DOWNLOADING_SCRIPT = 220,
            NEOVI_ERROR_DLL_SCRIPT_ERROR_CLEARING_SCRIPT = 221,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO = 222,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO = 223,
            NEOVI_ERROR_DLL_SCRIPT_INVALID_MSG_INDEX = 224,
            NEOVI_ERROR_DLL_SCRIPT_INVALID_APPSIG_INDEX = 225,
            NEOVI_ERROR_DLL_SCRIPT_NO_SCRIPT_RUNNING = 226,
            NEOVI_ERROR_DLL_COULD_NOT_SET_SETTINGS_FIRMWARE_MISMATCH = 227,
            NEOVI_ERROR_DLL_FIRE_CGI_TX_NOT_ENABLED = 228,
            NEOVI_ERROR_DLL_SEND_DEVICE_CONFIG_ERROR = 229,
            NEOVI_ERROR_DLL_GET_DEVICE_CONFIG_ERROR = 230,
            NEOVI_ERROR_DLL_UNKNOWN_NEOVI_TYPE = 231,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO2 = 232,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO3 = 233,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO4 = 234,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO2 = 235,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO2 = 236,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO2 = 237,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_ISO2 = 238,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO2 = 239,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO3 = 240,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO3 = 241,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO3 = 242,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_ISO3 = 243,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO3 = 244,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO4 = 245,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO4 = 246,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO4 = 247,
            NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_ISO4 = 248,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO4 = 249,
            NEOVI_ERROR_DLL_RED_FAILED_TO_CLEAR_LIN_SLAVE_DATA = 250,
            NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO1 = 251,
            NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO1 = 252,
            NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO1 = 253,
            NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO1 = 254,
            NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO1 = 255,
            NEOVI_ERROR_DLL_DROPPED_RTC_CMD = 256,
            NEOVI_ERROR_DLL_J1850_TX_REQUESTS_FLUSHED = 257,
            NEOVI_ERROR_J1708_COMM_BAD_PACKET = 258,
            NEOVI_ERROR_DLL_NETWORK_NOT_SUPPORTED_BY_HARDWARE = 259,
            NEOVI_ERROR_DLL_FEATURE_NOT_UNLOCKED = 260,
            NEOVI_ERROR_DLL_DEVICE_NOT_POWERED = 261,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_OK = 262,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORBADINIT = 263,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORNOCANPIPE = 264,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORISONEG7FRESPONSE = 265,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORTOOLNOTSELECTED = 266,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORINVALIDDEVICESELECTED = 267,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORCOULDNOTOPENTOOL = 268,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORNOFLOWCONTROL = 269,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORUNSPECIFIC = 270,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORCOREMININULLPTR = 271,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORCOREMINIZEROLEN = 272,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORRESERVED3 = 273,
            NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORRESERVED4 = 274,
            NEOVI_ERROR_DLL_YELLOW_DEPRECATED = 275,
            NEOVI_ERROR_DLL_HARDWARE_FAILURE_SRAM = 276,
            NEOVI_ERROR_ACTIVE_CONNECTION_ALREADY_EXISTS = 277,
            NEOVI_ERROR_DLL_MAIN51_RTC_FAILED_READ = 278,
            NEOVI_ERROR_DLL_MAIN51_RTC_AUTO_RTC = 279
        }

        public enum icsSpyDataStatusBitfield2
        {
            icsSpyStatusHasValue = 1,
            icsSpyStatusValueIsBoolean = 2,
            icsSpyStatusHighVoltage = 4,
            icsSpyStatusLongMessage = 8
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct icsSpyMessage   //reff
        {
            public Int32 StatusBitField; //4
            public Int32 StatusBitField2; //new '4
            public Int32 TimeHardware; // 4
            public Int32 TimeHardware2; //new ' 4
            public Int32 TimeSystem; // 4
            public Int32 TimeSystem2;
            public byte TimeStampHardwareID; //new ' 1
            public byte TimeStampSystemID;
            public byte NetworkID; //new ' 1
            public byte NodeID;
            public byte Protocol;
            public byte MessagePieceID; // 1
            public byte ColorID; //1
            public byte NumberBytesHeader; // 1
            public byte NumberBytesData; // 1
            public Int16 DescriptionID; // 2
            public Int32 ArbIDOrHeader; // Holds (up to 3 byte 1850 header or 29 bit CAN header) '4
            //public byte[] Data = new byte[8]; //(1 To 8); //8
            public byte Data1;
            public byte Data2;
            public byte Data3;
            public byte Data4;
            public byte Data5;
            public byte Data6;
            public byte Data7;
            public byte Data8;
            public byte AckBytes1;
            public byte AckBytes2;
            public byte AckBytes3;
            public byte AckBytes4;
            public byte AckBytes5;
            public byte AckBytes6;
            public byte AckBytes7;
            public byte AckBytes8;
            //public byte[] AckBytes = new byte[8]; //(1 To 8); //new '8
            public Single Value; // As Single ' 4
            public byte MiscData;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct icsSpyMessageLong
        {
            public Int32 StatusBitField; // 4
            public Int32 StatusBitField2; //new '4
            public Int32 TimeHardware;
            public Int32 TimeHardware2; //new ' 4
            public Int32 TimeSystem; //4
            public Int32 TimeSystem2;
            public byte TimeStampHardwareID; //new ' 1
            public byte TimeStampSystemID;
            public byte NetworkID; //new ' 1
            public byte NodeID;
            public byte Protocol;
            public byte MessagePieceID; // 1
            public byte ColorID; // 1
            public byte NumberBytesHeader; //
            public byte NumberBytesData; //2
            public Int16 DescriptionID; //2
            public Int32 ArbIDOrHeader;// Holds (up to 3 byte 1850 header or 29 bit CAN header)
            public Int32 DataMsb;
            public Int32 DataLsb;
            public byte AckBytes1;
            public byte AckBytes2;
            public byte AckBytes3;
            public byte AckBytes4;
            public byte AckBytes5;
            public byte AckBytes6;
            public byte AckBytes7;
            public byte AckBytes8;
            public Single Value; // As Single
            public byte MiscData;

        }

        [StructLayout(LayoutKind.Sequential)]
        public struct icsSpyMessageJ1850
        {
            public Int32 StatusBitField; //4
            public Int32 StatusBitField2; //new '4
            public Int32 TimeHardware; //4
            public Int32 TimeHardware2; //new ' 4
            public Int32 TimeSystem; //4
            public Int32 TimeSystem2;
            public byte TimeStampHardwareID; //new ' 1
            public byte TimeStampSystemID;
            public byte NetworkID; //new ' 1
            public byte NodeID;
            public byte Protocol;
            public byte MessagePieceID; // 1 new
            public byte ColorID; // 1
            public byte NumberBytesHeader; //1
            public byte NumberBytesData; //1
            public Int16 DescriptionID; //2
            public byte Header1;  //Holds (up to 3 byte 1850 header or 29 bit CAN header)
            public byte Header2;
            public byte Header3;
            public byte Header4;
            public byte Data1;
            public byte Data2;
            public byte Data3;
            public byte Data4;
            public byte Data5;
            public byte Data6;
            public byte Data7;
            public byte Data8;
            public byte AckBytes1;
            public byte AckBytes2;
            public byte AckBytes3;
            public byte AckBytes4;
            public byte AckBytes5;
            public byte AckBytes6;
            public byte AckBytes7;
            public byte AckBytes8;
            public Single Value; // As Single '4
            public byte MiscData;
        }

        //public struct spyFilterLong
        //{
        //    public int StatusValue;
        //    public int StatusMask;
        //    public int Status2Value;
        //    public int Status2Mask;
        //    public int Header;
        //    public int HeaderMask;
        //    public int MiscData;
        //    public int MiscDataMask;
        //    public int ByteDataMsb;
        //    public int ByteDataLsb;
        //    public int ByteDataMaskMsb;
        //    public int ByteDataMaskLsb;
        //    public int HeaderLength;
        //    public int ByteDataLength;
        //    public int NetworkID;
        //    public bool FrameMaster;
        //    public byte bStuff1;
        //    public byte bStuff2;
        //    public int ExpectedLength;
        //    public int NodeID;
        //}


        ////Structure for neoVI device types
        public struct NeoDevice
        {
            public int DeviceType;
            public int Handle;
            public int NumberOfClients;
            public int SerialNumber;
            public int MaxAllowedClients;
        }

        public struct CAN_SETTINGS
        {
            public byte Mode;
            public byte SetBaudrate;
            public byte Baudrate;
            public byte NetworkType;
            public byte TqSeg1;
            public byte TqSeg2;
            public byte TqProp;
            public byte TqSync;
            public short BRP;
            public short auto_baud;
        }

        public struct SWCAN_SETTINGS
        {
            public byte Mode;
            public byte SetBaudrate;
            public byte Baudrate;
            public byte NetworkType;
            public byte TqSeg1;
            public byte TqSeg2;
            public byte TqProp;
            public byte TqSync;
            public short BRP;
            public short high_speed_auto_switch;
            public short auto_baud;
        }

        public struct SVCAN3Settings
        {
        }

        public struct LIN_SETTINGS
        {
            public float Baudrate;
            public short spbrg;
            public short brgh;
            public byte MasterResistor;
            public byte Mode;
        }

        public struct ISO9141_KEYWORD2000__INIT_STEP
        {
            public short time_500us;
            public short k;
            public short l;
        }

        public struct ISO9141_KEYWORD2000_SETTINGS
        {
            public float Baudrate;
            public short spbrg;
            public short brgh;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_0;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_1;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_2;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_3;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_4;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_5;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_6;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_7;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_8;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_9;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_10;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_11;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_12;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_13;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_14;
            public ISO9141_KEYWORD2000__INIT_STEP Init_Step_15;
            public byte init_step_count;
            public short p2_500us;
            public short p3_500us;
            public short p4_500us;
            public short chksum_enabled;
        }

        public struct SFireSettings
        {
            public CAN_SETTINGS can1;
            public CAN_SETTINGS can2;
            public CAN_SETTINGS can3;
            public CAN_SETTINGS can4;
            public SWCAN_SETTINGS swcan;
            public CAN_SETTINGS lsftcan;
            public LIN_SETTINGS lin1;
            public LIN_SETTINGS lin2;
            public LIN_SETTINGS lin3;
            public LIN_SETTINGS lin4;
            public short cgi_enable;
            public short cgi_baud;
            public short cgi_tx_ifs_bit_times;
            public short cgi_rx_ifs_bit_times;
            public short cgi_chksum_enable;
            public short network_enables;
            public short network_enabled_on_boot;
            public short pwm_man_timeout;
            public short pwr_man_enable;
            public short misc_io_initial_ddr;
            public short misc_io_initial_latch;
            public short misc_io_analog_enable;
            public short misc_io_report_period;
            public short misc_io_on_report_events;
            public short ain_sample_period;
            public short ain_threshold;
            public short iso15765_separation_time_offset;
            public short iso9141_kwp_enable;
            public ISO9141_KEYWORD2000_SETTINGS iso9141_kwp_settings;
            public short perf_en;
            public short iso_parity;
            public short iso_msg_termination;
        }


        //// Function Declares /////////////////////////////////////////////////////////////////////////////


        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoFindNeoDevices(UInt32 DeviceTypes, ref NeoDevice pNeoDevice, ref Int32 pNumDevices);

        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoOpenNeoDevice(ref NeoDevice pNeoDevice, ref Int32 hObject, ref byte bNetworkIDs, Int32 bConfigRead, Int32 bSyncToPC);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoClosePort(Int32 hObject, ref Int32 pNumberOfErrors);
        [DllImport("icsneo40.dll")]
        public static extern void icsneoFreeObject(Int32 hObject);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoOpenPortEx(Int32 lPortNumber, Int32 lPortType, Int32 lDriverType, Int32 lIPAddressMSB, Int32 lIPAddressLSBOrBaudRate, Int32 bConfigRead, ref byte bNetworkID, ref Int32 hObject);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetMessages(Int32 hObject, ref icsSpyMessage pMsg, ref Int32 pNumberOfMessages, ref Int32 pNumberOfErrors);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoTxMessages(Int32 hObject, ref icsSpyMessage pMsg, Int32 lNetworkID, Int32 lNumMessages);

        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoWaitForRxMessagesWithTimeOut(Int32 hObject, UInt32 iTimeOut);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoEnableNetworkRXQueue(Int32 hObject, Int32 iEnable);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetTimeStampForMsg(Int32 hObject, ref icsSpyMessage pMsg, ref double pTimeStamp);
        [DllImport("icsneo40.dll")]
        public static extern void icsneoGetISO15765Status(Int32 hObject, Int32 lNetwork, Int32 lClearTxStatus, Int32 lClearRxStatus, ref Int32 lTxStatus, ref Int32 lRxStatus);
        //[DllImport("icsneo40.dll")]
        //public static extern void icsneoSetISO15765RxParameters(Int32 hObject, Int32 lNetwork, Int32 lEnable, ref spyFilterLong pFF_CFMsgFilter, ref icsSpyMessage pTxMsg, Int32 lCFTimeOutMs, Int32 lFlowCBlockSize,Int32 lUsesExtendedAddressing, Int32 lUseHardwareIfPresent);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetConfiguration(Int32 hObject, ref byte pData, ref Int32 lNumBytes);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoSendConfiguration(Int32 hObject, ref byte pData, Int32 lNumBytes);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetFireSettings(Int32 hObject, ref SFireSettings pSettings, Int32 iNumBytes);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoSetFireSettings(Int32 hObject, ref SFireSettings pSettings, Int32 iNumBytes, Int32 bSaveToEEPROM);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetVCAN3Settings(Int32 hObject, ref SVCAN3Settings pSettings, Int32 iNumBytes);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoSetVCAN3Settings(Int32 hObject, ref SVCAN3Settings pSettings, Int32 iNumBytes, Int32 bSaveToEEPROM);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoSetBitRate(Int32 hObject, Int32 BitRate, Int32 NetworkID);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetDeviceParameters(Int32 hObject, ref char pParameter, ref char pValues, Int16 ValuesLength);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoSetDeviceParameters(Int32 hObject, ref char pParmValue, ref Int32 pErrorIndex, Int32 bSaveToEEPROM);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetLastAPIError(Int32 hObject, ref UInt32 pErrorNumber);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetErrorMessages(Int32 hObject, ref Int32 pErrorMsgs, ref Int32 pNumberOfErrors);
        [DllImport("icsneo40.dll")]
        public static extern int icsneoGetErrorInfo(int iErrorNumber, StringBuilder sErrorDescriptionShort, StringBuilder sErrorDescriptionLong, ref int iMaxLengthShort, ref int iMaxLengthLong, ref int lErrorSeverity, ref int lRestartNeeded);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoValidateHObject(Int32 hObject);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoGetDLLVersion();
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoStartSockServer(Int32 hObject, Int32 iPort);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoStopSockServer(Int32 hObject);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptStart(Int32 hObject, Int32 iLocation);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptStop(Int32 hObject);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptLoad(Int32 hObject, ref byte bin, UInt32 len_bytes, Int32 iLocation);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptClear(Int32 hObject, Int32 iLocation);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptStartFBlock(Int32 hObject, UInt32 fb_index);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptGetFBlockStatus(Int32 hObject, UInt32 fb_index, ref Int32 piRunStatus);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptStopFBlock(Int32 hObject, UInt32 fb_index);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptGetScriptStatus(Int32 hObject, ref Int32 piStatus);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptReadAppSignal(Int32 hObject, UInt32 iIndex, ref double dValue);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptWriteAppSignal(Int32 hObject, UInt32 iIndex, double dValue);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptReadRxMessage(Int32 hObject, UInt32 iIndex, ref icsSpyMessage pRxMessageMask, ref icsSpyMessage pRxMessageValue);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptReadTxMessage(Int32 hObject, UInt32 iIndex, ref icsSpyMessage pTxMessage);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptWriteRxMessage(Int32 hObject, UInt32 iIndex, ref icsSpyMessage pRxMessageMask, ref icsSpyMessage pRxMessageValue);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoScriptWriteTxMessage(Int32 hObject, UInt32 iIndex, ref icsSpyMessage pTxMessage);
        //[DllImport("icsneo40.dll")]
        //public static extern Int32 icsneoScriptReadISO15765_2_TxMessage(Int32 hObject, UInt32 iIndex, stCM_ISO157652_ref TxMessage pTxMessage);
        //[DllImport("icsneo40.dll")]
        //public static extern Int32 icsneoScriptWriteISO15765_2_TxMessage(Int32 hObject, UInt32 iIndex,  stCM_ISO157652_ref TxMessage pTxMessage);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoOpenPort(Int32 lPortNumber, Int32 lPortType, Int32 lDriverType, ref byte bNetworkID, ref byte bSCPIDs, ref Int32 hObject);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoEnableNetworkCom(Int32 hObject, Int32 Enable);
        [DllImport("icsneo40.dll")]
        public static extern Int32 icsneoFindAllCOMDevices(Int32 lDriverType, Int32 lGetSerialNumbers, Int32 lStopAtFirst, Int32 lUSBCommOnly, ref Int32 p_lDeviceTypes, ref Int32 p_lComPorts, ref Int32 p_lSerialNumbers, ref Int32 lNumDevices);

        public static bool icsneoGetDLLErrorInfo(int lErrorNum, ref StringBuilder sErrorShort, ref StringBuilder sErrorLong, ref int lSeverity, ref int bRestart)
        {
            //int lErrorLongLength = 0;
            //int lErrorShortLength = 0;
            //int lRestart = 0;
            int lResult = 0;

            //sErrorLong = new string('0', 255);
            //sErrorShort = new string('0', 255);

            //lErrorLongLength = 255;
            //lErrorShortLength = 255;

            //lResult = icsneoGetErrorInfo(lErrorNum, sErrorShort, sErrorLong, ref lErrorShortLength, ref lErrorLongLength, ref lSeverity, ref lRestart);

            //sErrorShort = sErrorShort.Substring(0, lErrorShortLength);
            //sErrorLong = sErrorLong.Substring(0, lErrorLongLength);
            //bRestart = lRestart;
            return Convert.ToBoolean(lResult);

        }


        //'// this function returns a text description for a network ID
        public static string GetStringForNetworkID(int lNetworkID)
        {
            string functionReturnValue = null;
            switch (lNetworkID)
            {
                case NETID_HSCAN:
                    functionReturnValue = "HSCAN";
                    break;
                case NETID_MSCAN:
                    functionReturnValue = "MSCAN";
                    break;
                case NETID_SWCAN:
                    functionReturnValue = "SWCAN";
                    break;
                case NETID_LSFTCAN:
                    functionReturnValue = "LSFTCAN";
                    break;
                case NETID_FORDSCP:
                    functionReturnValue = "FORD SCP";
                    break;
                case NETID_J1708:
                    functionReturnValue = "J1708";
                    break;
                case NETID_AUX:
                    functionReturnValue = "AUX";
                    break;
                case NETID_JVPW:
                    functionReturnValue = "J1850 VPW";
                    break;
                case NETID_ISO:
                    functionReturnValue = "ISO/UART";
                    break;
            }
            return functionReturnValue;

        }

        public static double ConvertFromHex(string sValue)
        {
            return Convert.ToInt32(sValue);
        }

        public static double icsneoGetTimeStampVCANneoPRO(ref short TimeHardware, ref short TimeHardware2)
        {
            double NEOVIPRO_VCAN_TIMEHARDWARE_SCALING = 0.0;
            double NEOVIPRO_VCAN_TIMEHARDWARE2_SCALING = 0.0;

            return NEOVIPRO_VCAN_TIMEHARDWARE2_SCALING * TimeHardware2 + NEOVIPRO_VCAN_TIMEHARDWARE_SCALING * TimeHardware;
        }

        public static void ConvertCANtoJ1850Message(ref icsSpyMessage icsCANStruct, ref icsSpyMessageJ1850 icsJ1850Struct)
        {
            icsJ1850Struct.StatusBitField = icsCANStruct.StatusBitField;
            icsJ1850Struct.StatusBitField2 = icsCANStruct.StatusBitField2;
            icsJ1850Struct.TimeHardware = icsCANStruct.TimeHardware;
            icsJ1850Struct.TimeHardware2 = icsCANStruct.TimeHardware2;
            icsJ1850Struct.TimeSystem = icsCANStruct.TimeSystem;
            icsJ1850Struct.TimeSystem2 = icsCANStruct.TimeSystem2;
            icsJ1850Struct.TimeStampHardwareID = icsCANStruct.TimeStampHardwareID;
            icsJ1850Struct.TimeStampSystemID = icsCANStruct.TimeStampSystemID;
            icsJ1850Struct.NetworkID = icsCANStruct.NetworkID;
            icsJ1850Struct.NodeID = icsCANStruct.NodeID;
            icsJ1850Struct.Protocol = icsCANStruct.Protocol;
            icsJ1850Struct.MessagePieceID = icsCANStruct.MessagePieceID;
            icsJ1850Struct.ColorID = icsCANStruct.ColorID;
            icsJ1850Struct.NumberBytesHeader = icsCANStruct.NumberBytesHeader;
            icsJ1850Struct.NumberBytesData = icsCANStruct.NumberBytesData;
            icsJ1850Struct.DescriptionID = icsCANStruct.DescriptionID;
            icsJ1850Struct.Header1 = Convert.ToByte(icsCANStruct.ArbIDOrHeader & 0xff);
            icsJ1850Struct.Header2 = Convert.ToByte((0xFF00 & icsCANStruct.ArbIDOrHeader) / 256);
            icsJ1850Struct.Header3 = Convert.ToByte((0xFF0000 & icsCANStruct.ArbIDOrHeader) / 65536);
            icsJ1850Struct.Data1 = icsCANStruct.Data1;
            icsJ1850Struct.Data2 = icsCANStruct.Data2;
            icsJ1850Struct.Data3 = icsCANStruct.Data3;
            icsJ1850Struct.Data4 = icsCANStruct.Data4;
            icsJ1850Struct.Data5 = icsCANStruct.Data5;
            icsJ1850Struct.Data6 = icsCANStruct.Data6;
            icsJ1850Struct.Data7 = icsCANStruct.Data7;
            icsJ1850Struct.Data8 = icsCANStruct.Data8;
            icsJ1850Struct.AckBytes1 = icsCANStruct.AckBytes1;
            icsJ1850Struct.AckBytes2 = icsCANStruct.AckBytes2;
            icsJ1850Struct.AckBytes3 = icsCANStruct.AckBytes3;
            icsJ1850Struct.AckBytes4 = icsCANStruct.AckBytes4;
            icsJ1850Struct.AckBytes5 = icsCANStruct.AckBytes5;
            icsJ1850Struct.AckBytes6 = icsCANStruct.AckBytes6;
            icsJ1850Struct.AckBytes7 = icsCANStruct.AckBytes7;
            icsJ1850Struct.AckBytes8 = icsCANStruct.AckBytes8;
            icsJ1850Struct.Value = icsCANStruct.Value;
            icsJ1850Struct.MiscData = icsCANStruct.MiscData;
        }

        public static void ConvertJ1850toCAN(ref icsSpyMessage icsCANStruct, ref icsSpyMessageJ1850 icsJ1850Struct)
        {
            //Becuse memcopy is not available.  
            icsCANStruct.StatusBitField = icsJ1850Struct.StatusBitField;
            icsCANStruct.StatusBitField2 = icsJ1850Struct.StatusBitField2;
            icsCANStruct.TimeHardware = icsJ1850Struct.TimeHardware;
            icsCANStruct.TimeHardware2 = icsJ1850Struct.TimeHardware2;
            icsCANStruct.TimeSystem = icsJ1850Struct.TimeSystem;
            icsCANStruct.TimeSystem2 = icsJ1850Struct.TimeSystem2;
            icsCANStruct.TimeStampHardwareID = icsJ1850Struct.TimeStampHardwareID;
            icsCANStruct.TimeStampSystemID = icsJ1850Struct.TimeStampSystemID;
            icsCANStruct.NetworkID = icsJ1850Struct.NetworkID;
            icsCANStruct.NodeID = icsJ1850Struct.NodeID;
            icsCANStruct.Protocol = icsJ1850Struct.Protocol;
            icsCANStruct.MessagePieceID = icsJ1850Struct.MessagePieceID;
            icsCANStruct.ColorID = icsJ1850Struct.ColorID;
            icsCANStruct.NumberBytesHeader = icsJ1850Struct.NumberBytesHeader;
            icsCANStruct.NumberBytesData = icsJ1850Struct.NumberBytesData;
            icsCANStruct.DescriptionID = icsJ1850Struct.DescriptionID;
            icsCANStruct.ArbIDOrHeader = (icsJ1850Struct.Header3 * 65536) + (icsJ1850Struct.Header2 * 256) + icsJ1850Struct.Header1;
            icsCANStruct.Data1 = icsJ1850Struct.Data1;
            icsCANStruct.Data2 = icsJ1850Struct.Data2;
            icsCANStruct.Data3 = icsJ1850Struct.Data3;
            icsCANStruct.Data4 = icsJ1850Struct.Data4;
            icsCANStruct.Data5 = icsJ1850Struct.Data5;
            icsCANStruct.Data6 = icsJ1850Struct.Data6;
            icsCANStruct.Data7 = icsJ1850Struct.Data7;
            icsCANStruct.Data8 = icsJ1850Struct.Data8;
            icsCANStruct.AckBytes1 = icsJ1850Struct.AckBytes1;
            icsCANStruct.AckBytes2 = icsJ1850Struct.AckBytes2;
            icsCANStruct.AckBytes3 = icsJ1850Struct.AckBytes3;
            icsCANStruct.AckBytes4 = icsJ1850Struct.AckBytes4;
            icsCANStruct.AckBytes5 = icsJ1850Struct.AckBytes5;
            icsCANStruct.AckBytes6 = icsJ1850Struct.AckBytes6;
            icsCANStruct.AckBytes7 = icsJ1850Struct.AckBytes7;
            icsCANStruct.AckBytes8 = icsJ1850Struct.AckBytes8;
            icsCANStruct.Value = icsJ1850Struct.Value;
            icsCANStruct.MiscData = icsJ1850Struct.MiscData;
        }

    }
}