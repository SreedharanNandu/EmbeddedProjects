using System;

namespace FlashTool
{
    // The single image the boot expects:
    //  - contiguous,StartAddress
    //  - gaps and padding filled with 0xFF
    //  - length padded to a multiple of 4
    //  - ends below End Limit(the last 16 bytes belong to the boot)
    public class AppImage
    {
        public const uint StartAddress = 0;
        public const uint EndLimit = 0xFFFFFFFF; // first address that is NOT allowed
        public const int MaxLength = (int)(EndLimit - StartAddress);  

        public byte[] Data;             // the padded image
        public ushort Checksum;         // Vector 16-bit checksum of Data
        public int HexBytes;            // bytes that came from the HEX file
        public int FillBytes;           // 0xFF added (gaps below the highest address + padding to a multiple of 4)

        public int Length { get { return Data.Length; } }
        public uint LastAddress { get { return StartAddress + (uint)Data.Length - 1; } }

        // Throws FormatException if the HEX data does not fit the boot's rules.
        public static AppImage Build(IntelHexParser hex)
        {
            if (hex == null || hex.Blocks.Count == 0)
                throw new FormatException("The HEX file contains no data");

            ulong highest = 0;
            foreach (HexBlock b in hex.Blocks)
            {
                ulong last = (ulong)b.Address + (ulong)b.Data.Length - 1;
                if (b.Address < StartAddress)
                    throw new FormatException("HEX data at 0x" + b.Address.ToString("X8")
                        + " is below the application start 0x" + StartAddress.ToString("X8"));
                if (last >= EndLimit)
                    throw new FormatException("HEX data reaches 0x" + last.ToString("X8") + ", it must end below 0x"
                        + EndLimit.ToString("X8") + " (the last 16 bytes belong to the boot)");
                if (last > highest) highest = last;
            }

            int length = (int)(highest - StartAddress + 1);
            int padded = (length + 3) & ~3;

            byte[] data = new byte[padded];
            for (int i = 0; i < data.Length; i++) data[i] = 0xFF;
            int hexBytes = 0;
            foreach (HexBlock b in hex.Blocks)
            {
                Array.Copy(b.Data, 0, data, (int)(b.Address - StartAddress), b.Data.Length);
                hexBytes += b.Data.Length;
            }

            AppImage image = new AppImage();
            image.Data = data;
            image.HexBytes = hexBytes;
            image.FillBytes = padded - hexBytes;
            image.Checksum = Checksum16(data);
            return image;
        }

        // 16-bit little-endian words added up (wrapping at 65536), then the two's complement.
        // All words plus the checksum come to 0 (mod 65536). The checksum is SENT big-endian.
        public static ushort Checksum16(byte[] image)
        {
            uint acc = 0;
            for (int i = 0; i + 1 < image.Length; i += 2)
                acc += (uint)(image[i] | (image[i + 1] << 8));
            return (ushort)((0x10000 - (acc & 0xFFFF)) & 0xFFFF);
        }

        // The two test vectors from the boot's write-up
        public static bool SelfCheck()
        {
            byte[] v1 = new byte[] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
            byte[] v2 = new byte[16];
            for (int i = 0; i < 16; i++) v2[i] = (byte)i;
            return Checksum16(v1) == 0xEBF0 && Checksum16(v2) == 0xBFC8;
        }
    }
}
