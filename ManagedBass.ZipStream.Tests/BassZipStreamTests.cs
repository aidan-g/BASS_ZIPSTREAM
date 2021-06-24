using NUnit.Framework;
using System;
using System.IO;

namespace ManagedBass.ZipStream.Tests
{
    [Explicit]
    [TestFixture(true)]
    [TestFixture(false)]
    public class BassZipStreamTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveTests).Assembly.Location);

        public BassZipStreamTests(bool cleanup)
        {
            this.Cleanup = cleanup;
        }

        public bool Cleanup { get; private set; }

        [SetUp]
        public void SetUp()
        {
            if (this.Cleanup)
            {
                Assert.IsTrue(Archive.Cleanup());
            }
        }

        [TearDown]
        public void TearDown()
        {
            Utils.PasswordHandler.Reset();
        }

        [Explicit]
        [TestCase("Music.zip", "Gift\\01 Smile.flac", 37573200)]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac", 41630400)]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac", 32281200)]
        public void Test001(string archiveName, string entryPath, long length)
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

                Assert.AreEqual(length, Bass.ChannelGetLength(sourceChannel));

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
                while (Bass.ChannelIsActive(sourceChannel) == PlaybackState.Playing)
                {
                    global::System.Threading.Thread.Sleep(1000);
                }

                //TODO: Not working, reported position is greater than the length?
                //Assert.AreEqual(length, Bass.ChannelGetPosition(sourceChannel));

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

        [Explicit]
        [TestCase("Music (Protected).zip", "Gift\\01 Smile.flac", "")]
        [TestCase("Music (Protected).zip", "Gift\\01 Smile.flac", "wrong")]
        [TestCase("Music (Protected).zip", "Gift\\02 Again & Again.flac", "")]
        [TestCase("Music (Protected).zip", "Gift\\02 Again & Again.flac", "wrong")]
        [TestCase("Music (Protected).zip", "Gift\\03 Emotional Times.flac", "")]
        [TestCase("Music (Protected).zip", "Gift\\03 Emotional Times.flac", "wrong")]
        public void Test002(string archiveName, string entryPath, string password)
        {
            if (!this.Cleanup)
            {
                Assert.Ignore("Requires clean state.");
            }

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

            if (string.IsNullOrEmpty(password))
            {
                Utils.PasswordHandler.Reset();
            }
            else
            {
                Utils.PasswordHandler.Set(fileName, password, 5000);
            }

            try
            {
                var sourceChannel = BassZipStream.CreateStream(fileName, index);
                Assert.AreEqual(0, sourceChannel);
                Assert.AreEqual(ArchiveError.E_PASSWORD_REQUIRED, ArchiveError.GetLastError());
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
