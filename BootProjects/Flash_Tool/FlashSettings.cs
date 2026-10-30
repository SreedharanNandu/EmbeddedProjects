using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace FlashTool
{
    // One extra raw UDS request, taken from the [Steps] section of FlashTool.ini
    public class CustomStep
    {
        public string Hook;             // where in the sequence it is sent (see FlashSettings.Hooks)
        public byte[] Request;          // the complete request, e.g. 85 02
        public bool Optional;           // true: a refusal / no answer is only logged
        public int TimeoutMs = 0;       // wait after "response pending" (0 = the UDS default)
    }

    // Reads FlashTool.ini (next to the .exe) and applies it on top of the defaults written in the code.
    // A key that is missing in the file keeps the value from the code, so the file only has to contain
    // what you want to change.  The file is read again at every Program click: no restart is needed.
    public static class FlashSettings
    {
        // the points of the sequence where [Steps] entries are sent
        public static readonly string[] Hooks = { "AfterBootEntry", "BeforeErase", "AfterFlash", "AfterReset" };

        public static string IniPath
        {
            get { return Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "FlashTool.ini"); }
        }

        public static IniFile Current = null;
        public static bool ConfirmAbort = true;         // [Tool] ConfirmAbort

        // ------------------------------------------------------------------ loading
        public static IniFile Load()
        {
            IniFile ini = IniFile.Load(IniPath);
            Current = ini;
            ConfirmAbort = ini.GetBool("Tool", "ConfirmAbort", true);

            // the [Log] keys are used once at program start (InitLogging); touch them here so that
            // they are not reported as unknown settings when the file is read again for a flash
            string[] logKeys = { "Enabled", "File", "Append", "MaxSizeKB", "ShowSteps", "ShowUds",
                                 "ShowFrames", "ShowAllFrames", "RawBus", "DebugOutput" };
            foreach (string k in logKeys) ini.Has("Log", k);
            return ini;
        }

        // called once at program start: sets up log.txt and the trace switches from the [Log] section
        public static void InitLogging()
        {
            IniFile ini = Load();

            bool toFile = ini.GetBool("Log", "Enabled", true);
            bool toDebug = ini.GetBool("Log", "DebugOutput", true);
            FlashTrace.ShowSteps = ini.GetBool("Log", "ShowSteps", FlashTrace.ShowSteps);
            FlashTrace.ShowUds = ini.GetBool("Log", "ShowUds", FlashTrace.ShowUds);
            FlashTrace.ShowFrames = ini.GetBool("Log", "ShowFrames", FlashTrace.ShowFrames);
            FlashTrace.ShowAllFrames = ini.GetBool("Log", "ShowAllFrames", FlashTrace.ShowAllFrames);
            FlashTrace.RawBus = ini.GetBool("Log", "RawBus", FlashTrace.RawBus);
            FlashTrace.TraceToDebugger = toDebug;
            FlashTrace.Enabled = toFile || toDebug;

            if (toFile)
            {
                FlashTrace.OpenLog(ini.GetString("Log", "File", "log.txt"),
                                   ini.GetBool("Log", "Append", true),
                                   (long)ini.GetInt("Log", "MaxSizeKB", 4096) * 1024L);
            }

            string version = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString(3);
            FlashTrace.Write("APP", "FlashTool v" + version + " started");
            FlashTrace.Write("CFG", ini.Exists ? "settings file " + ini.Path
                                               : "settings file " + ini.Path + " NOT FOUND, built-in defaults are used");
            foreach (string w in ini.Warnings) FlashTrace.Write("CFG", "WARNING " + w);
        }

        // ------------------------------------------------------------------ CAN IDs
        public static int RequestId(IniFile ini, int def)
        {
            return ini.GetHex("CAN", "RequestId", def);
        }

        public static int ResponseId(IniFile ini, int def)
        {
            return ini.GetHex("CAN", "ResponseId", def);
        }

        // ------------------------------------------------------------------ apply
        public static void Apply(IniFile ini, IsoTp tp, UdsClient uds, UdsFlasher f)
        {
            // ---- ISO-TP
            tp.Padding = GetByte(ini, "IsoTp", "Padding", tp.Padding);
            tp.BlockSizeRx = ini.GetInt("IsoTp", "BlockSizeRx", tp.BlockSizeRx);
            tp.StMinRx = ini.GetInt("IsoTp", "StMinRx", tp.StMinRx);
            tp.TimeoutBs = ini.GetInt("IsoTp", "TimeoutBs", tp.TimeoutBs);
            tp.TimeoutCr = ini.GetInt("IsoTp", "TimeoutCr", tp.TimeoutCr);
            tp.MaxWaitFrames = ini.GetInt("IsoTp", "MaxWaitFrames", tp.MaxWaitFrames);

            // ---- UDS timing
            uds.P2Ms = ini.GetInt("Uds", "P2Ms", uds.P2Ms);
            uds.P2StarMs = ini.GetInt("Uds", "P2StarMs", uds.P2StarMs);

            // ---- entering the boot
            const string S = "Sequence";
            f.UseProgrammingSession = ini.GetBool(S, "UseProgrammingSession", f.UseProgrammingSession);
            f.DetectApplication = ini.GetBool(S, "DetectApplication", f.DetectApplication);
            f.ModeDid = GetUShort(ini, S, "ModeDid", f.ModeDid);
            f.ModeBootData = GetBytesOrNull(ini, S, "ModeBootData", f.ModeBootData);
            f.ModeAppData = GetBytesOrNull(ini, S, "ModeAppData", f.ModeAppData);
            f.AppExtendedSession = ini.GetBool(S, "AppExtendedSession", f.AppExtendedSession);
            f.DisableDtcSetting = ini.GetBool(S, "DisableDtcSetting", f.DisableDtcSetting);
            f.DisableCommunication = ini.GetBool(S, "DisableCommunication", f.DisableCommunication);
            f.JumpResetType = GetByte(ini, S, "JumpResetType", f.JumpResetType);
            f.AfterResetDelayMs = ini.GetInt(S, "AfterResetDelayMs", f.AfterResetDelayMs);
            f.BootWaitMs = ini.GetInt(S, "BootWaitMs", f.BootWaitMs);
            f.BootPollIntervalMs = ini.GetInt(S, "BootPollIntervalMs", f.BootPollIntervalMs);
            f.TesterPresentIntervalMs = ini.GetInt(S, "TesterPresentIntervalMs", f.TesterPresentIntervalMs);

            // ---- reads / writes before the erase
            f.LogBootInfoEnabled = ini.GetBool(S, "ReadBootInfo", f.LogBootInfoEnabled);
            f.ReadDids = ini.GetHexList(S, "ReadDids", f.ReadDids);
            f.ReadDidsMustSucceed = ini.GetBool(S, "ReadDidsMustSucceed", f.ReadDidsMustSucceed);
            if (ini.HasSection("WriteDids")) f.WriteDids = ReadWriteDids(ini);

            // ---- erase / download / check
            f.EraseTimeoutMs = ini.GetInt(S, "EraseTimeoutMs", f.EraseTimeoutMs);
            f.TransferRetries = ini.GetInt(S, "TransferRetries", f.TransferRetries);
            f.CheckTimeoutMs = ini.GetInt(S, "CheckTimeoutMs", f.CheckTimeoutMs);
            f.ReadAppState = ini.GetBool(S, "ReadAppState", f.ReadAppState);
            f.DownloadProgressMax = ini.GetInt(S, "DownloadProgressMax", f.DownloadProgressMax);

            f.FinalRoutine = ini.GetBool(S, "FinalRoutine", f.FinalRoutine);
            f.FinalRoutineType = GetByte(ini, S, "FinalRoutineType", f.FinalRoutineType);
            f.FinalRoutineId = GetUShort(ini, S, "FinalRoutineId", f.FinalRoutineId);
            f.FinalRoutineData = ini.GetBytes(S, "FinalRoutineData", f.FinalRoutineData);

            // ---- reset and the check after it
            f.EcuReset = ini.GetBool(S, "EcuReset", f.EcuReset);
            f.ResetType = GetByte(ini, S, "ResetType", f.ResetType);
            f.VerifyAfterReset = ini.GetBool(S, "VerifyAfterReset", f.VerifyAfterReset);
            f.VerifyTimeoutMs = ini.GetInt(S, "VerifyTimeoutMs", f.VerifyTimeoutMs);
            f.ReadPartNumberAfterReset = ini.GetBool(S, "ReadPartNumberAfterReset", f.ReadPartNumberAfterReset);
            f.PartNumberDid = GetUShort(ini, S, "PartNumberDid", f.PartNumberDid);
            f.PartNumberRetries = ini.GetInt(S, "PartNumberRetries", f.PartNumberRetries);
            f.PartNumberRetryDelayMs = ini.GetInt(S, "PartNumberRetryDelayMs", f.PartNumberRetryDelayMs);
            f.PartNumberMustSucceed = ini.GetBool(S, "PartNumberMustSucceed", f.PartNumberMustSucceed);

            // ---- security access
            f.UseSecurityAccess = ini.GetBool("Security", "Enabled", f.UseSecurityAccess);
            f.SecurityLevel = GetByte(ini, "Security", "Level", f.SecurityLevel);
            Func<byte[], byte[]> seedKey = SeedKey.Create(ini, f.SecurityLevel);
            if (seedKey != null) f.SeedToKey = seedKey;

            // ---- partitions and extra steps
            ApplyPartitions(ini, f);
            f.CustomSteps = ReadSteps(ini);

            // one line in the log with what is really going to happen
            StringBuilder names = new StringBuilder();
            foreach (Partition p in f.Partitions)
            {
                if (names.Length > 0) names.Append(", ");
                names.Append(p.Name);
            }
            FlashTrace.Write("CFG", "security access " + (f.UseSecurityAccess ? "ON (level 0x" + f.SecurityLevel.ToString("X2")
                                    + ", " + ini.GetString("Security", "Method", "None") + ")" : "off")
                                    + ", partitions: " + names + ", extra steps: " + f.CustomSteps.Count
                                    + ", boot detection " + (f.DetectApplication ? "on" : "off")
                                    + ", reset after flash " + (f.EcuReset ? "on" : "off"));
        }

        // after Apply: what the file contained that nobody used, and what was wrong in it
        public static void Report(IniFile ini)
        {
            FlashTrace.Write("CFG", ini.Exists ? "settings file " + ini.Path
                                               : "settings file " + ini.Path + " NOT FOUND, built-in defaults are used");
            foreach (string w in ini.Warnings) FlashTrace.Write("CFG", "WARNING " + w);
            foreach (string u in ini.Unused()) FlashTrace.Write("CFG", "WARNING unknown setting ignored: " + u);
        }

        // ------------------------------------------------------------------ partitions
        private static void ApplyPartitions(IniFile ini, UdsFlasher f)
        {
            f.Partitions = BuildPartitions(ini, f.Partitions);
        }

        // the partitions as the ini file defines them (used by the HEX file check in the main window,
        // so that the check and the flash see the same memory layout)
        public static List<Partition> LoadPartitions()
        {
            return BuildPartitions(Load(), PartitionTable.Default());
        }

        private static List<Partition> BuildPartitions(IniFile ini, List<Partition> current)
        {
            string list = ini.GetString("Partitions", "List", null);
            if (list != null)
            {
                // the list decides which partitions are flashed and in which order (add / remove here)
                List<Partition> result = new List<Partition>();
                string[] names = list.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);
                foreach (string raw in names)
                {
                    string name = raw.Trim();
                    if (name.Length == 0) continue;

                    Partition p = null;
                    foreach (Partition existing in current)
                        if (string.Equals(existing.Name, name, StringComparison.OrdinalIgnoreCase)) p = existing;
                    if (p == null)
                    {
                        p = new Partition();
                        p.Name = name;
                        if (!ini.HasSection("Partition." + name))
                            ini.Warnings.Add("[Partitions] List: '" + name + "' is new and has no [Partition." + name + "] section");
                    }
                    ApplyPartition(ini, p);
                    result.Add(p);
                }
                if (result.Count > 0) return result;
                ini.Warnings.Add("[Partitions] List is empty, the partitions from the code are used");
            }

            foreach (Partition p in current) ApplyPartition(ini, p);
            return current;
        }

        private static void ApplyPartition(IniFile ini, Partition p)
        {
            string sec = "Partition." + p.Name;
            p.StartAddress = ini.GetHexUInt(sec, "StartAddress", p.StartAddress);
            p.EndLimit = ini.GetHexUInt(sec, "EndLimit", p.EndLimit);

            string mode = ini.GetString(sec, "ImageStart", p.ImageStart.ToString());
            if (string.Equals(mode, "FromHex", StringComparison.OrdinalIgnoreCase)) p.ImageStart = ImageStartMode.FromHex;
            else if (string.Equals(mode, "PartitionStart", StringComparison.OrdinalIgnoreCase)) p.ImageStart = ImageStartMode.PartitionStart;
            else ini.Warnings.Add("[" + sec + "] ImageStart: '" + mode + "' is not FromHex / PartitionStart");

            p.Alignment = ini.GetInt(sec, "Alignment", p.Alignment);
            p.FillByte = GetByte(ini, sec, "FillByte", p.FillByte);
            p.Required = ini.GetBool(sec, "Required", p.Required);
            p.DataFormat = GetByte(ini, sec, "DataFormat", p.DataFormat);
            p.AddressBytes = ini.GetInt(sec, "AddressBytes", p.AddressBytes);
            p.SizeBytes = ini.GetInt(sec, "SizeBytes", p.SizeBytes);
            p.EraseRequest = ini.GetBytes(sec, "EraseRequest", p.EraseRequest);
            p.EraseWithRange = ini.GetBool(sec, "EraseWithRange", p.EraseWithRange);
            p.CheckRequestPrefix = ini.GetBytes(sec, "CheckRequestPrefix", p.CheckRequestPrefix);
            p.StateDid = GetUShort(ini, sec, "StateDid", p.StateDid);
        }

        // ------------------------------------------------------------------ DIDs and extra steps
        private static List<KeyValuePair<ushort, byte[]>> ReadWriteDids(IniFile ini)
        {
            List<KeyValuePair<ushort, byte[]>> list = new List<KeyValuePair<ushort, byte[]>>();
            foreach (KeyValuePair<string, string> kv in ini.Entries("WriteDids"))
            {
                long did;
                byte[] data;
                if (!IniFile.ParseNumber(kv.Key, true, out did) || did < 0 || did > 0xFFFF)
                {
                    ini.Warnings.Add("[WriteDids] " + kv.Key + ": the key must be a 16-bit hex DID");
                    continue;
                }
                if (!IniFile.TryParseBytes(kv.Value, out data))
                {
                    ini.Warnings.Add("[WriteDids] " + kv.Key + ": '" + kv.Value + "' are not hex bytes");
                    continue;
                }
                list.Add(new KeyValuePair<ushort, byte[]>((ushort)did, data));
            }
            return list;
        }

        private static List<CustomStep> ReadSteps(IniFile ini)
        {
            List<CustomStep> steps = new List<CustomStep>();
            foreach (KeyValuePair<string, string> kv in ini.Entries("Steps"))
            {
                string hook = null;
                foreach (string h in Hooks)
                    if (string.Equals(h, kv.Key, StringComparison.OrdinalIgnoreCase)) hook = h;
                if (hook == null)
                {
                    ini.Warnings.Add("[Steps] " + kv.Key + ": unknown place, use " + string.Join(", ", Hooks));
                    continue;
                }

                string bytesText = kv.Value;
                string options = "";
                int bar = bytesText.IndexOf('|');
                if (bar >= 0)
                {
                    options = bytesText.Substring(bar + 1);
                    bytesText = bytesText.Substring(0, bar);
                }

                byte[] request;
                if (!IniFile.TryParseBytes(bytesText, out request) || request.Length == 0)
                {
                    ini.Warnings.Add("[Steps] " + kv.Key + ": '" + bytesText.Trim() + "' are not hex bytes");
                    continue;
                }

                CustomStep step = new CustomStep();
                step.Hook = hook;
                step.Request = request;
                foreach (string rawOption in options.Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries))
                {
                    string opt = rawOption.Trim();
                    if (opt.Length == 0) continue;
                    if (string.Equals(opt, "optional", StringComparison.OrdinalIgnoreCase))
                    {
                        step.Optional = true;
                    }
                    else if (opt.StartsWith("timeout=", StringComparison.OrdinalIgnoreCase))
                    {
                        long ms;
                        if (IniFile.ParseNumber(opt.Substring(8), false, out ms) && ms >= 0 && ms <= int.MaxValue) step.TimeoutMs = (int)ms;
                        else ini.Warnings.Add("[Steps] " + kv.Key + ": bad " + opt);
                    }
                    else
                    {
                        ini.Warnings.Add("[Steps] " + kv.Key + ": unknown option '" + opt + "' (use optional, timeout=ms)");
                    }
                }
                steps.Add(step);
            }
            return steps;
        }

        // ------------------------------------------------------------------ small helpers
        private static byte GetByte(IniFile ini, string section, string key, byte def)
        {
            int v = ini.GetHex(section, key, def);
            if (v < 0 || v > 255)
            {
                ini.Warnings.Add("[" + section + "] " + key + ": " + v.ToString("X") + " does not fit into one byte, using 0x" + def.ToString("X2"));
                return def;
            }
            return (byte)v;
        }

        private static ushort GetUShort(IniFile ini, string section, string key, ushort def)
        {
            int v = ini.GetHex(section, key, def);
            if (v < 0 || v > 0xFFFF)
            {
                ini.Warnings.Add("[" + section + "] " + key + ": " + v.ToString("X") + " does not fit into 16 bits, using 0x" + def.ToString("X4"));
                return def;
            }
            return (ushort)v;
        }

        // an empty value switches the pattern off (null)
        private static byte[] GetBytesOrNull(IniFile ini, string section, string key, byte[] def)
        {
            if (!ini.Has(section, key)) return def;
            byte[] b = ini.GetBytes(section, key, def);
            return (b != null && b.Length == 0) ? null : b;
        }
    }

    // Seed -> key for 27 01 / 27 02, chosen with [Security] Method = ... in the ini file.
    //   None      no function (security access cannot be used)
    //   Xor       key[i] = seed[i] xor Mask[i]   (Mask is repeated if it is shorter than the seed)
    //   Add       key = seed + Mask, big-endian, as long as the seed
    //   Constant  the key is always the bytes of Key
    //   Dll       the Vector style seed&key DLL: GenerateKeyEx(seed, size, level, variant, key, max, &keySize)
    // Another algorithm: add a case in Create() that returns your own function.
    public static class SeedKey
    {
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate uint GenerateKeyExCdecl(byte[] seed, uint seedSize, uint level, string variant,
                                                 byte[] key, uint maxKeySize, out uint keySize);

        [UnmanagedFunctionPointer(CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        private delegate uint GenerateKeyExStdCall(byte[] seed, uint seedSize, uint level, string variant,
                                                   byte[] key, uint maxKeySize, out uint keySize);

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        private static extern IntPtr LoadLibrary(string fileName);

        [DllImport("kernel32.dll", CharSet = CharSet.Ansi, SetLastError = true)]
        private static extern IntPtr GetProcAddress(IntPtr module, string procName);

        public static Func<byte[], byte[]> Create(IniFile ini, byte level)
        {
            const string S = "Security";
            string method = ini.GetString(S, "Method", "None").Trim();

            // every key is read here, so none of them is reported as unknown when the method does not use it
            byte[] mask = ini.GetBytes(S, "Mask", new byte[0]);
            byte[] constant = ini.GetBytes(S, "Key", new byte[0]);
            string dll = ini.GetString(S, "Dll", "SeedKey.dll");
            string function = ini.GetString(S, "Function", "GenerateKeyEx");
            string convention = ini.GetString(S, "CallingConvention", "Cdecl");
            string variant = ini.GetString(S, "Variant", "");
            int dllLevel = ini.GetInt(S, "DllLevel", level);

            if (string.Equals(method, "None", StringComparison.OrdinalIgnoreCase)) return null;

            if (string.Equals(method, "Xor", StringComparison.OrdinalIgnoreCase))
            {
                return delegate(byte[] seed)
                {
                    if (mask.Length == 0) throw new UdsException("[Security] Method = Xor needs a Mask");
                    byte[] key = new byte[seed.Length];
                    for (int i = 0; i < seed.Length; i++) key[i] = (byte)(seed[i] ^ mask[i % mask.Length]);
                    return key;
                };
            }

            if (string.Equals(method, "Add", StringComparison.OrdinalIgnoreCase))
            {
                return delegate(byte[] seed)
                {
                    if (mask.Length == 0) throw new UdsException("[Security] Method = Add needs a Mask");
                    byte[] key = new byte[seed.Length];
                    int carry = 0;
                    for (int i = seed.Length - 1; i >= 0; i--)
                    {
                        int mi = i - (seed.Length - mask.Length);          // the mask is aligned to the last byte
                        int m = (mi >= 0 && mi < mask.Length) ? mask[mi] : 0;
                        int sum = seed[i] + m + carry;
                        key[i] = (byte)sum;
                        carry = sum >> 8;
                    }
                    return key;
                };
            }

            if (string.Equals(method, "Constant", StringComparison.OrdinalIgnoreCase))
            {
                return delegate(byte[] seed)
                {
                    if (constant.Length == 0) throw new UdsException("[Security] Method = Constant needs a Key");
                    return (byte[])constant.Clone();
                };
            }

            if (string.Equals(method, "Dll", StringComparison.OrdinalIgnoreCase))
            {
                bool stdcall = string.Equals(convention, "StdCall", StringComparison.OrdinalIgnoreCase);
                return delegate(byte[] seed) { return CallDll(dll, function, stdcall, seed, (uint)dllLevel, variant); };
            }

            ini.Warnings.Add("[Security] Method: '" + method + "' is unknown (None, Xor, Add, Constant, Dll)");
            return null;
        }

        private static byte[] CallDll(string dll, string function, bool stdcall, byte[] seed, uint level, string variant)
        {
            string path = Path.IsPathRooted(dll) ? dll : Path.Combine(AppDomain.CurrentDomain.BaseDirectory, dll);
            IntPtr module = LoadLibrary(path);
            if (module == IntPtr.Zero)
                throw new UdsException("The seed&key DLL " + path + " cannot be loaded (error " + Marshal.GetLastWin32Error()
                                       + "). A 32-bit program needs a 32-bit DLL.");
            IntPtr proc = GetProcAddress(module, function);
            if (proc == IntPtr.Zero)
                throw new UdsException("The function " + function + " is not exported by " + path);

            byte[] buffer = new byte[256];
            uint keySize = 0;
            uint rc;
            if (stdcall)
            {
                GenerateKeyExStdCall fn = (GenerateKeyExStdCall)Marshal.GetDelegateForFunctionPointer(proc, typeof(GenerateKeyExStdCall));
                rc = fn(seed, (uint)seed.Length, level, variant, buffer, (uint)buffer.Length, out keySize);
            }
            else
            {
                GenerateKeyExCdecl fn = (GenerateKeyExCdecl)Marshal.GetDelegateForFunctionPointer(proc, typeof(GenerateKeyExCdecl));
                rc = fn(seed, (uint)seed.Length, level, variant, buffer, (uint)buffer.Length, out keySize);
            }

            if (rc != 0) throw new UdsException("The seed&key DLL returned error code " + rc);
            if (keySize == 0 || keySize > buffer.Length) throw new UdsException("The seed&key DLL returned a bad key length " + keySize);

            byte[] key = new byte[keySize];
            Array.Copy(buffer, key, (int)keySize);
            return key;
        }
    }
}
