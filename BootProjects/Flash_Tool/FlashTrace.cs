using System;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Text;

namespace FlashTool
{
    // Trace of everything the tool does: the Visual Studio "Output" window (while the program runs under
    // the debugger) and the log file log.txt with a date and time on every line.
    // Switch parts on / off with the flags below or in the [Log] section of FlashTool.ini.
    public static class FlashTrace
    {
        public static bool Enabled = true;          // false: nothing at all (neither Output window nor log file)
        public static bool TraceToDebugger = true;  // the Visual Studio "Output" window
        public static bool ShowSteps = true;        // sequence steps (STEP) and progress (PROG)
        public static bool ShowUds = true;          // UDS requests (UDS>) and responses (UDS<)
        public static bool ShowFrames = true;       // ISO-TP frames (TX / RX): all frames of short messages,
                                                    //   only first / flow control frames of long ones
        public static bool ShowAllFrames = false;   // every consecutive frame of long messages (very verbose)
        public static bool RawBus = false;          // every frame the neoVI receives, also from other nodes (BUS)

        private static readonly Stopwatch clock = Stopwatch.StartNew();

        public static void Reset()
        {
            clock.Restart();
        }

        public static void Write(string tag, string text)
        {
            if (!Enabled) return;
            string line = string.Format(CultureInfo.InvariantCulture, "[{0,9:F1} ms] {1,-5} {2}",
                                        clock.Elapsed.TotalMilliseconds, tag, text);
            if (TraceToDebugger) Trace.WriteLine(line);
            WriteToFile(line);
        }

        // ------------------------------------------------------------------ log file
        private static readonly object fileLock = new object();
        private static StreamWriter logWriter = null;
        public static string LogFilePath = "";          // the file that is really used (empty: no log file)

        // Opens the log file (appends, so older sessions stay in it). A relative name is placed next to the .exe.
        // If that folder cannot be written (e.g. Program Files) the file goes to %LocalAppData%\FlashTool instead.
        public static void OpenLog(string fileName, bool append, long maxBytes)
        {
            lock (fileLock)
            {
                CloseLogLocked();
                string baseDir = AppDomain.CurrentDomain.BaseDirectory;
                string name = string.IsNullOrEmpty(fileName) ? "log.txt" : fileName;
                string path = Path.IsPathRooted(name) ? name : Path.Combine(baseDir, name);

                if (!TryOpen(path, append, maxBytes))
                {
                    string fallback = Path.Combine(Path.Combine(
                        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "FlashTool"),
                        Path.GetFileName(path));
                    if (TryOpen(fallback, append, maxBytes))
                        Trace.WriteLine("FlashTrace: cannot write " + path + ", the log goes to " + fallback);
                }
            }
        }

        private static bool TryOpen(string path, bool append, long maxBytes)
        {
            try
            {
                string dir = Path.GetDirectoryName(path);
                if (!string.IsNullOrEmpty(dir)) Directory.CreateDirectory(dir);

                // a log that grew too big is kept once as <name>.old.txt
                if (append && maxBytes > 0 && File.Exists(path) && new FileInfo(path).Length > maxBytes)
                {
                    string old = Path.Combine(dir ?? "", Path.GetFileNameWithoutExtension(path) + ".old" + Path.GetExtension(path));
                    if (File.Exists(old)) File.Delete(old);
                    File.Move(path, old);
                }

                FileStream fs = new FileStream(path, append ? FileMode.Append : FileMode.Create,
                                               FileAccess.Write, FileShare.ReadWrite);   // can be opened while the tool runs
                logWriter = new StreamWriter(fs, new UTF8Encoding(false));
                logWriter.AutoFlush = true;
                LogFilePath = path;
                logWriter.WriteLine();
                logWriter.WriteLine("==================== log opened " + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff",
                                    CultureInfo.InvariantCulture) + " ====================");
                return true;
            }
            catch (Exception ex)
            {
                Trace.WriteLine("FlashTrace: log file " + path + " not usable: " + ex.Message);
                logWriter = null;
                LogFilePath = "";
                return false;
            }
        }

        public static void CloseLog()
        {
            lock (fileLock) { CloseLogLocked(); }
        }

        private static void CloseLogLocked()
        {
            if (logWriter != null)
            {
                try { logWriter.Dispose(); } catch (Exception) { }
                logWriter = null;
            }
            LogFilePath = "";
        }

        private static void WriteToFile(string line)
        {
            if (logWriter == null) return;
            lock (fileLock)
            {
                if (logWriter == null) return;
                try
                {
                    logWriter.WriteLine(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff", CultureInfo.InvariantCulture) + "  " + line);
                }
                catch (Exception)
                {
                    // a full disk or a removed drive must never stop the flash: switch the log file off
                    logWriter = null;
                    LogFilePath = "";
                }
            }
        }

        public static string Hex(byte[] d, int max)
        {
            if (d == null) return "(null)";
            StringBuilder sb = new StringBuilder();
            int n = Math.Min(d.Length, max);
            for (int i = 0; i < n; i++)
            {
                if (i > 0) sb.Append(' ');
                sb.Append(d[i].ToString("X2"));
            }
            if (d.Length > max) sb.Append(" ... (" + d.Length + " bytes)");
            return sb.ToString();
        }

        public static string ServiceName(byte sid)
        {
            switch (sid)
            {
                case 0x10: return "DiagnosticSessionControl";
                case 0x11: return "ECUReset";
                case 0x22: return "ReadDataByIdentifier";
                case 0x27: return "SecurityAccess";
                case 0x2E: return "WriteDataByIdentifier";
                case 0x31: return "RoutineControl";
                case 0x34: return "RequestDownload";
                case 0x36: return "TransferData";
                case 0x37: return "RequestTransferExit";
                case 0x3E: return "TesterPresent";
                case 0x85: return "ControlDTCSetting";
                default: return "service 0x" + sid.ToString("X2");
            }
        }

        public static string NrcName(byte nrc)
        {
            switch (nrc)
            {
                case 0x10: return "generalReject";
                case 0x11: return "serviceNotSupported";
                case 0x12: return "subFunctionNotSupported";
                case 0x13: return "incorrectMessageLengthOrInvalidFormat";
                case 0x14: return "responseTooLong";
                case 0x21: return "busyRepeatRequest";
                case 0x22: return "conditionsNotCorrect";
                case 0x24: return "requestSequenceError";
                case 0x25: return "noResponseFromSubnetComponent";
                case 0x26: return "failurePreventsExecutionOfRequestedAction";
                case 0x31: return "requestOutOfRange";
                case 0x33: return "securityAccessDenied";
                case 0x35: return "invalidKey";
                case 0x36: return "exceededNumberOfAttempts";
                case 0x37: return "requiredTimeDelayNotExpired";
                case 0x70: return "uploadDownloadNotAccepted";
                case 0x71: return "transferDataSuspended";
                case 0x72: return "generalProgrammingFailure";
                case 0x73: return "wrongBlockSequenceCounter";
                case 0x78: return "requestCorrectlyReceived-ResponsePending";
                case 0x7E: return "subFunctionNotSupportedInActiveSession";
                case 0x7F: return "serviceNotSupportedInActiveSession";
                default: return "NRC 0x" + nrc.ToString("X2");
            }
        }
    }
}
