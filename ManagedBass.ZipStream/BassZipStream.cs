using System.Runtime.InteropServices;

namespace ManagedBass.ZipStream
{
    public static class BassZipStream
    {
        const string DllName = "bass_zipstream";

        /// <summary>
        /// //Buffer 20% before creating stream.
        /// </summary>
        public const int DEFAULT_BUFFER_MIN = 20;
        public const int DEFAULT_BUFFER_TIMEOUT = 1000;
        public const bool DEFAULT_DOUBLE_BUFFER = false;

        public static int BufferMin
        {
            get
            {
                var value = default(int);
                if (BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute.BufferMin, out value))
                {
                    return value;
                }
                return DEFAULT_BUFFER_MIN;
            }
            set
            {
                BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute.BufferMin, value);
            }
        }

        public static int BufferTimeout
        {
            get
            {
                var value = default(int);
                if (BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute.BufferTimeout, out value))
                {
                    return value;
                }
                return DEFAULT_BUFFER_TIMEOUT;
            }
            set
            {
                BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute.BufferTimeout, value);
            }
        }

        public static bool DoubleBuffer
        {
            get
            {
                var value = default(bool);
                if (BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute.DoubleBuffer, out value))
                {
                    return value;
                }
                return DEFAULT_DOUBLE_BUFFER;
            }
            set
            {
                BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute.DoubleBuffer, value);
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

        public static bool GetConfig(BassZipStreamAttribute Attrib, out int Value)
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
        BufferMin = 1,
        BufferTimeout = 2,
        DoubleBuffer = 3
    }
}
