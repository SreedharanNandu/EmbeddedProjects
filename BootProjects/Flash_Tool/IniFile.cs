using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;

namespace FlashTool
{
    // Minimal INI reader:  [Section]  key = value   ; comment   # comment
    //  - section and key names are not case sensitive
    //  - a key may appear several times (Entries() returns all of them in file order, Get() the last one)
    //  - bad values never throw: the default is used and a warning is collected
    //  - keys that nobody asked for are reported by Unused() (usually a typing mistake)
    public class IniFile
    {
        private class Entry
        {
            public string Key;
            public string Value;
            public int Line;
            public bool Used;
        }

        private readonly Dictionary<string, List<Entry>> sections =
            new Dictionary<string, List<Entry>>(StringComparer.OrdinalIgnoreCase);

        public string Path = "";
        public bool Exists = false;
        public readonly List<string> Warnings = new List<string>();

        // ------------------------------------------------------------ loading
        public static IniFile Load(string path)
        {
            IniFile ini = new IniFile();
            ini.Path = path;
            if (!File.Exists(path)) return ini;

            ini.Exists = true;
            string[] lines;
            try
            {
                lines = File.ReadAllLines(path);
            }
            catch (Exception ex)
            {
                ini.Exists = false;
                ini.Warnings.Add("cannot read " + path + ": " + ex.Message);
                return ini;
            }

            string section = "";
            for (int i = 0; i < lines.Length; i++)
            {
                string line = StripComment(lines[i]).Trim();
                if (line.Length == 0) continue;

                if (line[0] == '[')
                {
                    int close = line.IndexOf(']');
                    if (close < 0) { ini.Warnings.Add("line " + (i + 1) + ": missing ] in section header"); continue; }
                    section = line.Substring(1, close - 1).Trim();
                    continue;
                }

                int eq = line.IndexOf('=');
                if (eq <= 0) { ini.Warnings.Add("line " + (i + 1) + ": not a key = value line: " + line); continue; }

                Entry e = new Entry();
                e.Key = line.Substring(0, eq).Trim();
                e.Value = Unquote(line.Substring(eq + 1).Trim());
                e.Line = i + 1;

                List<Entry> list;
                if (!ini.sections.TryGetValue(section, out list))
                {
                    list = new List<Entry>();
                    ini.sections[section] = list;
                }
                list.Add(e);
            }
            return ini;
        }

        // a ; or # starts a comment at the start of the line or after a blank
        private static string StripComment(string s)
        {
            for (int i = 0; i < s.Length; i++)
            {
                if ((s[i] == ';' || s[i] == '#') && (i == 0 || s[i - 1] == ' ' || s[i - 1] == '\t'))
                    return s.Substring(0, i);
            }
            return s;
        }

        private static string Unquote(string s)
        {
            if (s.Length >= 2 && s[0] == '"' && s[s.Length - 1] == '"') return s.Substring(1, s.Length - 2);
            return s;
        }

        // ------------------------------------------------------------ lookup
        private Entry Find(string section, string key)
        {
            List<Entry> list;
            if (!sections.TryGetValue(section, out list)) return null;
            Entry found = null;
            foreach (Entry e in list)
                if (string.Equals(e.Key, key, StringComparison.OrdinalIgnoreCase)) { found = e; e.Used = true; }
            return found;
        }

        public bool Has(string section, string key)
        {
            return Find(section, key) != null;
        }

        public bool HasSection(string section)
        {
            return sections.ContainsKey(section);
        }

        public List<string> SectionNames()
        {
            return new List<string>(sections.Keys);
        }

        // every key = value line of a section in file order (also repeated keys)
        public List<KeyValuePair<string, string>> Entries(string section)
        {
            List<KeyValuePair<string, string>> result = new List<KeyValuePair<string, string>>();
            List<Entry> list;
            if (!sections.TryGetValue(section, out list)) return result;
            foreach (Entry e in list)
            {
                e.Used = true;
                result.Add(new KeyValuePair<string, string>(e.Key, e.Value));
            }
            return result;
        }

        // ------------------------------------------------------------ typed getters
        public string GetString(string section, string key, string def)
        {
            Entry e = Find(section, key);
            return e == null ? def : e.Value;
        }

        public bool GetBool(string section, string key, bool def)
        {
            Entry e = Find(section, key);
            if (e == null) return def;
            string v = e.Value.ToLowerInvariant();
            if (v == "true" || v == "yes" || v == "on" || v == "1") return true;
            if (v == "false" || v == "no" || v == "off" || v == "0") return false;
            Warnings.Add("[" + section + "] " + key + " (line " + e.Line + "): '" + e.Value + "' is not true/false, using " + def);
            return def;
        }

