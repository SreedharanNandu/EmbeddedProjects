using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.Threading;

namespace FlashTool
{
    public class UdsException : Exception
    {
        public byte Service;
        public byte Nrc;       // 0 = no negative response (timeout etc.)

        public UdsException(string message) : base(message) { }

        public UdsException(byte service, byte nrc)
            : base("Negative response to " + FlashTrace.ServiceName(service) + " (0x" + service.ToString("X2") + "), NRC 0x"
                   + nrc.ToString("X2") + " " + FlashTrace.NrcName(nrc))
        {
            Service = service;
            Nrc = nrc;
        }
    }

    // Sends UDS requests over ISO-TP and returns the positive response.
    // One request/response at a time; an optional background "tester present" (3E 00) is sent
    // only while the bus is idle, never in the middle of another request or a multi-frame message.
    public class UdsClient
    {
        private readonly IsoTp tp;
        private readonly object busLock = new object();
        private Timer tpTimer;
        private volatile bool tpActive = false;
        private int tpIntervalMs = 0;
        private volatile int lastActivityTick = Environment.TickCount;

        public int P2Ms = 1000;         // wait for the first response
        public int P2StarMs = 5000;     // wait after a "response pending" (NRC 0x78)

        public string TesterPresentError = null;   // set if a tester present got no / a bad answer
        public int TesterPresentCount = 0;

        public UdsClient(IsoTp tp) { this.tp = tp; }

        public byte[] Request(params byte[] request)
        {
            return Transaction(request, P2Ms, P2StarMs);
        }

        // same, but with your own limit for the time after "response pending" (erase can take a long time)
        public byte[] RequestLong(int p2StarMs, params byte[] request)
        {
            return Transaction(request, P2Ms, p2StarMs);
        }

        // same, with your own limits for the first answer and for the time after "response pending"
        public byte[] RequestWith(int p2Ms, int p2StarMs, params byte[] request)
        {
            return Transaction(request, p2Ms, p2StarMs);
        }

        private byte[] Transaction(byte[] request, int p2Ms, int p2StarMs)
        {
            lock (busLock)
            {
                Stopwatch swTotal = Stopwatch.StartNew();
                try
                {
                    if (FlashTrace.Enabled && FlashTrace.ShowUds)
                        FlashTrace.Write("UDS>", FlashTrace.Hex(request, 16) + "   " + FlashTrace.ServiceName(request[0]));
                    tp.Send(request);

                    Stopwatch sw = Stopwatch.StartNew();
                    int limit = p2Ms;
                    while (true)
                    {
                        int remaining = limit - (int)sw.ElapsedMilliseconds;
                        byte[] resp = remaining > 0 ? tp.Receive(remaining) : null;
                        if (resp == null)
                        {
                            if (FlashTrace.Enabled && FlashTrace.ShowUds)
                                FlashTrace.Write("UDS<", "NO RESPONSE to " + FlashTrace.ServiceName(request[0]) + " after "
                                                 + swTotal.ElapsedMilliseconds + " ms");
                            throw new UdsException("No response to service 0x" + request[0].ToString("X2"));
                        }

                        if (resp[0] == 0x7F && resp.Length >= 3 && resp[1] == request[0])
                        {
                            if (resp[2] == 0x78)            // response pending: keep waiting
                            {
                                if (FlashTrace.Enabled && FlashTrace.ShowUds)
                                    FlashTrace.Write("UDS<", FlashTrace.Hex(resp, 16) + "   response pending, waiting up to "
                                                     + p2StarMs + " ms");
                                sw.Restart();
                                limit = p2StarMs;
                                continue;
                            }
                            if (FlashTrace.Enabled && FlashTrace.ShowUds)
                                FlashTrace.Write("UDS<", FlashTrace.Hex(resp, 16) + "   NEGATIVE: " + FlashTrace.NrcName(resp[2]));
                            throw new UdsException(request[0], resp[2]);
                        }
                        if (resp[0] == (byte)(request[0] + 0x40))
                        {
                            if (FlashTrace.Enabled && FlashTrace.ShowUds)
                                FlashTrace.Write("UDS<", FlashTrace.Hex(resp, 16) + "   positive, " + swTotal.ElapsedMilliseconds + " ms");
                            return resp;
                        }
                        // anything else is not our answer: ignore and keep waiting
                        if (FlashTrace.Enabled && FlashTrace.ShowUds)
                            FlashTrace.Write("UDS<", FlashTrace.Hex(resp, 16) + "   ignored, not an answer to " + FlashTrace.ServiceName(request[0]));
                    }
                }
                finally
                {
                    lastActivityTick = Environment.TickCount;
                }
            }
        }

        // ---------------------------------------------------------- tester present
        public void StartTesterPresent(int intervalMs)
        {
            StopTesterPresent();
            if (intervalMs <= 0) return;
            tpIntervalMs = intervalMs;
            lastActivityTick = Environment.TickCount;
            TesterPresentError = null;
            tpActive = true;
            int period = Math.Max(20, intervalMs / 4);
            tpTimer = new Timer(TesterPresentTick, null, period, period);
        }

        public void StopTesterPresent()
        {
            tpActive = false;
            Timer t = tpTimer;
            tpTimer = null;
            if (t != null) t.Dispose();
            lock (busLock) { }          // wait for a tester present that may be in progress
        }

