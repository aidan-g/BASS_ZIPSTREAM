﻿using NUnit.Framework;
using System;
using System.IO;

namespace ManagedBass.ZipStream.Tests
{
    [TestFixture(true)]
    [TestFixture(false)]
    public class ArchiveEntryTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveEntryTests).Assembly.Location);

        public ArchiveEntryTests(bool cleanup)
        {
            if (cleanup)
            {
                Assert.IsTrue(Archive.Cleanup());
            }
        }

        [TestCase("Music.7z", "Gift\\01 Smile.flac", 27873249, 811163365)]
        [TestCase("Music.7z", "Gift\\02 Again & Again.flac", 30222116, 519855218)]
        [TestCase("Music.7z", "Gift\\03 Emotional Times.flac", 23088352, 1409150913)]
        [TestCase("Music.tar", "Gift\\01 Smile.flac", 27873249, 811163365)]
        [TestCase("Music.tar", "Gift\\02 Again & Again.flac", 30222116, 519855218)]
        [TestCase("Music.tar", "Gift\\03 Emotional Times.flac", 23088352, 1409150913)]
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

        [TestCase("Music.zip", "Gift\\01 Smile.flac", 27873249)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", 30222116)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", 23088352)]
        public void Test002(string archiveName, string entryPath, long length)
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
                    if (!ArchiveEntry.OpenEntry(fileName, b, out entry))
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

        [TestCase("Music.zip")]
        public void Test004(string archiveName)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var count = Utils.GetEntryCount(fileName);

            for (var a = 0; a < count; a++)
            {
                var entry = default(IntPtr);
                if (!ArchiveEntry.OpenEntry(fileName, a, out entry))
                {
                    Assert.Fail("Failed to open entry.");
                }

                try
                {
                    var buffer = new byte[10240];
                    Assert.Greater(ArchiveEntry.ReadEntry(entry, buffer, buffer.Length), 0);
                }
                finally
                {
                    ArchiveEntry.CloseEntry(entry);
                }
            }
        }



        [TestCase("Music (Protected).zip", "Gift\\01 Smile.flac", "password", 27873249, 811163365)]
        [TestCase("Music (Protected).zip", "Gift\\02 Again & Again.flac", "password", 30222116, 519855218)]
        [TestCase("Music (Protected).zip", "Gift\\03 Emotional Times.flac", "password", 23088352, 1409150913)]
        public void Test005(string archiveName, string entryPath, string password, long length, int hashCode)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var index = Utils.GetEntryIndex(archiveName, entryPath);

            Utils.PasswordHandler.SetPassword(fileName, password);

            var entry = default(IntPtr);
            if (!ArchiveEntry.OpenEntry(fileName, index, out entry))
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
    }
}