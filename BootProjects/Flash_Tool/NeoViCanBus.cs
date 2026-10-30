using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;

namespace FlashTool
{
    // Connects the ISO-TP / UDS classes to your NeoVIDriver (HS CAN channel).
    internal class NeoViCanBus : ICanBus
    {
        private readonly NeoVIDriver driver;
        private readonly Queue<KeyValuePair<int, byte[]>> rxQueue = new Queue<KeyValuePair<int, byte[]>>();

        public NeoViCanBus(NeoVIDriver driver)
        {
            this.driver = driver;
        }

        private int lastErrorTick = Environment.TickCount - 10000;

        // Set by the flash thread: returns true when the user wants to stop (Disconnect while programming).
        // Every wait for the ECU and every transmit goes through this class, so the abort takes effect
        // within about a millisecond, also in the middle of a block transfer or while waiting for an erase.
        public Func<bool> AbortCheck = null;

        private void ThrowIfAborted()
        {
            if (AbortCheck != null && AbortCheck()) throw new OperationCanceledException("Aborted by the user");
        }

        public bool Send(int id, byte[] data)
        {
            ThrowIfAborted();
            bool ok = driver.TransmitMessage(FTCommBus.CAN, new FTCommMessage(FTMessageType.WriteCAN, id, data));
            if (!ok) FlashTrace.Write("CAN", "icsneoTxMessages failed for ID " + id.ToString("X3"));
            return ok;
        }

        public bool Receive(out int id, out byte[] data, int timeoutMs)
        {
            Stopwatch sw = Stopwatch.StartNew();
            while (true)
            {
                ThrowIfAborted();
                if (rxQueue.Count > 0)
                {
                    KeyValuePair<int, byte[]> f = rxQueue.Dequeue();
                    id = f.Key;
                    data = f.Value;
                    return true;
                }

                if (driver.GetReceivedMessages())
                {
                    for (int i = 0; i < driver.Number_of_ReceivedMessages; i++)
                    {
                        FTCommReceiveMessage m = NeoVIUtil.ConvertNeoviMessageToCommMessage(ref driver.ReceiveMessageBuffer[i]);
                        rxQueue.Enqueue(new KeyValuePair<int, byte[]>(m.message.messageId, m.message.payload));
                        if (FlashTrace.Enabled && FlashTrace.RawBus)
                            FlashTrace.Write("BUS", m.message.messageId.ToString("X3") + " | " + FlashTrace.Hex(m.message.payload, 8));
                    }
                    if (rxQueue.Count > 0) continue;
                }
                else if (Environment.TickCount - lastErrorTick > 1000)
                {
                    lastErrorTick = Environment.TickCount;
                    FlashTrace.Write("CAN", "icsneoGetMessages failed");
                }

                if (sw.ElapsedMilliseconds >= timeoutMs)
                {
                    id = 0;
                    data = null;
                    return false;
                }
                Thread.Sleep(1);
            }
        }
    }
}
