using NUnit.Framework;
using System;
using System.IO;

namespace ManagedBass.ZipStream.Tests
{
    [TestFixture]
    public class ArchiveEntryTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveEntryTests).Assembly.Location);

        [TestCase("Music.zip", "Gift\\01 Smile.flac", true, 27873249, 811163365)]
        [TestCase("Music.zip", "Gift\\01 Smile.flac", false, 27873249, 811163365)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", true, 30222116, 519855218)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", false, 30222116, 519855218)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", true, 23088352, 1409150913)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", false, 23088352, 1409150913)]
        public void Test001(string archiveName, string entryPath, bool overwrite, long length, int hashCode)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var index = Utils.GetEntryIndex(archiveName, entryPath);

            var entry = default(IntPtr);
            if (!ArchiveEntry.OpenEntry(fileName, index, overwrite, out entry))
            {
                Assert.Fail("Failed to open entry.");
            }

            try
            {
                Assert.AreEqual(0, ArchiveEntry.GetEntryPosition(entry));
                Assert.AreEqual(length, ArchiveEntry.GetEntryLength(entry));
                Assert.AreEqual(hashCode, Utils.GetEntryHashCode(entry));
                Assert.AreEqual(length, ArchiveEntry.GetEntryPosition(entry));
                Assert.IsTrue(ArchiveEntry.IsEOF(entry));
            }
            finally
            {
                ArchiveEntry.CloseEntry(entry);
            }
        }

        [TestCase("Music.zip", "Gift\\01 Smile.flac", true, 27873249)]
        [TestCase("Music.zip", "Gift\\01 Smile.flac", false, 27873249)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", true, 30222116)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", false, 30222116)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", true, 23088352)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", false, 23088352)]
        public void Test002(string archiveName, string entryPath, bool overwrite, long length)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var index = Utils.GetEntryIndex(archiveName, entryPath);

            var entry = default(IntPtr);
            if (!ArchiveEntry.OpenEntry(fileName, index, overwrite, out entry))
            {
                Assert.Fail("Failed to open entry.");
            }

            try
            {
                Assert.AreEqual(0, ArchiveEntry.GetEntryPosition(entry));
                Assert.IsTrue(ArchiveEntry.SeekEntry(entry, length));
                Assert.AreEqual(length, ArchiveEntry.GetEntryPosition(entry));
                Assert.IsTrue(ArchiveEntry.IsEOF(entry));
            }
            finally
            {
                ArchiveEntry.CloseEntry(entry);
            }
        }

        [TestCase("Music.zip", 100)]
        public void Test003(string archiveName, int iterations)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var count = Utils.GetEntryCount(fileName);

            for (var a = 0; a < iterations; a++)
            {
                for (var b = 0; b < count; b++)
                {
                    var entry = default(IntPtr);
                    if (!ArchiveEntry.OpenEntry(fileName, b, a % 2 == 0, out entry))
                    {
                        Assert.Fail("Failed to open entry.");
                    }

                    try
                    {
                        Utils.GetEntryHashCode(entry);
                        Assert.IsTrue(ArchiveEntry.IsEOF(entry));
                    }
                    finally
                    {
                        ArchiveEntry.CloseEntry(entry);
                    }
                }
            }
        }
    }
}