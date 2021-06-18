using NUnit.Framework;
using System;
using System.IO;
using System.Threading;

namespace ManagedBass.ZipStream.Tests
{
    public static class Utils
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Utils).Assembly.Location);

        public static int GetEntryCount(string archiveName)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            if (!Archive.Open(archive, fileName))
            {
                Assert.Fail("Failed to open archive.");
            }

            try
            {
                var count = default(int);
                if (!Archive.GetEntryCount(archive, out count))
                {
                    Assert.Fail("Failed to get entry count.");
                }
                return count;
            }
            finally
            {
                Archive.Release(archive);
            }
        }

        public static int GetEntryIndex(string archiveName, string entryPath)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            if (!Archive.Open(archive, fileName))
            {
                Assert.Fail("Failed to open archive.");
            }

            try
            {
                return GetEntryIndex(archive, entryPath);
            }
            finally
            {
                Archive.Release(archive);
            }
        }

        public static int GetEntryIndex(IntPtr archive, string entryPath)
        {
            var count = default(int);
            if (!Archive.GetEntryCount(archive, out count))
            {
                Assert.Fail("Failed to get entry count.");
            }

            for (var a = 0; a < count; a++)
            {
                var entry = default(Archive.ArchiveEntry);
                if (!Archive.GetEntry(archive, out entry, a))
                {
                    Assert.Fail("Failed to get entry.");
                }
                if (string.Equals(entry.path, entryPath, StringComparison.OrdinalIgnoreCase))
                {
                    return a;
                }
            }
            Assert.Fail("The expected entry was not found.");
            throw new NotImplementedException();
        }

        public static int GetEntryHashCode(IntPtr entry)
        {
            var hashCode = default(int);
            var buffer = new byte[10240];
            do
            {
                var count = ArchiveEntry.ReadEntry(entry, buffer, buffer.Length);
                if (count <= 0)
                {
                    break;
                }
                for (var a = 0; a < count; a++)
                {
                    unchecked
                    {
                        hashCode += buffer[a];
                    }
                }
            } while (true);
            return Math.Abs(hashCode);
        }

        public static int GetEntryHashCode2(IntPtr entry)
        {
            var hashCode = default(int);
            var buffer = new byte[10240];
            var length = (int)Math.Floor((float)buffer.Length / 2);
            do
            {
                var count = ArchiveEntry.ReadEntry(entry, buffer, length, length);
                if (count <= 0)
                {
                    break;
                }
                for (var a = 0; a < count; a++)
                {
                    unchecked
                    {
                        hashCode += buffer[length + a];
                    }
                }
            } while (true);
            return Math.Abs(hashCode);
        }

        public static class PasswordHandler
        {
            public static string FileName { get; private set; }

            public static string Password { get; private set; }

            public static void Set(string fileName, string password)
            {
                FileName = fileName;
                Password = password;
                Archive.GetPassword(Get);
                //We need a shorter timeout as Sleep(1) can actually take much longer than 1ms.
                //This 100ms timeout combined with the 5000ms sleep in the password handler ensures all code is exercised.
                BassZipStream.BufferTimeout = 100;
            }

            public static bool Get(ref Archive.ArchivePassword password)
            {
                if (!string.Equals(password.path, FileName, StringComparison.OrdinalIgnoreCase))
                {
                    return false;
                }
                //This pause is long enough to cause the buffer operation to time out.
                Thread.Yield();
                Thread.Sleep(5000);
                Thread.Yield();
                password.password = Password;
                return !string.IsNullOrEmpty(password.password);
            }

            public static void Reset()
            {
                FileName = string.Empty;
                Password = string.Empty;
                Archive.GetPassword(null);
                BassZipStream.BufferTimeout = BassZipStream.DEFAULT_BUFFER_TIMEOUT;
            }
        }
    }
}
