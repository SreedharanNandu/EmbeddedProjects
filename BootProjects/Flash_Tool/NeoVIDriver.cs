using System;
using System.Runtime.InteropServices;

namespace FlashTool
{
    class NeoVIDriver
    {
        public int NeoVI_SerialNumber = 0;
        public string NeoVI_Status = "NeoVI Status";
        public icsneo40.icsSpyMessage[] ReceiveMessageBuffer = new icsneo40.icsSpyMessage[128];
        public int Number_of_ReceivedMessages = 0;
        public int[] NeoVI_Channel = new int[5] { 0, 0, 0, 0 ,0};

        private icsneo40.icsSpyMessage TransmitMessageBuffer = new icsneo40.icsSpyMessage();
        private icsneo40.NeoDevice[] NeoVI = new icsneo40.NeoDevice[10];
        private icsneo40.SFireSettings NeoVI_Settings = new icsneo40.SFireSettings();
        private int NeoVI_Handle = 0;
        private int[] NeoVI_ErrorNumber = new int[100];
        private int NeoVI_ErrorCount = 0;
        private bool Connected = false;
        private int NumberOfDevices = 1;
        private int Number_of_Errors = 0;
        private int Result = 0;


        private int[] BAUD_Rate = new int[1] { 500000 };
        private byte[] FIRE_Network_IDs = new byte[64];

        public NeoVIDriver()
        {
            // Fill Network ID array
            for (int I = 0; I <= 63; I++)
            {
                FIRE_Network_IDs[I] = Convert.ToByte(I);
            }
        }

        public int GetBusFromChannel(int channel)
        {
            int bus = 0;

            for (int index = 0; index < (int)FTCommBus.Count; index++)
            {
                if (this.NeoVI_Channel[index] == channel)
                {
                    bus = index;
                    break;
                }
            }

            return bus;
        }
        
        public bool GetReceivedMessages()
        {
            Number_of_ReceivedMessages = 0;
            Number_of_Errors = 0;
            
            // Read the received messages from the Intrepid API
            Result = icsneo40.icsneoGetMessages(NeoVI_Handle, ref ReceiveMessageBuffer[0], ref Number_of_ReceivedMessages, ref Number_of_Errors);

            // convert NeoVI channels to FTCommBuses
            for (int i = 0; i < Number_of_ReceivedMessages; i++)
            {
                ReceiveMessageBuffer[i].NetworkID = (byte)GetBusFromChannel((int)ReceiveMessageBuffer[i].NetworkID);
            }

            if (Result < 1)
            {
                // DLL error
                return false;
            }

            return true;
        }

      

        public bool TransmitMessage(FTCommBus txBus, FTCommMessage myTransmitMessage)
        {
            int TransmitChannel = NeoVI_Channel[(int)txBus];

            if (TransmitChannel > 0)
            {
                // update transmit buffer
                NeoVIUtil.ConvertCommMessageToNeoviMessage(txBus, ref this.TransmitMessageBuffer, myTransmitMessage);

                // check for high voltage wakeup
                if (myTransmitMessage.messageId == 0x100)
                {
                    this.TransmitMessageBuffer.StatusBitField |= 0;
                }

                // Transmit message in buffer
                Result = icsneo40.icsneoTxMessages(NeoVI_Handle, ref this.TransmitMessageBuffer, TransmitChannel, 1);
            }

            if (Result < 1)
            {
                // DLL error
                return false;
            }

            // transmit complete
            return true;
        }

        public void Disconnect()
        {
            NeoVI_SerialNumber = 0;
            Connected = false;
            NeoVI_Status = "Disconnected";
            icsneo40.icsneoFreeObject(NeoVI_Handle);
        }

        public bool IsConnected()
        {
            return Connected;
        }

