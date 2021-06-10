#include "ArchiveExtractCallback.h"

#include "Archive.h"
#include "CFileStream.h"
#include "IClosable.h"

#include "../7z/CPP/Common/IntToString.h"
#include "../7z/CPP/Windows/FileDir.h"
#include "../7z/CPP/7zip/Common/FileStreams.h"

ArchiveExtractCallback::ArchiveExtractCallback(Archive* parent, bool overwrite) {
	this->Parent = parent;
	this->Overwrite = overwrite;
}

bool ArchiveExtractCallback::GetTempFileName(UString& path, UInt32 index) {
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

bool ArchiveExtractCallback::OpenFile(ArchiveExtractFile* file, bool overwrite) {
	CFileStream* fileStream = new CFileStream();
	if (overwrite) {
		if (!fileStream->Create(file->Path, file->Size)) {
			return false;
		}
	}
	else {
		if (!fileStream->Open(file->Path, file->Size)) {
			return false;
		}
	}

	file->Stream = fileStream;

	return true;
}

bool ArchiveExtractCallback::OpenFile(UInt32 index) {
	UString path;
	if (!this->GetTempFileName(path, index)) {
		return false;
	}

	ArchiveExtractFile* file = new ArchiveExtractFile();
	file->Path = path;
	file->Index = index;

	this->Parent->GetEntry(path, file->Size, index);

	bool overwrite = true;
	if (!this->Overwrite) {
		NWindows::NFile::NFind::CFileInfo fileInfo;
		if (fileInfo.Find(file->Path)) {
			overwrite = false;
		}
	}

	if (!this->OpenFile(file, overwrite)) {
		return false;
	}

	this->Files.Add(file);

	return true;
}

bool ArchiveExtractCallback::OpenFiles(const UInt32* indices, UInt32 count) {
	for (UInt32 a = 0; a < count; a++) {
		if (!this->OpenFile(indices[a])) {
			return false;
		}
	}
	return true;
}

void ArchiveExtractCallback::Close() {
	this->CloseReaders();
	this->CloseWriters();
}

void ArchiveExtractCallback::CloseReaders() {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Stream) {
			CMyComPtr<IInStream> stream;
			if (file->Stream->GetReader(stream)) {
				CMyComPtr<IClosable> closable;
				if (stream->QueryInterface(IID_IClosable, (void**)&closable) == S_OK) {
					closable->Close();
				}
			}
		}
	}
}

void ArchiveExtractCallback::CloseWriters() {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Stream) {
			CMyComPtr<IOutStream> stream;
			if (file->Stream->GetWriter(stream)) {
				CMyComPtr<IClosable> closable;
				if (stream->QueryInterface(IID_IClosable, (void**)&closable) == S_OK) {
					closable->Close();
				}
			}
		}
	}
}

bool ArchiveExtractCallback::GetInStream(CMyComPtr<IInStream>& stream, int index) {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Index == index) {
			if (!file->Stream) {
				return false;
			}
			return file->Stream->GetReader(stream);
		}
	}
	return false;
}

bool ArchiveExtractCallback::GetOutStream(CMyComPtr<IOutStream>& stream, int index) {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Index == index) {
			if (!file->Stream) {
				return false;
			}
			return file->Stream->GetWriter(stream);
		}
	}
	return false;
}

STDMETHODIMP ArchiveExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode) {

	//Empty outStream means skip.
	*outStream = nullptr;

	CMyComPtr<IOutStream> fileStream;
	if (this->GetOutStream(fileStream, index)) {
		*outStream = fileStream.Detach();
	}

	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::PrepareOperation(Int32 askExtractMode) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetOperationResult(Int32 opRes) {
	this->CloseWriters();
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetTotal(UInt64 total) {
	return S_OK;
}

STDMETHODIMP ArchiveExtractCallback::SetCompleted(const UInt64* completeValue) {
	return S_OK;
}