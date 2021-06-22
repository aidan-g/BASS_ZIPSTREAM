using NUnit.Framework;
using System;
using System.IO;

namespace ManagedBass.ZipStream.Tests
{
    [TestFixture]
    public class ArchiveErrorTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveTests).Assembly.Location);

        [Test]
        public void Test001()
        {
            var fileName = Path.Combine(Location, "Media", "Missing.zip");

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            try
            {
                Assert.IsFalse(Archive.Open(archive, fileName));
                Assert.AreEqual(ArchiveError.E_FILE_NOT_FOUND, ArchiveError.GetLastError());
            }
            finally
            {
                Archive.Release(archive);
            }
        }
    }
}