        private void TesterPresentTick(object state)
        {
            if (!tpActive) return;
            if (Environment.TickCount - lastActivityTick < tpIntervalMs) return;   // recent traffic keeps the ECU awake
            if (!Monitor.TryEnter(busLock)) return;                                  // a request is running right now
            try
            {
                if (!tpActive) return;
                Transaction(new byte[] { 0x3E, 0x00 }, P2Ms, P2StarMs);
                TesterPresentCount++;
            }
            catch (Exception ex)
            {
                TesterPresentError = ex.Message;
                FlashTrace.Write("UDS", "tester present failed: " + ex.Message);
            }
            finally
            {
                Monitor.Exit(busLock);
            }
        }
    }

    // Flash sequence (per partition, see Partition.cs for what differs between partitions):
    //  enter the boot (see EnterBoot: boot already running -> 10 02; application running -> 10 03/10 01, mode DID, 10 02, 11 01, wait, then 10 02 in the boot)
    //  -> (3E 00 while idle) -> 27 01 / 27 02
    //  -> for every partition: erase -> 34 (whole image, address from the HEX) -> 36 ... -> 37
    //     -> check (31 01 FF 01 <length> <sum>) -> 22 <state DID>
    //  -> 11 01 reset
    public class UdsFlasher
    {
        private readonly UdsClient uds;
        private int lastProgressTrace = -1;
        private long progressDone, progressTotal;

        public Action<string> Log = delegate { };
        public Action<int> Progress = delegate { };
        public Action<string> Info = delegate { };           // one-line ECU info for the panel (boot version, serial, programmed status)
        public volatile bool Cancel = false;
        public Func<bool> CancelCheck = null;                // also asked at every cancel point (set by the flash thread)

        // the areas to flash (see PartitionTable); the HEX file is split into them by address
        public List<Partition> Partitions = PartitionTable.Default();

        public FlashPlan Plan = null;                       // what the last Flash() call flashed
        public Dictionary<ushort, byte[]> ReadResults = new Dictionary<ushort, byte[]>();

        // ======================= settings =======================

        // ---- entering the boot (ISO 14229: DiagnosticSessionControl 10 02; from the application it makes the ECU jump to the boot)
        // true: EnterBoot() below; false: the ECU is assumed to be in the boot already, only 10 02 is sent
        public bool UseProgrammingSession = true;

        // true: first find out what is running (see EnterBoot). false: just send 10 02 and go on.
        public bool DetectApplication = true;

        // ---- how the tool tells APPLICATION from BOOT: it reads this DID (22 <ModeDid>) ----
        // IMPORTANT: it must be a DID whose answer is DIFFERENT in the boot and in the application.
        // (0xF1A0 is NOT suitable: it is the application-state DID and is answered while the application runs.)
        // Rules, in this order:
        //   answer data (after 62 xx xx) starts with ModeBootData -> BOOT
        //   answer data starts with ModeAppData                   -> APPLICATION
        //   positive answer, no pattern matches: boot if ModeBootData is null (DID exists only in the boot), else application
        //   negative answer (e.g. NRC 31 requestOutOfRange)       -> APPLICATION (DID is not known there)
        //   no answer at all                                      -> silent (reset in progress / no ECU)
        public ushort ModeDid = 0xF180;
        public byte[] ModeBootData = null;      // e.g. new byte[] { 0x42, 0x4F, 0x4F, 0x54 } ("BOOT") if your boot returns it
        public byte[] ModeAppData = null;       // e.g. the first bytes of the application's identification

        // application phase: these are sent only while the APPLICATION is running, before the jump.
        // A refusal is only logged: the jump itself (10 02 + 11 01) decides.
        public bool AppExtendedSession = true;              // 10 03 (if refused or off: 10 01)
        public bool DisableDtcSetting = true;               // 85 02
        public bool DisableCommunication = false;           // 28 03 01

        // jump request: 11 <JumpResetType> after 10 02 (03 = soft reset instead of hard reset)
        public byte JumpResetType = 0x03;

        // after that reset: small pause, then ask the mode DID until the boot answers
        public int AfterResetDelayMs = 500;
        public int BootWaitMs = 10000;

        // after the final ECU reset: wait for any answer to the mode DID (boot or application) before reporting success
        public bool VerifyAfterReset = true;
        public int VerifyTimeoutMs = 5000;
        public int BootPollIntervalMs = 250;

        // after the final reset: open a session (10 03, if refused 10 01) and read the application part number (22 F181)
        // so the tool confirms the new software is running and shows its P/N on the panel
        public bool ReadPartNumberAfterReset = true;
        public ushort PartNumberDid = 0xF181;
        public int PartNumberRetries = 5;                   // the application may need a moment before it answers
        public int PartNumberRetryDelayMs = 500;
        public bool PartNumberMustSucceed = false;          // true: the flash is reported as failed if F181 cannot be read

        // extra raw UDS requests from the [Steps] section of FlashTool.ini, sent at fixed places of the sequence:
        //   AfterBootEntry (boot running, programming session open), BeforeErase (after security access),
        //   AfterFlash (all partitions checked, before the reset), AfterReset (after the part number check)
        public List<CustomStep> CustomSteps = new List<CustomStep>();

