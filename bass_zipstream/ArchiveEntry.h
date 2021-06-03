#pragma once

#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/Windows/FileIO.h"

class Archive;
class ArchiveExtractTask;

class ArchiveEntry
{
public:
	ArchiveEntry(Archive* parent, int index, bool overwrite);

	void Open();

	UInt64 GetPosition();

	UInt64 GetSize();

	UInt64 GetAvailable();

	UInt32 Read(void* buffer, UInt32 length);

	bool Buffer(UInt64 position, UInt32 timeout);

	bool Seek(UInt64 position);

	void Close();

private:
	Archive* Parent;
	int Index;
	UString Path;
	UInt64 Size;
	bool Overwrite;

	CMyComPtr<IInStream> InStream;
	ArchiveExtractTask* Task;

	void Extract();
};