        // decimal number (0x prefix = hex): times, counts
        public int GetInt(string section, string key, int def)
        {
            Entry e = Find(section, key);
            if (e == null) return def;
            long v;
            if (ParseNumber(e.Value, false, out v) && v >= int.MinValue && v <= int.MaxValue) return (int)v;
            Warnings.Add("[" + section + "] " + key + " (line " + e.Line + "): '" + e.Value + "' is not a number, using " + def);
            return def;
        }

        // hexadecimal number (0x prefix optional): IDs, DIDs, service bytes
        public int GetHex(string section, string key, int def)
        {
            Entry e = Find(section, key);
            if (e == null) return def;
            long v;
            if (ParseNumber(e.Value, true, out v) && v >= 0 && v <= int.MaxValue) return (int)v;
            Warnings.Add("[" + section + "] " + key + " (line " + e.Line + "): '" + e.Value + "' is not a hex number, using 0x" + def.ToString("X"));
            return def;
        }

        public uint GetHexUInt(string section, string key, uint def)
        {
            Entry e = Find(section, key);
            if (e == null) return def;
            long v;
            if (ParseNumber(e.Value, true, out v) && v >= 0 && v <= uint.MaxValue) return (uint)v;
            Warnings.Add("[" + section + "] " + key + " (line " + e.Line + "): '" + e.Value + "' is not a hex number, using 0x" + def.ToString("X"));
            return def;
        }

        // hex bytes: "31 01 FF 00" (also 0x31,0x01 or 3101FF00). An empty value gives an empty array.
        public byte[] GetBytes(string section, string key, byte[] def)
        {
            Entry e = Find(section, key);
            if (e == null) return def;
            byte[] b;
            if (TryParseBytes(e.Value, out b)) return b;
            Warnings.Add("[" + section + "] " + key + " (line " + e.Line + "): '" + e.Value + "' are not hex bytes, using the default");
            return def;
        }

        // list of hex numbers separated by blanks or commas: "F180 F181 F18C"
        public List<ushort> GetHexList(string section, string key, List<ushort> def)
        {
            Entry e = Find(section, key);
            if (e == null) return def;
            List<ushort> list = new List<ushort>();
            string[] parts = e.Value.Split(new char[] { ' ', ',', '\t' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string p in parts)
            {
                long v;
                if (!ParseNumber(p, true, out v) || v < 0 || v > 0xFFFF)
                {
                    Warnings.Add("[" + section + "] " + key + " (line " + e.Line + "): '" + p + "' is not a 16-bit hex number, list ignored");
                    return def;
                }
                list.Add((ushort)v);
            }
            return list;
        }

        // ------------------------------------------------------------ parsing helpers (also used elsewhere)
        public static bool ParseNumber(string text, bool hexByDefault, out long value)
        {
            value = 0;
            string t = text.Trim();
            if (t.Length == 0) return false;
            bool hex = hexByDefault;
            if (t.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) { hex = true; t = t.Substring(2); }
            if (t.Length == 0) return false;
            return long.TryParse(t, hex ? NumberStyles.AllowHexSpecifier : NumberStyles.Integer,
                                 CultureInfo.InvariantCulture, out value);
        }

        public static bool TryParseBytes(string text, out byte[] bytes)
        {
            List<byte> list = new List<byte>();
            string[] parts = text.Split(new char[] { ' ', ',', '\t' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string raw in parts)
            {
                string p = raw;
                if (p.StartsWith("0x", StringComparison.OrdinalIgnoreCase)) p = p.Substring(2);
                if (p.Length == 0 || p.Length % 2 != 0) { bytes = null; return false; }
                for (int i = 0; i < p.Length; i += 2)
                {
                    byte b;
                    if (!byte.TryParse(p.Substring(i, 2), NumberStyles.AllowHexSpecifier, CultureInfo.InvariantCulture, out b))
                    {
                        bytes = null;
                        return false;
                    }
                    list.Add(b);
                }
            }
            bytes = list.ToArray();
            return true;
        }

        // ------------------------------------------------------------ typing mistakes
        // keys that were never read: unknown name in the file
        public List<string> Unused()
        {
            List<string> result = new List<string>();
            foreach (KeyValuePair<string, List<Entry>> s in sections)
                foreach (Entry e in s.Value)
                    if (!e.Used) result.Add("[" + s.Key + "] " + e.Key + " (line " + e.Line + ")");
            return result;
        }
    }
}
