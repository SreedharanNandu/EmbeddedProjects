using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace FlashTool
{
    // Minimal CAN access, so the TP / UDS code does not depend on the NeoVI DLL.
    public interface ICanBus
    {
        // Transmit one CAN frame (data = the full payload, normally 8 bytes)
        bool Send(int id, byte[] data);

        // Wait up to timeoutMs for a frame. Returns false on timeout.
        bool Receive(out int id, out byte[] data, int timeoutMs);
    }

    public class IsoTpException : Exception
    {
        public IsoTpException(string message) : base(message) { }
    }

    // ISO 15765-2 (CAN TP), classic CAN, normal addressing, 11/29-bit IDs, payload up to 4095 bytes.
    public class IsoTp
    {
        private readonly ICanBus bus;
        private bool detail = true;                           // trace every frame of the current message?
        private readonly HashSet<int> ignoredIds = new HashSet<int>();

        public int TxId;                // tester -> ECU (request ID)
        public int RxId;                // ECU -> tester (response ID)
        public byte Padding = 0xAA;     // filler for unused bytes (use whatever your ECU expects)

        // Flow control we send when the ECU sends us a multi-frame message
        public int BlockSizeRx = 0;     // 0 = send everything without further flow control
        public int StMinRx = 0;

        public int TimeoutBs = 1000;    // ms to wait for the ECU's flow control
        public int TimeoutCr = 1000;    // ms to wait for the next consecutive frame
        public int MaxWaitFrames = 10;  // how many "wait" flow controls we tolerate

        public IsoTp(ICanBus bus, int txId, int rxId)
        {
            this.bus = bus;
            this.TxId = txId;
            this.RxId = rxId;
        }

        // ---------------------------------------------------------------- send
        public void Send(byte[] payload)
        {
            if (payload == null || payload.Length == 0)
                throw new ArgumentException("empty payload");
            if (payload.Length > 4095)
                throw Fail("payload longer than 4095 bytes is not supported");

            detail = payload.Length <= 32 || FlashTrace.ShowAllFrames;
            Stopwatch swSend = Stopwatch.StartNew();
            int cfCount = 0;

            if (payload.Length <= 7)
            {
                // single frame: [0 L] data...
                byte[] sf = NewFrame();
                sf[0] = (byte)payload.Length;
                Array.Copy(payload, 0, sf, 1, payload.Length);
                SendFrame(sf);
                return;
            }

            // first frame: [1 LLL] [LL] + 6 data bytes
            byte[] ff = NewFrame();
            ff[0] = (byte)(0x10 | (payload.Length >> 8));
            ff[1] = (byte)(payload.Length & 0xFF);
            Array.Copy(payload, 0, ff, 2, 6);
            SendFrame(ff);

            int offset = 6;
            int sn = 1;
            int bs, stMin;
            WaitForFlowControl(out bs, out stMin);
            int sentInBlock = 0;

            while (offset < payload.Length)
            {
                int n = Math.Min(7, payload.Length - offset);
                byte[] cf = NewFrame();
                cf[0] = (byte)(0x20 | sn);
                Array.Copy(payload, offset, cf, 1, n);
                SendFrame(cf);
                cfCount++;

                offset += n;
                sn = (sn + 1) & 0x0F;
                sentInBlock++;

                if (offset >= payload.Length) break;

                if (bs != 0 && sentInBlock >= bs)
                {
                    WaitForFlowControl(out bs, out stMin);   // ECU wants to grant the next block
                    sentInBlock = 0;
                }
                else
                {
                    WaitStMin(stMin);
                }
            }

            if (!detail && FlashTrace.Enabled && FlashTrace.ShowFrames)
                FlashTrace.Write("TX", TxId.ToString("X3") + " " + payload.Length + " bytes sent: FF + " + cfCount + " CF in "
                                 + swSend.ElapsedMilliseconds + " ms (BS=" + bs + ", STmin=" + StMinText(stMin) + ")");
        }

        // ------------------------------------------------------------- receive
        // Returns the next complete message from the ECU, or null on timeout.
        public byte[] Receive(int timeoutMs)
        {
            Stopwatch sw = Stopwatch.StartNew();
            while (true)
            {
                int remaining = timeoutMs - (int)sw.ElapsedMilliseconds;
                if (remaining < 0) return null;

                int id; byte[] d;
                if (!bus.Receive(out id, out d, remaining)) return null;
                if (id != RxId || d == null || d.Length < 1)             // not for us (or our own echo)
                {
                    TraceIgnored(id);
                    continue;
                }
                TraceFrame("RX", id, d);

                int pci = d[0] >> 4;
                if (pci == 0)
                {
                    int len = d[0] & 0x0F;
                    if (len == 0 || len > d.Length - 1) continue;        // malformed
                    byte[] msg = new byte[len];
                    Array.Copy(d, 1, msg, 0, len);
                    return msg;
                }
                if (pci == 1)
                {
                    return ReceiveMultiFrame(d);
                }
                // stray flow control / consecutive frame: ignore
            }
        }

        private byte[] ReceiveMultiFrame(byte[] ff)
        {
            int total = ((ff[0] & 0x0F) << 8) | ff[1];
            if (total < 8 || ff.Length < 8)
                throw Fail("malformed first frame");

            byte[] buf = new byte[total];
            int got = 6;
            Array.Copy(ff, 2, buf, 0, 6);
            detail = total <= 32 || FlashTrace.ShowAllFrames;
            Stopwatch swRecv = Stopwatch.StartNew();
            int cfCount = 0;
            SendFlowControl();

            int sn = 1;
            int inBlock = 0;
            while (got < total)
            {
                byte[] d = WaitFrameFromEcu(TimeoutCr, "consecutive frame");
                if ((d[0] >> 4) != 2) continue;                          // ignore anything but CF
                if ((d[0] & 0x0F) != sn)
                    throw Fail("wrong consecutive frame sequence number");

                int n = Math.Min(7, total - got);
                if (d.Length - 1 < n) throw Fail("short consecutive frame");
                Array.Copy(d, 1, buf, got, n);
                got += n;
                sn = (sn + 1) & 0x0F;
                cfCount++;

                inBlock++;
                if (BlockSizeRx != 0 && inBlock >= BlockSizeRx && got < total)
                {
                    SendFlowControl();
                    inBlock = 0;
                }
            }
            if (!detail && FlashTrace.Enabled && FlashTrace.ShowFrames)
                FlashTrace.Write("RX", RxId.ToString("X3") + " " + total + " bytes received: FF + " + cfCount + " CF in "
                                 + swRecv.ElapsedMilliseconds + " ms");
            return buf;
        }

        // ------------------------------------------------------------- helpers
        private void SendFlowControl()
        {
            byte[] fc = NewFrame();
            fc[0] = 0x30;                       // flow control, clear to send
            fc[1] = (byte)BlockSizeRx;
            fc[2] = (byte)StMinRx;
            SendFrame(fc);
        }

        private void WaitForFlowControl(out int bs, out int stMin)
        {
            int waits = 0;
            Stopwatch sw = Stopwatch.StartNew();
            while (true)
            {
                int remaining = TimeoutBs - (int)sw.ElapsedMilliseconds;
                if (remaining <= 0) throw Fail("timeout waiting for flow control");

                int id; byte[] d;
                if (!bus.Receive(out id, out d, remaining))
                    throw Fail("timeout waiting for flow control");
                if (id != RxId || d == null)
                {
                    TraceIgnored(id);
                    continue;
                }
                TraceFrame("RX", id, d);
                if (d.Length < 3 || (d[0] >> 4) != 3) continue;

                int fs = d[0] & 0x0F;
                if (fs == 0) { bs = d[1]; stMin = d[2]; return; }        // clear to send
                if (fs == 1)                                              // wait
                {
                    if (++waits > MaxWaitFrames) throw Fail("too many flow control WAIT frames");
                    sw.Restart();
                    continue;
                }
                throw Fail("ECU aborted transfer (flow status " + fs + ", buffer overflow)");
            }
        }

        private byte[] WaitFrameFromEcu(int timeoutMs, string what)
        {
            Stopwatch sw = Stopwatch.StartNew();
            while (true)
            {
                int remaining = timeoutMs - (int)sw.ElapsedMilliseconds;
                if (remaining <= 0) throw Fail("timeout waiting for " + what);
                int id; byte[] d;
                if (!bus.Receive(out id, out d, remaining))
                    throw Fail("timeout waiting for " + what);
                if (id == RxId && d != null && d.Length >= 1)
                {
                    TraceFrame("RX", id, d);
                    return d;
                }
                TraceIgnored(id);
            }
        }

        // ---------------------------------------------------------------- trace helpers
        private IsoTpException Fail(string message)
        {
            FlashTrace.Write("TP", "ERROR " + message);
            return new IsoTpException(message);
        }

        private void TraceFrame(string dir, int id, byte[] d)
        {
            if (!FlashTrace.Enabled || !FlashTrace.ShowFrames) return;
            int pci = d.Length > 0 ? d[0] >> 4 : -1;
            if (pci == 2 && !detail) return;               // consecutive frames of a long message: summarised later

            string kind = pci == 0 ? "SF" : pci == 1 ? "FF" : pci == 2 ? "CF" : pci == 3 ? "FC" : "??";
            string extra = "";
            if (pci == 3 && d.Length >= 3)
            {
                int fs = d[0] & 0x0F;
                extra = "   " + (fs == 0 ? "clear to send" : fs == 1 ? "WAIT" : "OVERFLOW")
                        + ", BS=" + d[1] + ", STmin=" + StMinText(d[2]);
            }
            FlashTrace.Write(dir, id.ToString("X3") + " " + kind + " | " + FlashTrace.Hex(d, 8) + extra);
        }

        private void TraceIgnored(int id)
        {
            if (!FlashTrace.Enabled || !FlashTrace.ShowFrames) return;
            if (!ignoredIds.Add(id)) return;               // say it once per ID
            FlashTrace.Write("RX", id.ToString("X3") + " ignored, not the response ID " + RxId.ToString("X3")
                             + (id == TxId ? " (our own transmit echo)" : ""));
        }

        private static string StMinText(int stMin)
        {
            if (stMin <= 0x7F) return stMin + " ms";
            if (stMin >= 0xF1 && stMin <= 0xF9) return ((stMin - 0xF0) * 100) + " us";
            return "reserved (127 ms)";
        }

        private byte[] NewFrame()
        {
            byte[] f = new byte[8];
            for (int i = 0; i < 8; i++) f[i] = Padding;
            return f;
        }

        private void SendFrame(byte[] frame)
        {
            // the neoVI queue can be momentarily full when STmin = 0, so retry a few times
            for (int attempt = 0; attempt < 5; attempt++)
            {
                if (bus.Send(TxId, frame))
                {
                    TraceFrame("TX", TxId, frame);
                    return;
                }
                if (FlashTrace.Enabled) FlashTrace.Write("TX", TxId.ToString("X3") + " transmit failed, retry " + (attempt + 1));
                Thread.Sleep(2);
            }
            throw Fail("CAN transmit failed");
        }

        // STmin: 0x00-0x7F = milliseconds, 0xF1-0xF9 = 100-900 microseconds, anything else = 127 ms
        private static void WaitStMin(int stMin)
        {
            long us;
            if (stMin <= 0x7F) us = stMin * 1000L;
            else if (stMin >= 0xF1 && stMin <= 0xF9) us = (stMin - 0xF0) * 100L;
            else us = 127000L;
            if (us == 0) return;

            long ticks = us * Stopwatch.Frequency / 1000000L;
            Stopwatch sw = Stopwatch.StartNew();
            while (sw.ElapsedTicks < ticks)
            {
                long left = (ticks - sw.ElapsedTicks) * 1000000L / Stopwatch.Frequency;   // microseconds left
                if (left > 2500) Thread.Sleep(1);
                else Thread.SpinWait(40);
            }
        }
    }
}