        // 3E 00 while the bus is idle (0 = off)
        public int TesterPresentIntervalMs = 2000;

        // 27 01 / 27 02: security access. SeedToKey gets the seed and returns the key.
        // The flash stops BEFORE the erase if this is on and SeedToKey is not set.
        public bool UseSecurityAccess = false;
        public byte SecurityLevel = 0x01;                   // request-seed sub-function (the key goes with +1)
        public Func<byte[], byte[]> SeedToKey = null;

        // optional DIDs: 22 xx xx reads (logged), 2E xx xx writes. Example: ReadDids.Add(0xF180);
        public List<ushort> ReadDids = new List<ushort>();
        public bool ReadDidsMustSucceed = false;
        public List<KeyValuePair<ushort, byte[]>> WriteDids = new List<KeyValuePair<ushort, byte[]>>();

        // erase (several "response pending", up to 5 s after each)
        public int EraseTimeoutMs = 5000;

        // 36: how often a block is sent again (same counter) when the answer is lost
        public int TransferRetries = 3;

        // check routine (several "response pending", up to 5 s after each)
        public int CheckTimeoutMs = 5000;

        // read the partition's state DID after the check; the state must be "present"
        public bool ReadAppState = true;

        // optional extra routine at the end (not in the boot's sequence, so off): 31 <type> <id> <data>
        public bool FinalRoutine = false;
        public ushort FinalRoutineId = 0xFF02;
        public byte FinalRoutineType = 0x01;
        public byte[] FinalRoutineData = new byte[0];

        // 11 01: ECU reset
        public bool EcuReset = true;
        public byte ResetType = 0x03;//0x01;

        // the data transfer fills the bar up to this value; check, state read, reset and P/N read follow, then 100
        public int DownloadProgressMax = 95;

        public UdsFlasher(UdsClient uds) { this.uds = uds; }

        // log a sequence step: Debug output window + your Log callback (status text)
        private void Step(string text)
        {
            if (FlashTrace.Enabled && FlashTrace.ShowSteps) FlashTrace.Write("STEP", text);
            Log(text);
        }

        public void Flash(IntelHexParser hex)
        {
            ReadResults.Clear();
            FlashTrace.Reset();
            lastProgressTrace = -1;

            // ---- everything that can be checked without the ECU comes first
            if (!FlashImage.SelfCheck())
                throw new UdsException("Checksum self-test failed (test vectors 0xEBF0 / 0xBFC8)");
            FlashPlan plan = FlashPlan.Build(hex, Partitions);          // throws if the HEX does not fit the partitions
            Plan = plan;
            foreach (FlashImage image in plan.Images)
            {
                Step("Image " + image.Partition.Name + " 0x" + image.StartAddress.ToString("X8") + " - 0x" + image.LastAddress.ToString("X8")
                     + ", " + image.Length + " bytes (" + image.HexBytes + " from HEX, " + image.FillBytes + " filled), checksum 0x"
                     + image.Checksum.ToString("X4"));
            }
            if (UseSecurityAccess && SeedToKey == null)
                throw new UdsException("Security access (27 01 / 27 02) is needed but no SeedToKey function is set");

            progressDone = 0;
            progressTotal = plan.TotalBytes;
            Progress(0);
            try
            {
                EnterBoot();

                if (TesterPresentIntervalMs > 0)
                {
                    uds.StartTesterPresent(TesterPresentIntervalMs);
                }

                RunCustomSteps("AfterBootEntry");

                if (UseSecurityAccess) SecurityAccess();

                foreach (ushort did in ReadDids) { CheckCancel(); ReadDid(did); }
                foreach (KeyValuePair<ushort, byte[]> w in WriteDids) { CheckCancel(); WriteDid(w.Key, w.Value); }

                RunCustomSteps("BeforeErase");

                foreach (FlashImage image in plan.Images)
                {
                    CheckCancel();
                    Step("---- " + image.Partition.Name + " ----");
                    Erase(image);
                    Download(image);
                    Check(image);
                    if (ReadAppState && image.Partition.StateDid != 0) ReadState(image);
                }

                if (FinalRoutine)
                {
                    Step("Routine control 0x" + FinalRoutineId.ToString("X4"));
                    byte[] req = new byte[4 + FinalRoutineData.Length];
                    req[0] = 0x31;
                    req[1] = FinalRoutineType;
                    req[2] = (byte)(FinalRoutineId >> 8);
                    req[3] = (byte)(FinalRoutineId & 0xFF);
                    Array.Copy(FinalRoutineData, 0, req, 4, FinalRoutineData.Length);
                    LogRoutineStatus(uds.RequestLong(CheckTimeoutMs, req));
                }

                RunCustomSteps("AfterFlash");

                uds.StopTesterPresent();

                if (EcuReset)
                {
                    Step("ECU reset (11 " + ResetType.ToString("X2") + ")");
                    try
                    {
                        uds.Request(0x11, ResetType);
                    }
                    catch (UdsException ex)
                    {
                        // an ECU that resets immediately may not answer at all
                        if (ex.Nrc != 0) throw;
                        Step("No answer to the reset request (the ECU may already be resetting)");
                    }

                    // the flash only counts as complete when the ECU is alive again after the reset
                    if (VerifyAfterReset)
                    {
                        Step("Waiting " + AfterResetDelayMs + " ms for the reset");
                        Thread.Sleep(AfterResetDelayMs);
                        Stopwatch vsw = Stopwatch.StartNew();
                        EcuMode alive;
                        while ((alive = ProbeMode()) == EcuMode.Silent)
                        {
                            CheckCancel();
                            if (vsw.ElapsedMilliseconds >= VerifyTimeoutMs)
                                throw new UdsException("The ECU does not answer " + VerifyTimeoutMs + " ms after the reset");
                            Thread.Sleep(BootPollIntervalMs);
                        }
                        Step("The ECU answers after the reset (" + (alive == EcuMode.Boot ? "boot" : "application") + ")");

                        if (ReadPartNumberAfterReset) VerifyPartNumber(alive);
                    }
                }

                RunCustomSteps("AfterReset");

                Progress(100);
                Step("Done");
            }
            finally
            {
                uds.StopTesterPresent();
            }
        }

