#pragma once

#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/Windows/FileIO.h"

class Archive;

class ArchiveEntry
{
public:
	ArchiveEntry(Archive* parent, int index);

	void Open();

	UInt64 GetSize();

	UInt32 Read(void* buffer, UInt32 length);

	bool Seek(UInt64 position);

	void Close();

private:
	Archive* Parent;
	int Index;

	UString FileName;
	CMyComPtr<IInStream> InStream;

	void Extract();

	void CreateInStream();
};
