using System;
using System.Collections.Generic;
using System.IO;

namespace FlashTool
{
    // One contiguous run of bytes at a start address
    public class HexBlock
    {
        public uint Address;
        public byte[] Data;
        public uint EndAddress { get { return Address + (uint)Data.Length - 1; } }
    }

    // Reads an Intel HEX file into a list of contiguous memory blocks.
    // Throws FormatException (with the line number) if the file is malformed.
    public class IntelHexParser
    {
        public List<HexBlock> Blocks = new List<HexBlock>();
        public int TotalBytes = 0;

        public static IntelHexParser Load(string path)
        {
            IntelHexParser result = new IntelHexParser();
            uint baseAddress = 0;               // upper address bits from type 02 / 04 records
            uint blockStart = 0;
            List<byte> blockData = null;
            bool endRecordSeen = false;
            int lineNo = 0;

            foreach (string rawLine in File.ReadLines(path))
            {
                lineNo++;
                string line = rawLine.Trim();
                if (line.Length == 0) continue;

                if (line[0] != ':')
                    throw new FormatException("Line " + lineNo + ": record does not start with ':'");
                if (line.Length < 11 || (line.Length % 2) == 0)
                    throw new FormatException("Line " + lineNo + ": bad record length");

                // ":" + LL AAAA TT [data] CC  ->  convert everything after ':' to bytes
                int n = (line.Length - 1) / 2;
                byte[] rec = new byte[n];
                for (int i = 0; i < n; i++)
                {
                    try { rec[i] = Convert.ToByte(line.Substring(1 + i * 2, 2), 16); }
                    catch (FormatException)
                    { throw new FormatException("Line " + lineNo + ": non-hex character"); }
                }

                int count = rec[0];
                if (n != count + 5)
                    throw new FormatException("Line " + lineNo + ": length field does not match record size");

                int sum = 0;
                for (int i = 0; i < n; i++) sum += rec[i];
                if ((sum & 0xFF) != 0)
                    throw new FormatException("Line " + lineNo + ": checksum error");

                int offset = (rec[1] << 8) | rec[2];
                int type = rec[3];

                switch (type)
                {
                    case 0x00:  // data
                        {
                            uint addr = baseAddress + (uint)offset;
                            if (blockData == null || addr != blockStart + (uint)blockData.Count)
                            {
                                result.CloseBlock(blockStart, blockData);   // address jump: start a new block
                                blockStart = addr;
                                blockData = new List<byte>();
                            }
                            for (int i = 0; i < count; i++) blockData.Add(rec[4 + i]);
                            break;
                        }
                    case 0x01:  // end of file
                        endRecordSeen = true;
                        break;
                    case 0x02:  // extended segment address
                        if (count != 2) throw new FormatException("Line " + lineNo + ": bad type 02 record");
                        baseAddress = (uint)((rec[4] << 8) | rec[5]) << 4;
                        break;
                    case 0x04:  // extended linear address
                        if (count != 2) throw new FormatException("Line " + lineNo + ": bad type 04 record");
                        baseAddress = (uint)((rec[4] << 8) | rec[5]) << 16;
                        break;
                    case 0x03:  // start segment address (ignored)
                    case 0x05:  // start linear address (ignored)
                        break;
                    default:
                        throw new FormatException("Line " + lineNo + ": unknown record type " + type.ToString("X2"));
                }
                if (endRecordSeen) break;
            }

            result.CloseBlock(blockStart, blockData);
            if (!endRecordSeen)
                throw new FormatException("End-of-file record (:00000001FF) not found");
            return result;
        }

        private void CloseBlock(uint start, List<byte> data)
        {
            if (data == null || data.Count == 0) return;
            HexBlock b = new HexBlock();
            b.Address = start;
            b.Data = data.ToArray();
            Blocks.Add(b);
            TotalBytes += b.Data.Length;
        }
    }
}
