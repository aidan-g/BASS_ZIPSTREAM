using NUnit.Framework;
using System;
using System.IO;

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
            var index = Utils.GetEntryIndex(archiveName, entryPath);

            if (!Bass.Init(Bass.DefaultDevice))
            {
                Assert.Fail(string.Format("Failed to initialize BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
            }

            if (!BassZipStream.Init())
            {
                Assert.Fail("Failed to initialize ZIPSTREAM.");
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
                if (!BassZipStream.Free())
                {
                    Assert.Fail(string.Format("Failed to free ZIPSTREAM: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }

                if (!Bass.Free())
                {
                    Assert.Fail(string.Format("Failed to free BASS: {0}", Enum.GetName(typeof(Errors), Bass.LastError)));
                }
            }
        }
    }
}
