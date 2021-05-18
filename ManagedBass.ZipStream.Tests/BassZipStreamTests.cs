using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace ManagedBass.ZipStream.Tests
{
    [TestFixture]
    public class BassZipStreamTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveTests).Assembly.Location);

        [TestCase("Music.zip", "Gift\\01 Smile.flac")]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac")]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac")]
        public void Test001(string archiveName, string entryPath)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);
            var index = this.GetEntryIndex(archiveName, entryPath);

            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            try
            {
                var sourceChannel = BassZipStream.CreateStream(fileName, index);
                if (sourceChannel == 0)
                {
                    Assert.Fail(string.Format("Failed to create source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!Bass.ChannelPlay(sourceChannel))
                {
                    Assert.Fail(string.Format("Failed to play the source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                //Play for a bit.
                global::System.Threading.Thread.Sleep(10000);

                if (!Bass.ChannelSetPosition(sourceChannel, Bass.ChannelGetLength(sourceChannel) - Bass.ChannelSeconds2Bytes(sourceChannel, 10)))
                {
                    Assert.Fail(string.Format("Failed to seek the source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                //Play for a bit.
                global::System.Threading.Thread.Sleep(10000);

                if (!Bass.StreamFree(sourceChannel))
                {
                    Assert.Fail(string.Format("Failed to free the source stream: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }
            }
            finally
            {
                if (!Bass.Free())
                {
                    Assert.Fail(string.Format("Failed to free BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }
            }
        }

        private int GetEntryIndex(string archiveName, string entryPath)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            try
            {
                if (!Archive.Open(archive, fileName))
                {
                    Assert.Fail("Failed to open archive.");
                }

                var count = default(int);
                if (!Archive.GetEntryCount(archive, out count))
                {
                    Assert.Fail("Failed to get entry count.");
                }

                for (var a = 0; a < count; a++)
                {
                    var entry = default(ArchiveEntry);
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
            finally
            {
                Archive.Close(archive);
                Archive.Release(archive);
            }
        }
    }
}