        // ------------------------------------------------------------------ entering the boot
        //
        // What is running when the tool starts?
        //  1. the boot                      -> only 10 02 (programming session) and go on
        //  2. the application               -> 10 03 (or 10 01), read the mode DID, [27 if enabled], 10 02,
        //                                      11 01 reset to jump to the boot, small timeout, wait until the
        //                                      boot answers the mode DID, then the normal boot sequence (10 02 ...)
        //  3. no application (blank ECU)    -> same as 1
        //
        // If the application jumps by itself on 10 02 (answers pending / resets), the mode DID shows the boot
        // right after the 10 02 and the extra 11 01 is skipped.
        private enum EcuMode { Silent, App, Boot }

        private EcuMode ProbeMode()
        {
            try
            {
                byte[] r = uds.RequestWith(300, 300, 0x22, (byte)(ModeDid >> 8), (byte)(ModeDid & 0xFF));
                byte[] data = new byte[Math.Max(0, r.Length - 3)];
                Array.Copy(r, 3, data, 0, data.Length);
                if (ModeBootData != null && StartsWith(data, ModeBootData)) return EcuMode.Boot;
                if (ModeAppData != null && StartsWith(data, ModeAppData)) return EcuMode.App;
                return ModeBootData == null ? EcuMode.Boot : EcuMode.App;
            }
            catch (UdsException ex)
            {
                return ex.Nrc != 0 ? EcuMode.App : EcuMode.Silent;
            }
            catch (IsoTpException)
            {
                return EcuMode.Silent;
            }
        }

        private EcuMode ProbeModeLogged(string when)
        {
            EcuMode m = ProbeMode();
            Step(when + ": 22 " + ModeDid.ToString("X4") + " -> " + (m == EcuMode.Boot ? "BOOT" : m == EcuMode.App ? "APPLICATION" : "no answer"));
            return m;
        }

        private static bool StartsWith(byte[] data, byte[] prefix)
        {
            if (data.Length < prefix.Length) return false;
            for (int i = 0; i < prefix.Length; i++) if (data[i] != prefix[i]) return false;
            return true;
        }

        private bool Tolerant(string text, params byte[] request)
        {
            Step(text);
            try
            {
                uds.Request(request);
                return true;
            }
            catch (UdsException ex)
            {
                Step("  not accepted (" + ex.Message + "), continuing");
            }
            catch (IsoTpException ex)
            {
                Step("  not sent (" + ex.Message + "), continuing");
            }
            return false;
        }

        private void SecurityAccess()
        {
            Step("Security access (27 " + SecurityLevel.ToString("X2") + " / 27 " + ((byte)(SecurityLevel + 1)).ToString("X2") + ")");
            byte[] r = uds.Request(0x27, SecurityLevel);
            byte[] seed = new byte[r.Length - 2];
            Array.Copy(r, 2, seed, 0, seed.Length);
            byte[] key = SeedToKey(seed);
            byte[] send = new byte[2 + key.Length];
            send[0] = 0x27;
            send[1] = (byte)(SecurityLevel + 1);
            Array.Copy(key, 0, send, 2, key.Length);
            uds.Request(send);
        }

        // log what the boot reports before the flash: serial number (F18C) and programmed status (F1A0)
        public bool LogBootInfoEnabled = true;
        private string infoBoot = "?", infoHw = "?", infoSerial = "?";

