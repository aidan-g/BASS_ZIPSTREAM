#pragma once

#include "Interfaces.h"

class Archive;

struct ArchiveExtractFile {
	UString Path;
	CMyComPtr<ISequentialOutStream> Stream;
	UInt32 Index;
};

class ArchiveExtractCallback :
	public IArchiveExtractCallback,
	public CMyUnknownImp
{
private:
	Archive* Parent;
	bool Overwrite;

	CObjectVector<ArchiveExtractFile> Files;

	bool GetTempFileName(UString& path, int index);

	void CloseFiles();

public:
	MY_QUERYINTERFACE_BEGIN2(IArchiveExtractCallback)
		MY_QUERYINTERFACE_END
		MY_ADDREF_RELEASE
		INTERFACE_IArchiveExtractCallback(;)

public:
	ArchiveExtractCallback(Archive* parent, bool overwrite);

	bool GetPath(UString& path, int index);
};

