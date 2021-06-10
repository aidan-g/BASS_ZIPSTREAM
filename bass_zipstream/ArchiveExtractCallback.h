#pragma once

#include "Interfaces.h"
#include "CFileStream.h"

class Archive;

class ArchiveExtractFile :
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP;

	UString Path;
	UInt64 Size;
	CMyComPtr<CFileStream> Stream;
	UInt32 Index;
};

class ArchiveExtractCallback :
	public IArchiveExtractCallback,
	public CMyUnknownImp
{
private:
	Archive* Parent;
	bool Overwrite;

	CObjectVector<CMyComPtr<ArchiveExtractFile>> Files;

	void ForEachFile(bool (*action)(ArchiveExtractFile* file));

	bool GetTempFileName(UString& path, UInt32 index);

	bool OpenFile(UInt32 index);

	bool OpenFile(ArchiveExtractFile* file, bool overwrite);

public:
	MY_QUERYINTERFACE_BEGIN2(IArchiveExtractCallback)
		MY_QUERYINTERFACE_END
		MY_ADDREF_RELEASE
		INTERFACE_IArchiveExtractCallback(;)

public:
	ArchiveExtractCallback(Archive* parent, bool overwrite);

	bool OpenFiles(const UInt32* indices, UInt32 count);

	void CloseReaders();

	void CloseWriters();

	void Close();

	bool GetInStream(CMyComPtr<IInStream>& stream, int index);

	bool GetOutStream(CMyComPtr<IOutStream>& stream, int index);
};