        private void LogBootInfo()
        {
            if (!LogBootInfoEnabled) return;
            string version = "?", serial = "?", status = "?";

            byte[] r = ReadInfo(0xF180);
            if (r != null && r.Length >= 6)
            {
                version = "v" + r[3] + "." + r[4] + "." + r[5];
                Step("Boot version (F180): " + version);
            }

            r = ReadInfo(0xF18C);
            if (r != null)
            {
                byte[] s = new byte[r.Length - 3];
                Array.Copy(r, 3, s, 0, s.Length);
                serial = Encoding.ASCII.GetString(s).TrimEnd('\0', ' ');
                Step("ECU serial number (F18C): " + serial + "  [" + Hex(s) + "]");
            }

            string hwVersion = ReadText(0xF191, "Hardware version");
            string appPart = ReadText(0xF181, "Application part number");

            r = ReadInfo(0xF1A0);
            if (r != null)
            {
                if (r.Length < 10)
                {
                    Step("Programmed status (F1A0): short answer " + FlashTrace.Hex(r, 16));
                }
                else
                {
                    int state = r[3];
                    int len = (r[4] << 24) | (r[5] << 16) | (r[6] << 8) | r[7];
                    int sum = (r[8] << 8) | r[9];
                    string name = state == 0x01 ? "PROGRAMMED (present)" : state == 0x02 ? "REVOKED" : state == 0x00 ? "NOT programmed (invalid)" : "0x" + state.ToString("X2");
                    status = state == 0x01 ? "programmed" : state == 0x02 ? "revoked" : state == 0x00 ? "not programmed" : "0x" + state.ToString("X2");
                    Step("Programmed status (F1A0): " + name + ", length " + len + " (0x" + len.ToString("X") + "), sum 0x" + sum.ToString("X4"));
                }
            }

            infoBoot = version; infoHw = hwVersion; infoSerial = serial;
            Info("Boot " + version + "  |  HW " + hwVersion + "  |  S/N " + serial + "  |  App P/N " + appPart + "  |  App: " + status);
        }

        // after the reset: session change (10 03, else 10 01), then 22 F181 to read the part number of what is now running
        private void VerifyPartNumber(EcuMode mode)
        {
            Step("Part number check after the reset");
            string pn = null;
            for (int attempt = 0; attempt <= PartNumberRetries && pn == null; attempt++)
            {
                CheckCancel();
                if (attempt > 0) Thread.Sleep(PartNumberRetryDelayMs);

                // 10 03 first, 10 01 if it is refused (both are only logged, a refusal does not stop the check)
                bool ok = Tolerant("Extended session (10 03)", 0x10, 0x03);
                if (!ok) ok = Tolerant("Default session (10 01)", 0x10, 0x01);

                byte[] r = ReadInfo(PartNumberDid);
                if (r != null)
                {
                    byte[] d = new byte[r.Length - 3];
                    Array.Copy(r, 3, d, 0, d.Length);
                    pn = FormatDid(d);
                    Step("Application part number (" + PartNumberDid.ToString("X4") + ") after the reset: " + pn + "  [" + Hex(d) + "]");
                }
            }

            if (pn == null)
            {
                string msg = "The part number (" + PartNumberDid.ToString("X4") + ") could not be read after the reset";
                if (PartNumberMustSucceed) throw new UdsException(msg);
                Step(msg);
                return;
            }
            Info("Boot " + infoBoot + "  |  HW " + infoHw + "  |  S/N " + infoSerial + "  |  App P/N " + pn
                 + "  |  App: programmed (" + (mode == EcuMode.Boot ? "boot" : "application") + " running)");
        }

        // reads a DID, logs it as text (or version digits) and returns the short text; "?" if it could not be read
        private string ReadText(ushort did, string name)
        {
            byte[] r = ReadInfo(did);
            if (r == null) return "?";
            byte[] d = new byte[r.Length - 3];
            Array.Copy(r, 3, d, 0, d.Length);
            string s = FormatDid(d);
            Step(name + " (" + did.ToString("X4") + "): " + s + "  [" + Hex(d) + "]");
            return s;
        }

        // printable ASCII -> text; 0xFF only (erased) -> "not set"; otherwise numbers joined by dots (e.g. 1.0.2)
        private static string FormatDid(byte[] d)
        {
            if (d.Length == 0) return "(empty)";
            bool allFF = true, ascii = true;
            int end = d.Length;
            while (end > 0 && (d[end - 1] == 0x00 || d[end - 1] == 0x20)) end--;
            for (int i = 0; i < d.Length; i++) if (d[i] != 0xFF) allFF = false;
            if (allFF) return "not set";
            for (int i = 0; i < end; i++) if (d[i] < 0x20 || d[i] > 0x7E) ascii = false;
            if (ascii && end > 0) return Encoding.ASCII.GetString(d, 0, end);
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < d.Length; i++)
            {
                if (i > 0) sb.Append('.');
                sb.Append(d[i]);
            }
            return "v" + sb.ToString();
        }

        // 22 <did>; returns the full positive answer or null (logged) if it failed
        private byte[] ReadInfo(ushort did)
        {
            byte hi = (byte)(did >> 8), lo = (byte)(did & 0xFF);
            try
            {
                byte[] r = uds.Request(0x22, hi, lo);
                if (r.Length >= 3 && r[1] == hi && r[2] == lo) return r;
                Step("Unexpected answer to 22 " + did.ToString("X4") + ": " + FlashTrace.Hex(r, 16));
            }
            catch (UdsException ex) { Step("DID " + did.ToString("X4") + " not read: " + ex.Message); }
            catch (IsoTpException ex) { Step("DID " + did.ToString("X4") + " not read: " + ex.Message); }
            return null;
        }

        private void EnterBoot()
        {
            if (!UseProgrammingSession)
            {
                Step("Assuming the boot is running and in the programming session (UseProgrammingSession is off)");
                return;
            }

            if (!DetectApplication)
            {
                Step("Programming session (10 02)");
                uds.Request(0x10, 0x02);
                return;
            }

            // ---- what is running?
            EcuMode mode = ProbeModeLogged("Mode check");
            if (mode == EcuMode.Silent)
            {
                Thread.Sleep(300);
                mode = ProbeModeLogged("Mode check (2nd try)");
            }
            if (mode == EcuMode.Silent)
                throw new UdsException("No answer from the ECU. Check the power, the CAN wiring and baud rate, and the request / response IDs.");

            if (mode == EcuMode.App)
                JumpToBoot();
            else
                Step("The boot is running");

            LogBootInfo();

            Step("Programming session (10 02)");
            uds.Request(0x10, 0x02);
        }

