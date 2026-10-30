using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace FlashTool
{
    class FTUtil
    {
        public static char[] delimiterChars = { ' ', ',', ':', '\t' };

        public static void setBit(ref byte buffer, int whichBit)
        {
            buffer |= (byte)(buffer | (0x01 << whichBit));
        }

        public static void setBit(ref ushort buffer, int whichBit)
        {
            buffer |= (ushort)(buffer | (0x01 << whichBit));
        }

        public static void setBit(byte[] buffer, int whichByte, int whichBit)
        {
            buffer[whichByte] |= (byte)(buffer[whichByte] | (0x01 << whichBit));
        }

        public static void clearBit(ref byte buffer, int whichBit)
        {
            buffer &= (byte)~(buffer & (0x01 << whichBit));
        }

        public static void clearBit(ref ushort buffer, int whichBit)
        {
            buffer &= (ushort)~(buffer & (0x01 << whichBit));
        }

        public static void clearBit(byte[] buffer, int whichByte, int whichBit)
        {
            buffer[whichByte] &= (byte)~(buffer[whichByte] & (0x01 << whichBit));
        }

        public static bool testBit(byte buffer, int whichBit)
        {
            return (buffer == (byte)(buffer | (0x01 << whichBit)));
        }

        public static int GetIntValueFromHexString(String ValueString)
        {
            int returnValue = 0;
            try
            {
                returnValue = Convert.ToInt32(ValueString, 16);
            }
            catch
            {
            }
            return returnValue;
        }

        public static int GetIntValueFromDecString(String ValueString)
        {
            int returnValue = 0;
            try
            {
                returnValue = Convert.ToInt32(ValueString, 10);
            }
            catch
            {
            }
            return returnValue;
        }

        public static String GetASCIIStringFromBytes(byte[] textBytes, int startOffset, int endOffset)
        {
            String returnString = "";

            if ((textBytes != null) && (textBytes.Length > startOffset + endOffset))
            {
                for (int index = startOffset; index < (textBytes.Length - endOffset); index++)
                {
                    // only convert standard ascii chars
                    if ((textBytes[index] >= 0x20) && ((textBytes[index] <= 0x7E)))
                    {
                        returnString += ASCIIEncoding.ASCII.GetString(textBytes, index, 1);
                    }
                }
            }

            return returnString;
        }

        public static String GetStringFromBus(FTCommBus bus)
        {
            String BusName = "";
            switch (bus)
            {
  
                case FTCommBus.CAN:
                    BusName = "CAN";
                    break;

                default:
                    break;
            }
            return BusName;
        }

        public static FTCommBus GetBusFromString(String busName)
        {
            FTCommBus myBus = FTCommBus.Count;
            switch (busName)
            {

                case "CAN":
                    myBus = FTCommBus.CAN;
                    break;

                default:
                    break;
            }
            return myBus;
        }
    }
}
