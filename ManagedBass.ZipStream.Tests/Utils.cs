using NUnit.Framework;
using System;
using System.IO;

namespace ManagedBass.ZipStream.Tests
{
    public static class Utils
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Utils).Assembly.Location);

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
                Archive.Close(archive);
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
    }
}
