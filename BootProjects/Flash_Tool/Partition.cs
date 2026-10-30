using System;
using System.Collections.Generic;

namespace FlashTool
{
    public enum ImageStartMode
    {
        FromHex,            // the image (and the address sent in 34) starts at the lowest HEX address in this area
        PartitionStart      // the image always starts at the partition start, the gap below the HEX data is filled
    }

    // One flashable area of the ECU. Everything that differs between areas lives here, so supporting
    // another area (calibration, OpSW, ...) means adding one entry to PartitionTable.Default().
    public class Partition
    {
        public string Name = "Application";

        // The address WINDOW of the area: StartAddress .. EndLimit - 1   (EndLimit = first address NOT allowed).
        // It decides which HEX data belongs to this area and protects everything outside (e.g. the boot).
        // The address that is sent to the ECU does NOT come from here but from the HEX file (see ImageStart).
        public uint StartAddress = 0;
        public uint EndLimit = 0;

        // where the image starts: the lowest HEX address inside the window (rounded down to Alignment) ...
        public ImageStartMode ImageStart = ImageStartMode.FromHex;
        // ... or always the window start (gaps are filled), for boots that insist on one fixed address

        // image rules
        public int Alignment = 4;            // the image length is padded to a multiple of this ...
        public byte FillByte = 0xFF;         // ... with this value (also used for gaps inside the image)
        public bool Required = true;         // flashing fails if the HEX file has no data for this area

        // 34 RequestDownload: 34 <DataFormat> <size bytes | address bytes> <address> <length>
        public byte DataFormat = 0x00;
        public int AddressBytes = 4;
        public int SizeBytes = 4;

        // erase: the complete UDS request
        public byte[] EraseRequest = new byte[] { 0x31, 0x01, 0xFF, 0x00 };
        // true: append <size bytes|address bytes> <address> <length> of the image to the erase request
        // (for boots that erase a range; this boot erases the whole area, so false)
        public bool EraseWithRange = false;

        // check and validate: this prefix, then <length:4> <checksum:2>, all big-endian
        public byte[] CheckRequestPrefix = new byte[] { 0x31, 0x01, 0xFF, 0x01 };

        // state DID read after the check: 22 <did>  ->  62 <did> <state> <length:4> <sum:2>   (0 = not read)
        public ushort StateDid = 0;

        // checksum of the padded image
        public Func<byte[], ushort> Checksum = FlashImage.Checksum16;

        public int MaxLength { get { return (int)(EndLimit - StartAddress); } }
        public uint LastAddress { get { return EndLimit - 1; } }

        public override string ToString()
        {
            return Name + " 0x" + StartAddress.ToString("X8") + " - 0x" + LastAddress.ToString("X8");
        }
    }

    // The ONE place where the memory layout of the ECU is written down.
    public static class PartitionTable
    {
        public static List<Partition> Default()
        {
            List<Partition> list = new List<Partition>();

            // Application window:.
            // The last 16 bytes below belong to the boot, so the window ends accordingly.
            // The download address is taken from the HEX file (ImageStart.FromHex). If this boot only accepts
            // above your set address, HEX may start higher, set ImageStart = ImageStartMode.PartitionStart.
            list.Add(new Partition
            {
                Name = "Application",
                StartAddress = 0x00000000,
                EndLimit = 0xFFFFFFFF,
                StateDid = 0xF1A0
            });

            // Later, for example:
            // list.Add(new Partition { Name = "Calibration", StartAddress = ..., EndLimit = ...,
            //                          EraseRequest = new byte[] { 0x31, 0x01, 0xFF, 0x10 }, ... });
            // list.Add(new Partition { Name = "OpSW", ... });

            return list;
        }
    }
}
