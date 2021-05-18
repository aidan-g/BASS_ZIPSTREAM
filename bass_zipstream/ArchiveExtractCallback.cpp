#include "ArchiveExtractCallback.h"

#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/7zip/Common/FileStreams.h"

ArchiveExtractCallback::ArchiveExtractCallback() {

}

bool ArchiveExtractCallback::GetTempFileName(UString& path, int index) {
	if (!NWindows::NFile::NDir::MyGetTempPath(path)) {
		return false;
	}
	//TODO: Use hash from archive path and entry index.
	path += L"bass_zipstream";
	path += L".tmp";
	return true;
}

void ArchiveExtractCallback::CloseFiles() {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile file = this->Files[a];
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
	UString path;
	if (!this->GetTempFileName(path, index)) {
		//TODO: Warn.
		return S_FALSE;
	}

	COutFileStream* fileStream = new COutFileStream();

	if (!fileStream->Open(path, CREATE_ALWAYS)) {
		//TODO: Warn.
		return S_FALSE;
	}

	ArchiveExtractFile file = ArchiveExtractFile();
	file.Path = path;
	file.Stream = fileStream;
	file.Index = index;
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