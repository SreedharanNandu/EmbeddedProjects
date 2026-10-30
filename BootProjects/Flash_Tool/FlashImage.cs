using System;
using System.Collections.Generic;

namespace FlashTool
{
    // The image of one partition: contiguous from the partition start, gaps and padding filled,
    // length padded to the partition's alignment.
    public class FlashImage
    {
        public Partition Partition;
        public byte[] Data;             // the padded image
        public ushort Checksum;         // checksum of Data (the partition's method)
        public int HexBytes;            // bytes that came from the HEX file
        public int FillBytes;           // fill bytes added (gaps below the highest address + padding)

        public uint StartAddress;       // where the image starts: this is the address sent in 34
        public int Length { get { return Data.Length; } }
        public uint LastAddress { get { return StartAddress + (uint)Data.Length - 1; } }

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

    // What will be flashed: one image per partition that has data in the HEX file.
    public class FlashPlan
    {
        public List<FlashImage> Images = new List<FlashImage>();

        public int TotalBytes
        {
            get { int t = 0; foreach (FlashImage i in Images) t += i.Length; return t; }
        }

        // Splits the HEX data by address range into the partitions. Throws FormatException if
        // some HEX data lies outside every partition, or if a required partition has no data.
        public static FlashPlan Build(IntelHexParser hex, IList<Partition> partitions)
        {
            if (hex == null || hex.Blocks.Count == 0)
                throw new FormatException("The HEX file contains no data");
            if (partitions == null || partitions.Count == 0)
                throw new FormatException("No partitions are defined");

            // 1. cut every HEX block into the pieces that fall into each partition
            List<HexBlock>[] pieces = new List<HexBlock>[partitions.Count];
            for (int p = 0; p < pieces.Length; p++) pieces[p] = new List<HexBlock>();

            foreach (HexBlock b in hex.Blocks)
            {
                ulong first = b.Address;
                ulong last = first + (ulong)b.Data.Length - 1;
                int covered = 0;
                for (int p = 0; p < partitions.Count; p++)
                {
                    ulong lo = Math.Max(first, (ulong)partitions[p].StartAddress);
                    ulong hi = Math.Min(last, (ulong)partitions[p].EndLimit - 1);
                    if (lo > hi) continue;

                    HexBlock piece = new HexBlock();
                    piece.Address = (uint)lo;
                    piece.Data = new byte[hi - lo + 1];
                    Array.Copy(b.Data, (int)(lo - first), piece.Data, 0, piece.Data.Length);
                    pieces[p].Add(piece);
                    covered += piece.Data.Length;
                }
                if (covered != b.Data.Length)
                {
                    List<string> names = new List<string>();
                    foreach (Partition part in partitions) names.Add(part.ToString());
                    throw new FormatException("HEX data 0x" + first.ToString("X8") + " - 0x" + last.ToString("X8")
                        + " is not inside the partitions (" + string.Join("; ", names.ToArray()) + ")");
                }
            }

            // 2. one padded image per partition
            FlashPlan plan = new FlashPlan();
            for (int p = 0; p < partitions.Count; p++)
            {
                Partition part = partitions[p];
                if (pieces[p].Count == 0)
                {
                    if (part.Required)
                        throw new FormatException("The HEX file has no data for the " + part.Name + " partition ("
                            + part.ToString() + ")");
                    continue;
                }

                ulong lowest = ulong.MaxValue, highest = 0;
                int hexBytes = 0;
                foreach (HexBlock piece in pieces[p])
                {
                    ulong first = piece.Address;
                    ulong last = first + (ulong)piece.Data.Length - 1;
                    if (first < lowest) lowest = first;
                    if (last > highest) highest = last;
                    hexBytes += piece.Data.Length;
                }

                int align = Math.Max(1, part.Alignment);

                // the start address: from the HEX file (rounded down to the alignment), or the window start
                ulong start = part.StartAddress;
                if (part.ImageStart == ImageStartMode.FromHex)
                {
                    start = lowest - (lowest % (ulong)align);
                    if (start < part.StartAddress) start = part.StartAddress;
                }

                int length = (int)(highest - start + 1);
                int padded = ((length + align - 1) / align) * align;
                if (start + (ulong)padded > part.EndLimit)
                    throw new FormatException("The " + part.Name + " image (0x" + start.ToString("X8") + ", " + padded
                        + " bytes with padding) does not fit into the window " + part.ToString());

                byte[] data = new byte[padded];
                for (int i = 0; i < data.Length; i++) data[i] = part.FillByte;
                foreach (HexBlock piece in pieces[p])
                    Array.Copy(piece.Data, 0, data, (int)(piece.Address - start), piece.Data.Length);

                FlashImage image = new FlashImage();
                image.Partition = part;
                image.StartAddress = (uint)start;
                image.Data = data;
                image.HexBytes = hexBytes;
                image.FillBytes = padded - hexBytes;
                image.Checksum = part.Checksum(data);
                plan.Images.Add(image);
            }
            return plan;
        }
    }
}
