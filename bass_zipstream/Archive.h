#pragma once

#include "Codecs.h"
#include "Interfaces.h"

#include "..\7z\CPP\7zip\UI\Common\LoadCodecs.h"

class ArchiveEntry;

class Archive
{
private:
	UString FileName;
	int FormatIndex;

	CCodecs* Codecs;
	CMyComPtr<IInStream> InStream;
	CMyComPtr<IInArchive> InArchive;

	void LoadCodecs();

	void LoadFormat();

	void CreateInStream();

	void CreateInArchive();

	void OpenInArchive();

	bool ReadProperty(UString& value, PROPID propID, int index);

	bool ReadProperty(UInt64& value, PROPID propID, int index);

public:
	Archive();

	int GetFormatCount();

	void GetFormat(UString& name, UString& extensions, int index);

	void Open(UString& fileName);

	bool IsOpen(UString& fileName);

	int GetEntryCount();

	void GetEntry(UString& path, int index);

	void ExtractEntry(IInStream** stream, int index, bool overwrite);

	ArchiveEntry* OpenEntry(int index, bool overwrite);

	void CloseEntry(ArchiveEntry* entry);

	void Close();
};