        // application is running: bring it into the boot
        private void JumpToBoot()
        {
            Step("The application is running: jumping to the boot");

            // 1. session in the application: 10 03, or 10 01
            bool ok = AppExtendedSession && Tolerant("Extended session (10 03)", 0x10, 0x03);
            if (!ok) Tolerant("Default session (10 01)", 0x10, 0x01);

            // 2. read the mode DID again in that session
            EcuMode mode = ProbeModeLogged("After the session change");
            if (mode == EcuMode.Boot) return;                           // it is the boot after all
            if (mode == EcuMode.Silent)
                throw new UdsException("The ECU stopped answering after the session change");

            if (DisableDtcSetting) Tolerant("DTC setting off (85 02)", 0x85, 0x02);
            if (DisableCommunication) Tolerant("Communication control, disable (28 03 01)", 0x28, 0x03, 0x01);

            // 3. security access in the application only if it is switched on
            if (UseSecurityAccess) SecurityAccess();

            // 4. programming session request in the application
            Step("Programming session in the application (10 02)");
            try
            {
                // an application that jumps by itself answers "pending" and the boot answers later
                uds.RequestWith(uds.P2Ms, BootWaitMs, 0x10, 0x02);
            }
            catch (UdsException ex)
            {
                if (ex.Nrc != 0) throw;                                 // the application refused 10 02
                Step("No answer to 10 02 (the ECU may already be jumping to the boot)");
            }

            // 5. still the application? then reset it so that it comes up in the boot
            mode = ProbeModeLogged("After 10 02");
            if (mode == EcuMode.App)
            {
                Step("ECU reset (11 " + JumpResetType.ToString("X2") + ") to jump to the boot");
                try
                {
                    uds.Request(0x11, JumpResetType);
                }
                catch (UdsException ex)
                {
                    if (ex.Nrc != 0) throw;                             // the application refused the reset
                    Step("No answer to the reset request (the ECU may already be resetting)");
                }
            }
            else if (mode == EcuMode.Boot)
            {
                Step("The application jumped by itself, no reset needed");
            }

            // 6. small timeout, then wait until the boot answers
            Step("Waiting " + AfterResetDelayMs + " ms for the reset");
            Thread.Sleep(AfterResetDelayMs);

            Step("Waiting for the boot (up to " + BootWaitMs + " ms)");
            Stopwatch sw = Stopwatch.StartNew();
            while (true)
            {
                CheckCancel();
                mode = ProbeMode();
                if (mode == EcuMode.Boot) break;
                if (sw.ElapsedMilliseconds >= BootWaitMs)
                    throw new UdsException("The ECU did not come up in the boot within " + BootWaitMs + " ms after the reset"
                                           + (mode == EcuMode.App ? " (the mode DID " + ModeDid.ToString("X4") + " still looks like the application)" : " (no answer)"));
                Thread.Sleep(BootPollIntervalMs);
            }
            Step("The boot is up (" + sw.ElapsedMilliseconds + " ms after the reset wait)");
            // the boot now starts its normal sequence: 10 02 is sent by EnterBoot
        }

        // ------------------------------------------------------------------ extra steps from the ini file
        private void RunCustomSteps(string hook)
        {
            foreach (CustomStep step in CustomSteps)
            {
                if (step.Hook != hook) continue;
                CheckCancel();
                Step("Extra step (" + hook + "): " + Hex(step.Request) + (step.Optional ? "  [optional]" : ""));
                try
                {
                    uds.RequestWith(uds.P2Ms, step.TimeoutMs > 0 ? step.TimeoutMs : uds.P2StarMs, step.Request);
                }
                catch (UdsException ex)
                {
                    if (!step.Optional) throw;
                    Step("  not accepted (" + ex.Message + "), continuing");
                }
                catch (IsoTpException ex)
                {
                    if (!step.Optional) throw;
                    Step("  not sent (" + ex.Message + "), continuing");
                }
            }
        }

        // ------------------------------------------------------------------ steps
        private void Erase(FlashImage image)
        {
            Partition part = image.Partition;
            byte[] req = part.EraseRequest;
            if (part.EraseWithRange)
            {
                // <erase request> <size bytes|address bytes> <address> <length> of this image
                req = new byte[part.EraseRequest.Length + 1 + part.AddressBytes + part.SizeBytes];
                Array.Copy(part.EraseRequest, 0, req, 0, part.EraseRequest.Length);
                int o = part.EraseRequest.Length;
                req[o] = (byte)((part.SizeBytes << 4) | part.AddressBytes);
                WriteBigEndian(req, o + 1, image.StartAddress, part.AddressBytes);
                WriteBigEndian(req, o + 1 + part.AddressBytes, (uint)image.Length, part.SizeBytes);
            }
            Step("Erase " + part.Name + " (" + Hex(req) + ")");
            byte[] resp = uds.RequestLong(EraseTimeoutMs, req);
            LogRoutineStatus(resp);
        }

