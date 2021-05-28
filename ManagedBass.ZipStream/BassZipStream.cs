using System;
using System.Runtime.InteropServices;

namespace ManagedBass.ZipStream
{
    public static class BassZipStream
    {
        const string DllName = "bass_zipstream";

        public static bool Overwrite
        {
            get
            {
                var overwrite = default(bool);
                if (!GetConfig(BassZipStreamAttribute.Overwrite, out overwrite))
                {
                    return false;
                }
                return overwrite;
            }
            set
            {
                SetConfig(BassZipStreamAttribute.Overwrite, value);
            }
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_Init();

        /// <summary>
        /// Initialize.
        /// </summary>
        /// <returns></returns>
        public static bool Init()
        {
            return BASS_ZIPSTREAM_Init();
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_Free();

        /// <summary>
        /// Free.
        /// </summary>
        /// <returns></returns>
        public static bool Free()
        {
            return BASS_ZIPSTREAM_Free();
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute Attrib, out bool Value);

        public static bool GetConfig(BassZipStreamAttribute Attrib, out bool Value)
        {
            return BASS_ZIPSTREAM_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute Attrib, out int Value);

        public static bool SetConfig(BassZipStreamAttribute Attrib, out int Value)
        {
            return BASS_ZIPSTREAM_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute Attrib, bool Value);

        public static bool SetConfig(BassZipStreamAttribute Attrib, bool Value)
        {
            return BASS_ZIPSTREAM_SetConfig(Attrib, Value);
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute Attrib, int Value);

        public static bool SetConfig(BassZipStreamAttribute Attrib, int Value)
        {
            return BASS_ZIPSTREAM_SetConfig(Attrib, Value);
        }

        [DllImport(DllName, CharSet = CharSet.Unicode)]
        static extern int BASS_ZIPSTREAM_StreamCreateFile(bool mem, string file, int index, long offset, long length, BassFlags flags);

        public static int CreateStream(string File, int Index, long Offset = 0, long Length = 0, BassFlags Flags = BassFlags.Default)
        {
            return BASS_ZIPSTREAM_StreamCreateFile(false, File, Index, Offset, Length, Flags | BassFlags.Unicode);
        }
    }

    public enum BassZipStreamAttribute : byte
    {
        None = 0,
        Overwrite = 1
    }
}
