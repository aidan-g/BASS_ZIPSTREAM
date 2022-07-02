# BASS_ZIPSTREAM

A BASS plugin which creates a stream from an archive entry.

bass.dll is required for native projects.
ManagedBass is required for .NET projects.

A simple example;

```c#
var sourceChannel = BassZipStream.CreateStream("Music.zip", 0); //Create a stream for the first file in Music.zip.

Bass.ChannelPlay(sourceChannel);

while (Bass.ChannelIsActive(sourceChannel) == PlaybackState.Playing)
{
    Thread.Sleep(1000);
}

Bass.StreamFree(sourceChannel);
```

Some archive reading framework is provided, password protection is supported.
See the test suite.