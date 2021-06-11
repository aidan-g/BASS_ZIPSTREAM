#include "Archive.h"
#include "ArchiveEntry.h"

#include "Common.h"
#include "ArchiveExtractCallback.h"
#include "IOUtils.h"

#include "../7z/CPP/7zip/Common/FileStreams.h"
#include "../7z/CPP/Windows/PropVariant.h"

Archive::Archive() {
	this->Codecs = new CCodecs();
	this->FormatIndex = -1;
	this->LoadCodecs();
}

void Archive::LoadCodecs() {
	OK(this->Codecs->Load());
}

void Archive::LoadFormat() {
	this->FormatIndex = this->Codecs->FindFormatForArchiveName(this->FileName);
	if (this->FormatIndex < 0) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
}

int Archive::GetFormatCount() {
	CObjectVector<CArcInfoEx> formats = this->Codecs->Formats;
	return formats.Size();
}

void Archive::GetFormat(UString& name, UString& extensions, int index) {
	CObjectVector<CArcInfoEx> formats = this->Codecs->Formats;
	CArcInfoEx& format = formats[index];
	name = format.Name;
	for (unsigned a = 0; a < format.Exts.Size(); a++) {
		CArcExtInfo& ext = format.Exts[a];
		if (a > 0) {
			extensions += ",";
		}
		extensions += ext.Ext;
	}
}

void Archive::Open(UString& fileName) {
	this->FileName = fileName;
	this->LoadFormat();
	this->CreateInStream();
	this->CreateInArchive();
	this->OpenInArchive();
}

void Archive::CreateInStream() {
	CInFileStream* fileStream = new CInFileStream;
	this->InStream = fileStream;
	if (!fileStream->Open(this->FileName)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
}

void Archive::CreateInArchive() {
	if (this->Codecs->CreateInArchive(this->FormatIndex, this->InArchive) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	if (!this->InArchive) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
}

void Archive::OpenInArchive() {
	UInt64 maxStartPosition = 1 << 23;
	if (this->InArchive->Open(this->InStream, &maxStartPosition, nullptr) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
}

bool Archive::ReadProperty(UString& value, PROPID propID, int index) {
	NWindows::NCOM::CPropVariant property;
	if (this->InArchive->GetProperty(index, propID, &property) == S_OK) {
		if (property.vt == VT_BSTR && property.bstrVal) {
			value.SetFromBstr(property.bstrVal);
			return true;
		}
		else if (property.vt == VT_EMPTY) {
			value.Empty();
			return true;
		}
	}
	return false;
}

bool Archive::ReadProperty(UInt64& value, PROPID propID, int index) {
	NWindows::NCOM::CPropVariant property;
	if (this->InArchive->GetProperty(index, propID, &property) == S_OK) {
		if (property.vt == VT_UI4 && property.ulVal) {
			value = property.ulVal;
			return true;
		}
		else if (property.vt == VT_UI8 && property.uhVal.QuadPart) {
			value = property.uhVal.QuadPart;
			return true;
		}
	}
	return false;
}

bool Archive::IsOpen(UString& fileName) {
	fileName = this->FileName;
	return !fileName.IsEmpty();
}

int Archive::GetEntryCount() {
	UInt32 count;
	if (this->InArchive->GetNumberOfItems(&count) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	return count;
}

void Archive::GetEntry(UString& path, UInt64& size, int index) {
	if (!this->ReadProperty(path, kpidPath, index)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	if (!this->ReadProperty(size, kpidSize, index)) {
		//Not a file.
		size = 0;
	}
}

void Archive::ExtractEntry(CMyComPtr<IInStream>& stream, CMyComPtr<ArchiveExtractTask>& task, int index) {
	const UInt32 indices[1] = {
		index
	};

	ArchiveExtractCallback* callback = new ArchiveExtractCallback(this);
	if (!callback->OpenFiles(indices, 1)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}

	if (!callback->GetInStream(stream, index)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}

	task = new ArchiveExtractTask(this->InArchive, callback);
	if (!task->Start(indices, 1)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}

	this->Tasks.Add(task);
}

ArchiveEntry* Archive::OpenEntry(int index) {
	ArchiveEntry* entry = new ArchiveEntry(this, index);
	entry->Open();
	return entry;
}

void Archive::CloseEntry(ArchiveEntry* entry) {
	entry->Close();
	delete entry;
}

void Archive::WaitForTasks() {
	for (unsigned a = 0; a < this->Tasks.Size(); a++) {
		this->Tasks[a]->Wait();
	}
	this->Tasks.Clear();
}

void Archive::Close() {
	this->WaitForTasks();
	if (this->InArchive) {
		this->InArchive->Close();
	}
	if (this->InStream) {
		//TODO: Close input stream.
	}
	this->FileName.Empty();
}

bool Archive::Cleanup() {
	return ArchiveExtractCallback::Cleanup();
}