        public bool Connect()
        {
            // Search for connected Fire hardware
            Result = icsneo40.icsneoFindNeoDevices(icsneo40.NEODEVICE_FIRE, ref NeoVI[0], ref NumberOfDevices);

            if (Result < 1)
            {
                NeoVI_Status = "NeoVI DLL Error";
                return false;
            }

            if (NumberOfDevices < 1)
            {
                NeoVI_Status = "NeoVI Not Found";
                return false;
            }

            // Open the first found device
            Result = icsneo40.icsneoOpenNeoDevice(ref NeoVI[0], ref NeoVI_Handle, ref FIRE_Network_IDs[0], 1, 0);

            // What if the desired NeoVI is not the first device?

            if (Result < 1)
            {
                NeoVI_Status = "NeoVI DLL Error";
                return false;
            }

            // both NeoVI Red and Fire return as FIRE
            if (NeoVI[0].DeviceType != icsneo40.NEODEVICE_FIRE)
            {
                NeoVI_Status = "NeoVI Not Found";
                return false;
            }

            NeoVI_SerialNumber = NeoVI[0].SerialNumber;

            // Read  the FIRE interface settings
            Result = icsneo40.icsneoGetFireSettings(NeoVI_Handle, ref NeoVI_Settings, Marshal.SizeOf(NeoVI_Settings));

            if (Result < 1)
            {
                NeoVI_Status = "NeoVI DLL Error";
                return false;
            }

            // Write the FIRE interface settings
            // this call has been giving access violation errors
            Result = icsneo40.icsneoSetFireSettings(NeoVI_Handle, ref NeoVI_Settings, Marshal.SizeOf(NeoVI_Settings), 1);

            if (Result < 1)
            {
               // NeoVI_Status = "NeoVI DLL Error";
               // return false;
            }

            // enable the receive queue, whatever this does...
            Result = icsneo40.icsneoEnableNetworkRXQueue(NeoVI_Handle, 1);

            if (Result < 1)
            {
                NeoVI_Status = "Recevie queue enable error";
                return false;
            }

            // set baud rates
            for (int bus = 0; bus < (int)FTCommBus.Count; bus++)
            {
                if (NeoVI_Channel[bus] > 0)
                {
                    Result = icsneo40.icsneoSetBitRate(NeoVI_Handle, BAUD_Rate[bus], NeoVI_Channel[bus]);

                    if (Result < 1)
                    {
                        NeoVI_Status = "NeoVI DLL Error";
                        return false;
                    }
                }
            }

            // Read the FIRE interface settings
            Result = icsneo40.icsneoGetFireSettings(NeoVI_Handle, ref NeoVI_Settings, Marshal.SizeOf(NeoVI_Settings));

            if (Result < 1)
            {
                NeoVI_Status = "NeoVI DLL Error";
                return false;
            }

            // Write the FIRE interface settings
            Result = icsneo40.icsneoSetFireSettings(NeoVI_Handle, ref NeoVI_Settings, Marshal.SizeOf(NeoVI_Settings), 1);

            if (Result < 1)
            {
               // NeoVI_Status = "NeoVI DLL Error";
               // return false;
            }

            Connected = true;

            // configuration successful!
            return true;
        }

        public String GetNeoviErrorStatusString()
        {
            String ErrorString = "";

            Result = icsneo40.icsneoGetErrorMessages(NeoVI_Handle, ref NeoVI_ErrorNumber[0], ref NeoVI_ErrorCount);

            if (Result < 1)
            {
                ErrorString = "Error reading errors";
            }
            else
            {
                if (NeoVI_ErrorCount == 0)
                {
                    ErrorString = "No errors reported";
                }
                else
                {
                    ErrorString = NeoVI_ErrorCount.ToString() + " error(s) reported: ";

                    for (int err = 0; err < NeoVI_ErrorCount; err++)
                    {
                        ErrorString += GetNeoviErrorTextFromNumber(NeoVI_ErrorNumber[err]) + "; ";
                    }
                }
            }

            return ErrorString;
        }

