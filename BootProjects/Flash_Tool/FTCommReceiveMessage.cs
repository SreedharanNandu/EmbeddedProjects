using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FlashTool
{
    class FTCommReceiveMessage
    {
        public FTCommBus bus = FTCommBus.Count;
        public FTCommMessage message = null;
        public int statusBitfield = 0;
        public int statusBitfield2 = 0;

        public FTCommReceiveMessage()
        {
            message = new FTCommMessage();
        }
    }
}
