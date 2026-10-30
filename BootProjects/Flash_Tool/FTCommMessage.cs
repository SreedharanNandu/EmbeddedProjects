using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FlashTool
{
    public enum FTCommBus
    {
        CAN,
        Count
    };



    public enum FTMessageType
    {
        WriteCAN,
        ReadCAN,
        Count
    };

   

    public enum FTCommMessageId_CAN
    {
        // transmit messages
        DiagReq,
        // receive messages
        DiagResp,
        Count
    };
 
 

    public class FTCommMessage
    {
        public FTMessageType messageType = FTMessageType.Count;
        public int messageId = 0;
        public byte[] payload = null;
        public bool sent = false;

        // empty constructor
        public FTCommMessage()
        {
        }

        // for easier message init
        public FTCommMessage(FTMessageType msgType, int myMessageId, byte[] msgPayload)
        {
            this.messageType = msgType;
            this.messageId = myMessageId;
            this.payload = msgPayload;
        }
    }
}
