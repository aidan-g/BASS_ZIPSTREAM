#pragma once

#include "Interfaces.h"

class Archive;

class ArchiveExtractFile :
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP;

	UString Path;
	UInt64 Size;
	CMyComPtr<IInStream> InStream;
	CMyComPtr<IOutStream> OutStream;
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

	bool GetTempFileName(UString& path, UInt32 index);

	bool GetEntrySize(UInt64& size, UInt32 index);

	bool OpenFile(UInt32 index);

	bool OpenInputFile(ArchiveExtractFile* file);

	bool OpenOutputFile(ArchiveExtractFile* file);

	void CloseFiles();

public:
	MY_QUERYINTERFACE_BEGIN2(IArchiveExtractCallback)
		MY_QUERYINTERFACE_END
		MY_ADDREF_RELEASE
		INTERFACE_IArchiveExtractCallback(;)

public:
	ArchiveExtractCallback(Archive* parent, bool overwrite);

	bool OpenFiles(const UInt32* indices, UInt32 count);

	bool GetInStream(CMyComPtr<IInStream>& stream, int index);

	bool GetOutStream(CMyComPtr<IOutStream>& stream, int index);
};

