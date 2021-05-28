#pragma once

#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/Windows/FileIO.h"

class Archive;

class ArchiveEntry
{
public:
	ArchiveEntry(Archive* parent, int index, bool overwrite);

	void Open();

	UInt64 GetPosition();

	UInt64 GetSize();

	UInt32 Read(void* buffer, UInt32 length);

	bool Seek(UInt64 position);

	void Close();

private:
	Archive* Parent;
	int Index;
	bool Overwrite;

	UString FileName;
	CMyComPtr<IInStream> InStream;

	void Extract();

	void CreateInStream();
};
