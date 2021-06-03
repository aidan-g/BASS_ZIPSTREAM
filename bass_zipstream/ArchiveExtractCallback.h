#pragma once

#include "Interfaces.h"

class Archive;

struct ArchiveExtractFile {
	UString Path;
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

	CObjectVector<ArchiveExtractFile*> Files;

	bool GetTempFileName(UString& path, UInt32 index);

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

	bool GetInStream(IInStream** stream, int index);

	bool GetOutStream(IOutStream** stream, int index);
};

