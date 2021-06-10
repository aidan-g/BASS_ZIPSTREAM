#include "Archive.h"

#include "../7z/CPP/7zip/Common/FileStreams.h"

ArchiveEntry::ArchiveEntry(Archive* parent, int index) {
	this->Parent = parent;
	this->Index = index;
	this->Size = 0;
}

void ArchiveEntry::Open() {
	this->Parent->GetEntry(this->Path, this->Size, this->Index);
	this->Extract();
}

void ArchiveEntry::Extract() {
	this->Parent->ExtractEntry(this->InStream, this->Task, this->Index);
}

UInt64 ArchiveEntry::GetPosition() {
	UInt64 position;
	if (this->InStream->Seek(0, STREAM_SEEK_CUR, &position) != S_OK) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	return position;
}

UInt64 ArchiveEntry::GetSize() {
	return this->Size;
}

UInt64 ArchiveEntry::GetAvailable() {
	UInt64 available;
	if (this->Task->IsRunning(this->Index, available)) {
		return available;
	}
	return this->Size;
}

UInt32 ArchiveEntry::Read(void* buffer, UInt32 length) {
	UInt32 count;
	if (this->InStream->Read(buffer, length, &count)) {
		//TODO: Warn.
		throw CSystemException(S_FALSE);
	}
	return count;
}

bool ArchiveEntry::Buffer(UInt64 position, UInt32 timeout) {
	if (position > this->Size) {
		return false;
	}
	for (unsigned a = 0; a < timeout; a++) {
		if (this->GetAvailable() >= position) {
			return true;
		}
		Sleep(1);
	}
	return false;
}

bool ArchiveEntry::Seek(UInt64 position) {
	if (position > this->GetAvailable()) {
		return false;
	}
	if (this->InStream->Seek(position, STREAM_SEEK_SET, nullptr) == S_OK) {
		return true;
	}
	//TODO: Warn.
	return false;
}

void ArchiveEntry::Close() {
	if (this->Task) {
		this->Task->Cancel();
	}
	if (this->InStream) {
		//TODO: Close input stream.
	}
}