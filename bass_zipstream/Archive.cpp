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
		if (property.vt == VT_INT && property.intVal) {

		}
	}
	return false;
}

int Archive::GetEntryCount() {
	UInt32 count;
	if (this->InArchive->GetNumberOfItems(&count) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	return count;
}

void Archive::GetEntry(UString& path, int index) {
	if (!this->ReadProperty(path, kpidPath, index)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
}

void Archive::ExtractEntry(UString& path, int index) {
	const UInt32 indices[1] = {
		index
	};

	ArchiveExtractCallback* archiveExtractCallback = new ArchiveExtractCallback();

	CMyComPtr<IArchiveExtractCallback> ptr = archiveExtractCallback;
	if (this->InArchive->Extract(indices, 1, false, ptr.Detach()) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}

	if (!archiveExtractCallback->GetPath(path, index)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
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

void Archive::Close() {
	if (this->InArchive) {
		this->InArchive->Close();
	}
}