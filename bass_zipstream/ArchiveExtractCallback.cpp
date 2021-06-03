#include "ArchiveExtractCallback.h"

#include "Archive.h"

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

bool ArchiveExtractCallback::OpenInputFile(ArchiveExtractFile* file) {
	CInFileStream* fileStream = new CInFileStream();
	if (!fileStream->File.OpenShared(file->Path, true)) {
		return false;
	}

	file->InStream = fileStream;

	return true;
}

bool ArchiveExtractCallback::OpenOutputFile(ArchiveExtractFile* file) {
	COutFileStream* fileStream = new COutFileStream();
	if (!fileStream->Open(file->Path, CREATE_ALWAYS)) {
		return false;
	}

	file->OutStream = fileStream;

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

	if (!this->Overwrite) {
		NWindows::NFile::NFind::CFileInfo fileInfo;
		if (fileInfo.Find(path)) {
			if (this->OpenInputFile(file)) {
				this->Files.Add(file);
				return true;
			}
		}
	}

	if (!this->OpenOutputFile(file)) {
		return false;
	}

	if (!this->OpenInputFile(file)) {
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

void ArchiveExtractCallback::CloseFiles() {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->OutStream) {
			COutFileStream* fileStream = (COutFileStream*)(IOutStream*)file->OutStream;
			fileStream->Close();
		}
	}
}

bool ArchiveExtractCallback::GetInStream(IInStream** stream, int index) {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Index == index) {
			if (!file->InStream) {
				return false;
			}
			*stream = file->InStream;
			return true;
		}
	}
	return false;
}

bool ArchiveExtractCallback::GetOutStream(IOutStream** stream, int index) {
	for (int a = 0; a < this->Files.Size(); a++) {
		ArchiveExtractFile* file = this->Files[a];
		if (file->Index == index) {
			if (!file->OutStream) {
				return false;
			}
			*stream = file->OutStream;
			return true;
		}
	}
	return false;
}

STDMETHODIMP ArchiveExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode) {

	//Empty outStream means skip.
	*outStream = nullptr;

	CMyComPtr<IOutStream> fileStream;
	if (this->GetOutStream(&fileStream, index)) {
		*outStream = fileStream.Detach();
	}

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