        private void Download(FlashImage image)
        {
            Partition part = image.Partition;
            int total = image.Length;

            // 34 <format> <size bytes|address bytes> <address> <length>, one request for the whole image
            Step("Request download, " + total + " bytes at 0x" + image.StartAddress.ToString("X8")
                 + (part.ImageStart == ImageStartMode.FromHex ? " (the address comes from the HEX file)" : ""));
            byte[] req = new byte[3 + part.AddressBytes + part.SizeBytes];
            req[0] = 0x34;
            req[1] = part.DataFormat;
            req[2] = (byte)((part.SizeBytes << 4) | part.AddressBytes);
            WriteBigEndian(req, 3, image.StartAddress, part.AddressBytes);
            WriteBigEndian(req, 3 + part.AddressBytes, (uint)total, part.SizeBytes);
            byte[] resp = uds.Request(req);                                           // 74 20 <max:2>

            int lenBytes = resp.Length > 1 ? resp[1] >> 4 : 0;
            if (lenBytes == 0 || resp.Length < 2 + lenBytes)
                throw new UdsException("Bad RequestDownload response");
            int max = 0;
            for (int i = 0; i < lenBytes; i++) max = (max << 8) | resp[2 + i];

            // one 36 request = SID + counter + data. The data per block is a multiple of the partition's
            // alignment (4) and ISO-TP carries 4095 bytes at most.
            int dataPerBlock = Math.Min(max, 4095) - 2;
            dataPerBlock -= dataPerBlock % Math.Max(1, part.Alignment);
            if (dataPerBlock < Math.Max(1, part.Alignment))
                throw new UdsException("The boot's maximum block length is too small: " + max);
            Step("Max block length " + max + ", " + dataPerBlock + " data bytes per 36 (multiple of " + part.Alignment + ")");

            byte sn = 1;                                    // counter: starts at 1, 0xFF wraps to 0x00
            int offset = 0;
            while (offset < total)
            {
                CheckCancel();

                int n = Math.Min(dataPerBlock, total - offset);
                byte[] td = new byte[2 + n];
                td[0] = 0x36;
                td[1] = sn;
                Array.Copy(image.Data, offset, td, 2, n);

                TransferBlock(td, sn);

                offset += n;
                progressDone += n;
                sn++;                                       // a byte wraps by itself
                int pct = progressTotal > 0 ? (int)(progressDone * DownloadProgressMax / progressTotal) : 0;   // 100 is reported only when everything is finished
                Progress(pct);
                if (pct / 10 != lastProgressTrace / 10 && FlashTrace.Enabled && FlashTrace.ShowSteps)
                    FlashTrace.Write("PROG", pct + "%  (" + progressDone + " of " + progressTotal + " bytes)");
                lastProgressTrace = pct;
            }

            Step("Transfer exit (37)");
            uds.Request(0x37);                              // 77, NRC 24 if not all bytes were sent
        }

        // If the answer is lost the same block is sent again with the same counter;
        // the boot then acknowledges it without writing it a second time.
        private void TransferBlock(byte[] td, byte sn)
        {
            for (int attempt = 0; ; attempt++)
            {
                byte[] r;
                try
                {
                    r = uds.Request(td);
                }
                catch (UdsException ex)
                {
                    if (ex.Nrc != 0 || attempt >= TransferRetries) throw;      // a negative response aborts
                    Step("No answer to block " + sn + ", sending it again (" + (attempt + 1) + " of " + TransferRetries + ")");
                    continue;
                }
                catch (IsoTpException ex)
                {
                    if (attempt >= TransferRetries) throw;
                    Step("Transport problem at block " + sn + " (" + ex.Message + "), sending it again ("
                         + (attempt + 1) + " of " + TransferRetries + ")");
                    continue;
                }

                if (r.Length < 2 || r[1] != sn)
                    throw new UdsException("Wrong block sequence counter in the response");
                return;
            }
        }

        private void Check(FlashImage image)
        {
            Partition part = image.Partition;
            int pl = part.CheckRequestPrefix.Length;
            Step("Check and validate " + part.Name + " (" + Hex(part.CheckRequestPrefix) + "), length " + image.Length
                 + ", sum 0x" + image.Checksum.ToString("X4"));

            int len = image.Length;
            byte[] req = new byte[pl + 6];
            Array.Copy(part.CheckRequestPrefix, 0, req, 0, pl);
            req[pl + 0] = (byte)(len >> 24);
            req[pl + 1] = (byte)(len >> 16);
            req[pl + 2] = (byte)(len >> 8);
            req[pl + 3] = (byte)len;
            req[pl + 4] = (byte)(image.Checksum >> 8);                              // big-endian on the wire
            req[pl + 5] = (byte)image.Checksum;

            byte[] resp;
            try
            {
                resp = uds.RequestLong(CheckTimeoutMs, req);                      // 71 01 FF 01 <status> <sum:2>
            }
            catch (UdsException ex)
            {
                if (ex.Nrc != 0) Step("Check failed, NRC 0x" + ex.Nrc.ToString("X2") + " = " + CheckNrcHint(ex.Nrc));
                throw;
            }

            if (resp.Length < pl + 3)
                throw new UdsException("Short response to the check routine");
            int status = resp[pl];
            int bootSum = (resp[pl + 1] << 8) | resp[pl + 2];
            if (status != 0x00)
                throw new UdsException("The boot reports a flash MISMATCH in " + part.Name + " (status 0x" + status.ToString("X2")
                                       + "): its sum is 0x" + bootSum.ToString("X4") + ", ours 0x" + image.Checksum.ToString("X4")
                                       + ". Erase and download again.");
            Step("Check OK: status 00 (valid), the boot's sum 0x" + bootSum.ToString("X4"));
            if (bootSum != image.Checksum)
                throw new UdsException("The sum returned by the boot (0x" + bootSum.ToString("X4") + ") differs from ours (0x"
                                       + image.Checksum.ToString("X4") + ")");
        }

