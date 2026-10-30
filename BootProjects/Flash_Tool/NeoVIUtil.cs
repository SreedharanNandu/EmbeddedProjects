using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using System.Threading;

namespace FlashTool
{
    class NeoVIUtil
    {
        public static FTCommReceiveMessage ConvertNeoviMessageToCommMessage(ref icsneo40.icsSpyMessage NeoVIMessageRef)
        {
            FTCommReceiveMessage receiveMessage = new FTCommReceiveMessage();
            receiveMessage.bus = (FTCommBus)NeoVIMessageRef.NetworkID;
            receiveMessage.message.messageId = GetMessageID(receiveMessage.bus, NeoVIMessageRef);
            receiveMessage.message.payload = GetMessageData(receiveMessage.bus, NeoVIMessageRef);
            receiveMessage.statusBitfield = NeoVIMessageRef.StatusBitField;
            receiveMessage.statusBitfield = NeoVIMessageRef.StatusBitField2;
            if (receiveMessage.bus == FTCommBus.CAN)
            {
                // CAN message, so verify checksum
                if ((receiveMessage.statusBitfield < 2) && (NeoVIMessageRef.NumberBytesData > 1))
                {
                }
            }

            return receiveMessage;
        }

        public static int GetMessageID(FTCommBus Bus, icsneo40.icsSpyMessage NeoVIMessage)
        {
            int MessageID = 0;
            MessageID = NeoVIMessage.ArbIDOrHeader & 0x1FFFFFFF;
            return MessageID;
        }

        public static byte[] GetMessageData(FTCommBus Bus, icsneo40.icsSpyMessage NeoVIMessage)
        {
            byte[] MessageData = new byte[] { };     
            MessageData = GetMessageDataCAN(NeoVIMessage);
            return MessageData;
        }

        private static byte[] GetMessageDataCAN(icsneo40.icsSpyMessage NeoVIMessage)
        {
            int DataLength;

            DataLength = NeoVIMessage.NumberBytesData;

            if (DataLength <= 0)
            {
                // no data!
                return new byte[] { };
            }

            byte[] MessageData = new byte[DataLength];

            // decode body data
            for (int i = 0; i < DataLength; i++)
            {
                switch (i)
                {
                    case 0:
                        MessageData[i] = NeoVIMessage.Data1;
                        break;
                    case 1:
                        MessageData[i] = NeoVIMessage.Data2;
                        break;
                    case 2:
                        MessageData[i] = NeoVIMessage.Data3;
                        break;
                    case 3:
                        MessageData[i] = NeoVIMessage.Data4;
                        break;
                    case 4:
                        MessageData[i] = NeoVIMessage.Data5;
                        break;
                    case 5:
                        MessageData[i] = NeoVIMessage.Data6;
                        break;
                    case 6:
                        MessageData[i] = NeoVIMessage.Data7;
                        break;
                    case 7:
                        MessageData[i] = NeoVIMessage.Data8;
                        break;
                    default:
                        break;
                }
            }

            return MessageData;
        }




        // convert FT Message to NeoVIMessage
        public static void ConvertCommMessageToNeoviMessage(FTCommBus txBus, ref icsneo40.icsSpyMessage NeoVIMessage, FTCommMessage Message)
        {

          CreateCANNeoVIMessage(ref NeoVIMessage, Message);

        }

        private static void CreateCANNeoVIMessage(ref icsneo40.icsSpyMessage NeoVIMessage, FTCommMessage Message)
        {
            NeoVIMessage.StatusBitField = 0;
            NeoVIMessage.StatusBitField2 = 0;

            if (Message.messageId < 2048)
            {
                // 11-bit GMLAN ID
                NeoVIMessage.StatusBitField &= ~(int)icsneo40.icsSpyDataStatusBitfield.icsSpyStatusXtdFrame;
            }
            else
            {
                // 29-bit GMLAN ID
                NeoVIMessage.StatusBitField |= (int)icsneo40.icsSpyDataStatusBitfield.icsSpyStatusXtdFrame;
            }

            // set the transmit bus
            NeoVIMessage.NetworkID = 0;

            // Message ID and Data

            // set lengths
            //dww is this needed? NeoVI_Transmit_Message.NumberBytesHeader = 3;
            NeoVIMessage.NumberBytesData = (byte)Message.payload.Length;

            // set GMLAN message ID
            NeoVIMessage.ArbIDOrHeader = Message.messageId;

            NeoVIMessage.Data1 = 0;
            NeoVIMessage.Data2 = 0;
            NeoVIMessage.Data3 = 0;
            NeoVIMessage.Data4 = 0;
            NeoVIMessage.Data5 = 0;
            NeoVIMessage.Data6 = 0;
            NeoVIMessage.Data7 = 0;
            NeoVIMessage.Data8 = 0;

            // set Data
            try
            {
                if (Message.payload.Length > 0)
                {
                    NeoVIMessage.Data1 = (byte)Message.payload[0];
                }
                if (Message.payload.Length > 1)
                {
                    NeoVIMessage.Data2 = (byte)Message.payload[1];
                }
                if (Message.payload.Length > 2)
                {
                    NeoVIMessage.Data3 = (byte)Message.payload[2];
                }
                if (Message.payload.Length > 3)
                {
                    NeoVIMessage.Data4 = (byte)Message.payload[3];
                }
                if (Message.payload.Length > 4)
                {
                    NeoVIMessage.Data5 = (byte)Message.payload[4];
                }
                if (Message.payload.Length > 5)
                {
                    NeoVIMessage.Data6 = (byte)Message.payload[5];
                }
                if (Message.payload.Length > 6)
                {
                    NeoVIMessage.Data7 = (byte)Message.payload[6];
                }
                if (Message.payload.Length > 7)
                {
                    NeoVIMessage.Data8 = (byte)Message.payload[7];
                }
            }
            catch
            {
                // do nothing
            }
        }
    }
}
