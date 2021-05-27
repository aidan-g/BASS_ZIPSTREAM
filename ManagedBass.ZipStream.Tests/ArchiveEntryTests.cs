using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ManagedBass.ZipStream.Tests
{
    [TestFixture]
    public class ArchiveEntryTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveEntryTests).Assembly.Location);

        [TestCase("Music.zip", "Gift\\01 Smile.flac", 27873249, 811163365)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", 30222116, 519855218)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", 23088352, 1409150913)]
        public void Test001(string archiveName, string entryPath, long length, int hashCode)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var index = Utils.GetEntryIndex(archiveName, entryPath);

            var entry = default(IntPtr);
            if (!ArchiveEntry.OpenEntry(fileName, index, out entry))
            {
                Assert.Fail("Failed to open entry.");
            }

            try
            {
                Assert.AreEqual(length, ArchiveEntry.GetEntryLength(entry));
                Assert.AreEqual(hashCode, Utils.GetEntryHashCode(entry));
            }
            finally
            {
                ArchiveEntry.CloseEntry(entry);
            }
        }
    }
}