        private String GetNeoviErrorTextFromNumber(int ErrorNumber)
        {
            String strHLINMsg = "Unknown Error";

            if (ErrorNumber == 0)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_TX_BUFFER_OVERFLOW  ";
            }
            else if (ErrorNumber == 1)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ERROR_BUFFER_OVERFLOW ";
            }
            else if (ErrorNumber == 2)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_USB_SEND_DATA_ERROR ";
            }
            else if (ErrorNumber == 3)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISO_DATA_BUFFER_ALLOC   ";
            }
            else if (ErrorNumber == 4)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISO_DATA_READ_BUFFER    ";
            }
            else if (ErrorNumber == 5)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISO_DATA_ZERO_PACKETS ";
            }
            if (ErrorNumber == 6)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RX_MSG_BUFFER_OVERFLOW ";
            }
            if (ErrorNumber == 7)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_STOP_ISO_STREAM ";
            }
            if (ErrorNumber == 8)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_INVALID_NETID ";
            }
            if (ErrorNumber == 9)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_STOPPING_RX_THREAD ";
            }
            if (ErrorNumber == 10)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_STOPPING_TX_THREAD  ";
            }
            else if (ErrorNumber == 11)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN_PIC_BUFFER_OVERFLOW  ";
            }
            else if (ErrorNumber == 12)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_INVALID_DEVICE_RESPONSE ";
            }
            else if (ErrorNumber == 13)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISOTX_DATA_BUFFER_ALLOC ";
            }
            else if (ErrorNumber == 14)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RX_CMD_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 15)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_RX_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 16)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_READCOMERR ";
            }
            else if (ErrorNumber == 17)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_READ";
            }
            else if (ErrorNumber == 18)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 19)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_TX_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 20)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_MISC_ERROR";
            }
            else if (ErrorNumber == 21)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_FIND_WRITE";
            }
            else if (ErrorNumber == 22)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_FIND_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 23)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_FIND_CLEARCOMM";
            }
            else if (ErrorNumber == 24)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_FIND_READCOMM";
            }
            else if (ErrorNumber == 25)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_FIND_TIMEOUT";
            }
            else if (ErrorNumber == 26)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_BREAK";
            }
            else if (ErrorNumber == 27)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_FRAME";
            }
            else if (ErrorNumber == 28)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_IOE";
            }
            else if (ErrorNumber == 29)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_OVERRUN";
            }
            else if (ErrorNumber == 30)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_ERR_PARITY";
            }
            else if (ErrorNumber == 31)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_TXBUFFER_ALLOC";
            }
            else if (ErrorNumber == 32)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_USB_TX_RS232_ERROR";
            }
            else if (ErrorNumber == 33)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_CREATE_FILE";
            }
            else if (ErrorNumber == 34)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_GET_COMM_STATE";
            }
            else if (ErrorNumber == 35)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_SET_COMM_STATE";
            }
            else if (ErrorNumber == 36)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_START_COMM_RX_THREAD";
            }
            else if (ErrorNumber == 37)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RS232_START_COMM_TX_THREAD";
            }
            else if (ErrorNumber == 38)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SYNC_COUNT_ERR";
            }
            else if (ErrorNumber == 39)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RX_MSG_FRAME_ERR";
            }
            else if (ErrorNumber == 40)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RX_MSG_FIFO_OVER";
            }
            else if (ErrorNumber == 41)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RX_MSG_CHK_SUM_ERR";
            }
            else if (ErrorNumber == 42)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_STOPPING_BULKIN_THREAD";
            }
            else if (ErrorNumber == 43)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_BULKIN_ERR_READ";
            }

            else if (ErrorNumber == 44)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_RX_FIFO_OVERFLOW";
            }
            else if (ErrorNumber == 45)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW";
            }
            else if (ErrorNumber == 46)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_DEV_FIFO_OVERFLOW";
            }
            else if (ErrorNumber == 47)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ERROR_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 48)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISO_LONG_CACHE_OVERFLOW";
            }
            else if (ErrorNumber == 49)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISORX_LONG_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 50)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_J1708_LONG_CACHE_OVERFLOW";
            }
            else if (ErrorNumber == 51)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_J1708_LONG_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 52)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_DEVICE";
            }
            else if (ErrorNumber == 53)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_HSCAN";
            }

            else if (ErrorNumber == 54)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_MSCAN";
            }
            else if (ErrorNumber == 55)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_SWCAN";
            }
            else if (ErrorNumber == 56)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_LSFTCAN";
            }
            else if (ErrorNumber == 57)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_FORDSCP";
            }
            else if (ErrorNumber == 58)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_J1708";
            }
            else if (ErrorNumber == 59)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_AUX";
            }
            else if (ErrorNumber == 60)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_JVPW";
            }
            else if (ErrorNumber == 61)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_ISO";
            }
            else if (ErrorNumber == 62)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_ISOPIC";
            }
            else if (ErrorNumber == 63)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_MAIN51";
            }
            else if (ErrorNumber == 64)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_FIFO_OVERFLOW_HOST";
            }
            else if (ErrorNumber == 65)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_READ_ENTIRE_DEEPROM_ERROR";
            }
            else if (ErrorNumber == 66)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_WRITE_ENTIRE_DEEPROM_ERROR";
            }
            else if (ErrorNumber == 67)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_USB_PORT_ALREADY_OPEN";
            }
            else if (ErrorNumber == 68)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_JVPW_TX_REPORT_FIFO_ERR_IN";
            }
            else if (ErrorNumber == 69)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISOJ_TX_REPORT_FIFO_ERR_IN";
            }
            else if (ErrorNumber == 70)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_JVPW_TX_REPORT_FIFO_ERR_OUT";
            }
            else if (ErrorNumber == 71)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISOJ_TX_REPORT_FIFO_ERR_OUT";
            }
            else if (ErrorNumber == 72)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_IN_FROM_HOST_FIFO";
            }
            else if (ErrorNumber == 73)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_HOST_CHKSUM";
            }
            else if (ErrorNumber == 74)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ISOJ_TX_HOST_MISSED_BYTE";
            }
            else if (ErrorNumber == 75)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NEOVI_NO_RESPONSE";
            }
            else if (ErrorNumber == 76)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RX_SOCKET_FIFO_OVER";
            }
            else if (ErrorNumber == 77)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_STOPPING_TXSOCKET_THREAD";
            }
            else if (ErrorNumber == 78)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_STOPPING_RXSOCKET_THREAD";
            }
            else if (ErrorNumber == 79)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_STOPPING_RXSOCKET_CLIENT_THREAD";
            }
            else if (ErrorNumber == 80)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_TCP_CLIENT_TX";
            }
            else if (ErrorNumber == 81)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_TCP_CLIENT_RX";
            }
            else if (ErrorNumber == 82)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_TCP_CLIENT_RX_SOCK";
            }
            else if (ErrorNumber == 83)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_UNABLE_CONNECT_TO_SRVR";
            }
            else if (ErrorNumber == 84)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_UNABLE_CREATE_CLIENT_SOCK";
            }
            else if (ErrorNumber == 85)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_UNABLE_WSASTARTUP";
            }
            else if (ErrorNumber == 86)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SOCK_CL_RD_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 87)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SOCK_CL_TX_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 88)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SOCK_SRVR_RX_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 89)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SOCK_SRVR_TX_BUFFER_ALLOC";
            }
            else if (ErrorNumber == 90)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ILLEGAL_TX_NETWORK";
            }
            else if (ErrorNumber == 91)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_TX_HOST_OVERRUN";
            }
            else if (ErrorNumber == 92)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_OPEN_GET_COMM_TIMEOUT";
            }
            else if (ErrorNumber == 93)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_OPEN_SET_COMM_TIMEOUT";
            }
            else if (ErrorNumber == 94)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_OPEN_READ_DEVICE_TYPE";
            }
            else if (ErrorNumber == 95)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_OPEN_READ_DEVICE_TYPE_TOUT";
            }
            else if (ErrorNumber == 96)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_CLOSE_PURGE_COMM";
            }
            else if (ErrorNumber == 97)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_TX_COM_FIFO_OVERFLOW";
            }
            else if (ErrorNumber == 98)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_GET_USBSERIAL_DEVICES";
            }
            else if (ErrorNumber == 99)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_USB_TX_RS232_BCOUNT ";
            }
            else if (ErrorNumber == 100)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_OPEN_INBOOTLOADER ";
            }
            else if (ErrorNumber == 101)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_TOO_MANY_PERIODICS ";
            }
            else if (ErrorNumber == 102)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_PROBLEM_FIRMWARE_INFO ";
            }
            else if (ErrorNumber == 103)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NRED_ODDNUMBYTES ";
            }
            else if (ErrorNumber == 104)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NRED_UNKNOWN_RED_NETID ";
            }
            else if (ErrorNumber == 105)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_NOT_SUPPORTED ";
            }
            else if (ErrorNumber == 106)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_BL_START_INDEX ";
            }
            else if (ErrorNumber == 107)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3G_BL_FAILURE ";
            }
            else if (ErrorNumber == 116)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_BL_END_INDEX  ";
            }

            else if (ErrorNumber == 117)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_FAILED_TO_ENTER_BL ";
            }
            else if (ErrorNumber == 118)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_AUTHENTICATE  ";
            }
            else if (ErrorNumber == 119)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ERROR_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 120)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_APP_VERSION ";
            }
            else if (ErrorNumber == 121)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SET_BAUD_COMM_FAILURE ";
            }
            else if (ErrorNumber == 122)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_INVALID_BAUD_SPECIFIED ";
            }
            else if (ErrorNumber == 123)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_READ_BAUD_COMM_FAILURE ";
            }
            else if (ErrorNumber == 124)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_FAILED_TO_SAVE_EEPROM ";
            }
            else if (ErrorNumber == 125)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_FAILED_TO_UPDATE_WAVEFORM_CHANNEL ";
            }
            else if (ErrorNumber == 126)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_UNKNOWN_NETWORK ";
            }
            else if (ErrorNumber == 127)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_UNKNOWN_NETWORK ";
            }
            else if (ErrorNumber == 128)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_UNKNOWN_NETWORK ";
            }
            else if (ErrorNumber == 129)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_HSCAN1 ";
            }
            else if (ErrorNumber == 130)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_HSCAN1 ";
            }
            else if (ErrorNumber == 131)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_HSCAN1 ";
            }
            else if (ErrorNumber == 132)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_HSCAN1 ";
            }
            else if (ErrorNumber == 133)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ERROR_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 134)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_HSCAN2 ";
            }
            else if (ErrorNumber == 135)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_HSCAN2 ";
            }
            else if (ErrorNumber == 136)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_HSCAN2 ";
            }
            else if (ErrorNumber == 137)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_MSCAN1 ";
            }
            else if (ErrorNumber == 138)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_MSCAN1 ";
            }
            else if (ErrorNumber == 139)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_MSCAN1 ";
            }
            else if (ErrorNumber == 140)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_MSCAN1 ";
            }
            else if (ErrorNumber == 141)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_HSCAN3 ";
            }
            else if (ErrorNumber == 142)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_HSCAN3 ";
            }
            else if (ErrorNumber == 143)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_HSCAN3 ";
            }
            else if (ErrorNumber == 144)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_HSCAN3 ";
            }
            else if (ErrorNumber == 145)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_SWCAN ";
            }
            else if (ErrorNumber == 146)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_SWCAN ";
            }
            else if (ErrorNumber == 147)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_SWCAN ";
            }
            else if (ErrorNumber == 148)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_SWCAN ";
            }
            else if (ErrorNumber == 149)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LSFTCAN ";
            }
            else if (ErrorNumber == 150)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LSFTCAN ";
            }
            else if (ErrorNumber == 151)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LSFTCAN ";
            }
            else if (ErrorNumber == 152)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LSFTCAN ";
            }
            else if (ErrorNumber == 153)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN1 ";
            }
            else if (ErrorNumber == 154)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN1 ";
            }
            else if (ErrorNumber == 155)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN1 ";
            }
            else if (ErrorNumber == 156)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN1 ";
            }
            else if (ErrorNumber == 157)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN2 ";
            }
            else if (ErrorNumber == 158)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN2 ";
            }
            else if (ErrorNumber == 159)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN2 ";
            }
            else if (ErrorNumber == 160)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN2 ";
            }
            else if (ErrorNumber == 161)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN3 ";
            }
            else if (ErrorNumber == 162)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN3 ";
            }
            else if (ErrorNumber == 163)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN3 ";
            }
            else if (ErrorNumber == 164)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN3 ";
            }
            else if (ErrorNumber == 165)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_LIN4 ";
            }
            else if (ErrorNumber == 166)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_LIN4 ";
            }
            else if (ErrorNumber == 167)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_LIN4 ";
            }
            else if (ErrorNumber == 168)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_LIN4 ";
            }
            else if (ErrorNumber == 169)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_USB_PURGE_FAILED ";
            }
            else if (ErrorNumber == 170)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_FIRE_COMM_BAD_PACKET ";
            }
            else if (ErrorNumber == 171)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_FIRE_CGI_COMM_BAD_PACKET  ";
            }
            else if (ErrorNumber == 172)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_HSCAN1 ";
            }
            else if (ErrorNumber == 173)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_HSCAN2 ";
            }
            else if (ErrorNumber == 174)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_HSCAN3 ";
            }
            else if (ErrorNumber == 175)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_MSCAN   ";
            }
            else if (ErrorNumber == 176)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_SWCAN ";
            }
            else if (ErrorNumber == 177)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LSFTCAN ";
            }
            else if (ErrorNumber == 178)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN1 ";
            }
            else if (ErrorNumber == 179)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN2 ";
            }
            else if (ErrorNumber == 180)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN3 ";
            }
            else if (ErrorNumber == 181)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_LIN4 ";
            }
            else if (ErrorNumber == 182)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_UNKNOWN_NETWORK ";
            }
            else if (ErrorNumber == 183)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_JVPW ";
            }
            else if (ErrorNumber == 184)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_JVPW ";
            }
            else if (ErrorNumber == 185)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_JVPW ";
            }
            else if (ErrorNumber == 186)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_JVPW ";
            }
            else if (ErrorNumber == 187)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_INTERNAL_SERIAL_NO_DOES_NOT_MATCH_REGISTRY_SERIAL_NO ";
            }
            else if (ErrorNumber == 188)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_JVPW_LONG_CACHE_OVERFLOW ";
            }
            else if (ErrorNumber == 189)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_FAILED_TO_SET_LICENSE ";
            }
            else if (ErrorNumber == 190)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3G_DEVICE_LICENSE_NEEDS_TO_BE_UPGRADED ";
            }
            else if (ErrorNumber == 191)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_HSCAN  ";
            }
            else if (ErrorNumber == 192)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_MSCAN  ";
            }
            else if (ErrorNumber == 193)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_HSCAN2  ";
            }
            else if (ErrorNumber == 194)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_HSCAN3  ";
            }
            else if (ErrorNumber == 195)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LSFT  ";
            }
            else if (ErrorNumber == 196)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_SW  ";
            }
            else if (ErrorNumber == 197)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN1  ";
            }
            else if (ErrorNumber == 198)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN2  ";
            }
            else if (ErrorNumber == 199)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN3  ";
            }
            else if (ErrorNumber == 200)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_LIN4  ";
            }
            else if (ErrorNumber == 201)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_JVPW ";
            }
            else if (ErrorNumber == 202)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_KYW ";
            }
            else if (ErrorNumber == 203)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_J1708 ";
            }
            else if (ErrorNumber == 204)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_RTC_INVALID ";
            }
            else if (ErrorNumber == 205)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_LOADED_DEFAULT_SETTINGS ";
            }
            else if (ErrorNumber == 206)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_ERROR_BUFFER_OVERFLOW";
            }
            else if (ErrorNumber == 207)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_CGI ";
            }
            else if (ErrorNumber == 208)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_CGI ";
            }
            else if (ErrorNumber == 209)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_CGI ";
            }
            else if (ErrorNumber == 210)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_CGI ";
            }
            else if (ErrorNumber == 211)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_CGI ";
            }
            else if (ErrorNumber == 212)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_JVPW ";
            }
            else if (ErrorNumber == 213)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_INVALID_SCRIPT_LOCATION ";
            }
            else if (ErrorNumber == 214)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SDCARD_NOT_INSERTED ";
            }
            else if (ErrorNumber == 215)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SDCARD_NOT_FORMATTED ";
            }
            else if (ErrorNumber == 216)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SDCARD_WRITE_ERROR ";
            }
            else if (ErrorNumber == 217)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SDCARD_READ_ERROR ";
            }
            else if (ErrorNumber == 218)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_START_ERROR ";
            }
            else if (ErrorNumber == 219)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_INVALID_FUNCBLOCK_INDEX ";
            }
            else if (ErrorNumber == 220)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_ERROR_DOWNLOADING_SCRIPT ";
            }
            else if (ErrorNumber == 221)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_ERROR_CLEARING_SCRIPT ";
            }
            else if (ErrorNumber == 222)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO ";
            }
            else if (ErrorNumber == 223)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO ";
            }
            else if (ErrorNumber == 224)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_INVALID_MSG_INDEX ";
            }
            else if (ErrorNumber == 225)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_INVALID_APPSIG_INDEX ";
            }
            else if (ErrorNumber == 226)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SCRIPT_NO_SCRIPT_RUNNING ";
            }
            else if (ErrorNumber == 227)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_COULD_NOT_SET_SETTINGS_FIRMWARE_MISMATCH ";
            }
            else if (ErrorNumber == 228)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_FIRE_CGI_TX_NOT_ENABLED ";
            }
            else if (ErrorNumber == 229)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_SEND_DEVICE_CONFIG_ERROR ";
            }
            else if (ErrorNumber == 230)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_GET_DEVICE_CONFIG_ERROR ";
            }
            else if (ErrorNumber == 231)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_UNKNOWN_NEOVI_TYPE ";
            }
            else if (ErrorNumber == 232)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO2 ";
            }
            else if (ErrorNumber == 233)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO3 ";
            }
            else if (ErrorNumber == 234)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO4 ";
            }
            else if (ErrorNumber == 235)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO2 ";
            }
            else if (ErrorNumber == 236)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO2 ";
            }
            else if (ErrorNumber == 237)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO2 ";
            }
            else if (ErrorNumber == 238)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_ISO2 ";
            }
            else if (ErrorNumber == 239)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO2 ";
            }
            else if (ErrorNumber == 240)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO3 ";
            }
            else if (ErrorNumber == 241)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO3 ";
            }
            else if (ErrorNumber == 242)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO3 ";
            }
            else if (ErrorNumber == 243)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_ISO3 ";
            }
            else if (ErrorNumber == 244)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO3 ";
            }
            else if (ErrorNumber == 245)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO4 ";
            }
            else if (ErrorNumber == 246)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO4 ";
            }
            else if (ErrorNumber == 247)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO4 ";
            }
            else if (ErrorNumber == 248)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_DRIVER_OVERFLOW_ISO4 ";
            }
            else if (ErrorNumber == 249)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO4 ";
            }
            else if (ErrorNumber == 250)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_FAILED_TO_CLEAR_LIN_SLAVE_DATA ";
            }
            else if (ErrorNumber == 251)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_ENABLED_ISO1 ";
            }
            else if (ErrorNumber == 252)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_RX_MSG_FULL_ISO1 ";
            }
            else if (ErrorNumber == 253)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_MSG_FULL_ISO1 ";
            }
            else if (ErrorNumber == 254)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_TX_REPORT_MSG_FULL_ISO1 ";
            }
            else if (ErrorNumber == 255)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_RED_SETTINGS_NOT_SET_ISO1 ";
            }
            else if (ErrorNumber == 256)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_DROPPED_RTC_CMD ";
            }
            else if (ErrorNumber == 257)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_J1850_TX_REQUESTS_FLUSHED ";
            }
            else if (ErrorNumber == 258)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_J1708_COMM_BAD_PACKET ";
            }
            else if (ErrorNumber == 259)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_NETWORK_NOT_SUPPORTED_BY_HARDWARE ";
            }
            else if (ErrorNumber == 260)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_FEATURE_NOT_UNLOCKED ";
            }
            else if (ErrorNumber == 261)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_DEVICE_NOT_POWERED ";
            }
            else if (ErrorNumber == 262)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_OK ";
            }
            else if (ErrorNumber == 263)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORBADINIT ";
            }
            else if (ErrorNumber == 264)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORNOCANPIPE ";
            }
            else if (ErrorNumber == 265)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORISONEG7FRESPONSE ";
            }
            else if (ErrorNumber == 266)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORTOOLNOTSELECTED ";
            }
            else if (ErrorNumber == 267)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORINVALIDDEVICESELECTED ";
            }
            else if (ErrorNumber == 268)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORCOULDNOTOPENTOOL ";
            }
            else if (ErrorNumber == 269)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORNOFLOWCONTROL ";
            }
            else if (ErrorNumber == 270)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORUNSPECIFIC ";
            }
            else if (ErrorNumber == 271)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORCOREMININULLPTR  ";
            }
            else if (ErrorNumber == 272)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORCOREMINIZEROLEN  ";
            }
            else if (ErrorNumber == 273)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORRESERVED3  ";
            }
            else if (ErrorNumber == 274)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_3GCANDOWNLOADER_ERRORRESERVED4 = 274	 ";
            }
            else if (ErrorNumber == 275)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_YELLOW_DEPRECATED  ";
            }
            else if (ErrorNumber == 276)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_HARDWARE_FAILURE_SRAM  ";
            }
            else if (ErrorNumber == 277)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_ACTIVE_CONNECTION_ALREADY_EXISTS  ";
            }
            else if (ErrorNumber == 278)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_RTC_FAILED_READ  ";
            }
            else if (ErrorNumber == 279)
            {
                strHLINMsg = "Error :  NEOVI_ERROR_DLL_MAIN51_RTC_AUTO_RTC  ";
            }

            return strHLINMsg;
        }

    }
}