        private void ReadState(FlashImage image)
        {
            Partition part = image.Partition;
            byte hi = (byte)(part.StateDid >> 8), lo = (byte)(part.StateDid & 0xFF);
            string didText = part.StateDid.ToString("X4");

            byte[] r;
            try
            {
                r = uds.Request(0x22, hi, lo);                                    // 62 <did> <state> <len:4> <sum:2>
            }
            catch (UdsException ex)
            {
                throw new UdsException("The " + part.Name + " state (" + didText + ") could not be read: " + ex.Message);
            }
            catch (IsoTpException ex)
            {
                throw new UdsException("The " + part.Name + " state (" + didText + ") could not be read: " + ex.Message);
            }

            if (r.Length < 10 || r[1] != hi || r[2] != lo)
                throw new UdsException("Unexpected response to 22 " + didText + ": " + FlashTrace.Hex(r, 16));
            int state = r[3];
            int len = (r[4] << 24) | (r[5] << 16) | (r[6] << 8) | r[7];
            int sum = (r[8] << 8) | r[9];
            string name = state == 0x01 ? "present" : state == 0x02 ? "revoked" : state == 0x00 ? "invalid" : "0x" + state.ToString("X2");
            Step(part.Name + " state: " + name + ", length " + len + ", sum 0x" + sum.ToString("X4"));
            if (state != 0x01)
                throw new UdsException("The " + part.Name + " state is " + name + " after a valid check");
            if (len != image.Length || sum != image.Checksum)
                throw new UdsException("Length / sum in " + didText + " (" + len + " / 0x" + sum.ToString("X4") + ") differ from the image ("
                                       + image.Length + " / 0x" + image.Checksum.ToString("X4") + ")");
        }

        private void ReadDid(ushort did)
        {
            try
            {
                byte[] r = uds.Request(0x22, (byte)(did >> 8), (byte)(did & 0xFF));
                if (r.Length < 3 || r[1] != (byte)(did >> 8) || r[2] != (byte)(did & 0xFF))
                    throw new UdsException("Wrong DID in the response to 22 " + did.ToString("X4"));
                byte[] data = new byte[r.Length - 3];
                Array.Copy(r, 3, data, 0, data.Length);
                ReadResults[did] = data;
                Step("DID " + did.ToString("X4") + " = " + Hex(data));
            }
            catch (UdsException ex)
            {
                if (ReadDidsMustSucceed) throw;
                Step("DID " + did.ToString("X4") + " not read: " + ex.Message);
            }
        }

        private void WriteDid(ushort did, byte[] data)
        {
            byte[] req = new byte[3 + data.Length];
            req[0] = 0x2E;
            req[1] = (byte)(did >> 8);
            req[2] = (byte)(did & 0xFF);
            Array.Copy(data, 0, req, 3, data.Length);
            uds.Request(req);
            Step("DID " + did.ToString("X4") + " written (" + data.Length + " bytes)");
        }

        // ---------------------------------------------------------------- helpers
        private static string CheckNrcHint(byte nrc)
        {
            switch (nrc)
            {
                case 0x13: return "wrong request length";
                case 0x31: return "bad length, or not a multiple of 4";
                case 0x24: return "no erase done, or a transfer is still open";
                case 0x22: return "partial erase, or wrong session";
                case 0x33: return "security access";
                case 0x72: return "write failure";
                default: return FlashTrace.NrcName(nrc);
            }
        }

        private void CheckCancel()
        {
            if (Cancel || (CancelCheck != null && CancelCheck())) throw new OperationCanceledException("Aborted by the user");
        }

        private void LogRoutineStatus(byte[] resp)
        {
            // 71 [type] [id hi] [id lo] [status record...]
            if (resp.Length > 4)
            {
                byte[] status = new byte[resp.Length - 4];
                Array.Copy(resp, 4, status, 0, status.Length);
                Step("Routine status: " + Hex(status));
            }
        }

        private static string Hex(byte[] d)
        {
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < d.Length; i++)
            {
                if (i > 0) sb.Append(' ');
                sb.Append(d[i].ToString("X2"));
            }
            return sb.ToString();
        }

        private static void WriteBigEndian(byte[] dst, int index, uint value, int bytes)
        {
            for (int i = 0; i < bytes; i++)
            {
                int shift = 8 * (bytes - 1 - i);
                dst[index + i] = shift >= 32 ? (byte)0 : (byte)(value >> shift);
            }
        }
    }
}
