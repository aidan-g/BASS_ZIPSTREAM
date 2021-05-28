#include "Archive.h"
#include "ArchiveEntry.h"

#include "../7z/CPP/7zip/Common/FileStreams.h"

ArchiveEntry::ArchiveEntry(Archive* parent, int index, bool overwrite) {
	this->Parent = parent;
	this->Index = index;
	this->Overwrite = overwrite;
}

void ArchiveEntry::Open() {
	this->Extract();
	this->CreateInStream();
}

void ArchiveEntry::Extract() {
	this->Parent->ExtractEntry(this->FileName, this->Index, this->Overwrite);
}

void ArchiveEntry::CreateInStream() {
	CInFileStream* fileStream = new CInFileStream;
	this->InStream = fileStream;
	if (!fileStream->Open(this->FileName)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
}

UInt64 ArchiveEntry::GetSize() {
	CMyComPtr<IStreamGetSize> getSize;
	if (this->InStream->QueryInterface(IID_IStreamGetSize, (void**)&getSize) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	UInt64 size;
	if (getSize->GetSize(&size) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	return size;
}

UInt32 ArchiveEntry::Read(void* buffer, UInt32 length) {
	UInt32 count;
	if (this->InStream->Read(buffer, length, &count)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	return count;
}

bool ArchiveEntry::Seek(UInt64 position) {
	if (this->InStream->Seek(position, STREAM_SEEK_SET, nullptr) == S_OK) {
		return true;
	}
	//TODO: Warn.
	return false;
}

void ArchiveEntry::Close() {
	if (this->InStream) {
		//TODO: Close input stream.
	}
}