#include "ArchiveExtractCallback.h"

#include "Archive.h"

#include "../7z/CPP/Common/IntToString.h"
#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/7zip/Common/FileStreams.h"

ArchiveExtractCallback::ArchiveExtractCallback(Archive* parent, bool overwrite) {
	this->Parent = parent;
	this->Overwrite = overwrite;
}

bool ArchiveExtractCallback::GetTempFileName(UString& path, int index) {
	if (!NWindows::NFile::NDir::MyGetTempPath(path)) {
		return false;
	}
	UString fileName;
	if (!this->Parent->IsOpen(fileName)) {
		return false;
	}
	UInt64 hashCode = 7;
	for (int a = 0; a < fileName.Len(); a++) {
		hashCode = 31 * hashCode + fileName[a] + index;
	}
	char temp[32];
	ConvertUInt64ToString(hashCode, temp);
	path += L"bass_zipstream.";
	path += UString(temp).Left(8);
	path += L".tmp";
	return true;
}

void ArchiveExtractCallback::CloseFiles() {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile file = this->Files[a];
		if (!file.Stream) {
			continue;
		}
		COutFileStream* stream = (COutFileStream*)(ISequentialOutStream*)file.Stream;
		stream->Close();
	}
}

bool ArchiveExtractCallback::GetPath(UString& path, int index) {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile file = this->Files[a];
		if (file.Index == index) {
			path = file.Path;
			return true;
		}
	}
	return false;
}

STDMETHODIMP ArchiveExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode) {

	//Empty outStream means skip.
	*outStream = nullptr;

	UString path;
	if (!this->GetTempFileName(path, index)) {
		//TODO: Warn.
		return S_FALSE;
	}

	ArchiveExtractFile file = ArchiveExtractFile();
	file.Path = path;
	file.Index = index;

	if (!this->Overwrite) {
		NWindows::NFile::NFind::CFileInfo fileInfo;
		if (fileInfo.Find(path)) {
			//File exists and not overwriting, nothing to do.
			this->Files.Add(file);
			return S_OK;
		}
	}

	COutFileStream* fileStream = new COutFileStream();

	if (!fileStream->Open(path, CREATE_ALWAYS)) {
		//TODO: Warn.
		return S_FALSE;
	}

	file.Stream = fileStream;
	this->Files.Add(file);

	CMyComPtr<ISequentialOutStream> ptr = fileStream;
	*outStream = ptr.Detach();

	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::PrepareOperation(Int32 askExtractMode) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetOperationResult(Int32 opRes) {
	this->CloseFiles();
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetTotal(UInt64 total) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetCompleted(const UInt64* completeValue) {
	return S_OK;
}