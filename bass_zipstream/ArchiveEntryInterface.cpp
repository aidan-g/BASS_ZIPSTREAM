#include "Archive.h"
#include "ArchiveEntry.h"
#include "ArchiveEntryInterface.h"
#include "Common.h"

extern "C" {

#define BUFFER_TIMEOUT 1000

	BOOL ARCHIVEDEF(ARCHIVE_OpenEntry)(const void* file, DWORD index, BOOL overwrite, ARCHIVE_ENTRY_HANDLE** handle) {

		*handle = (ARCHIVE_ENTRY_HANDLE*)malloc(sizeof(ARCHIVE_ENTRY_HANDLE));
		if (!*handle) {
			//TODO: Warn.
			return FALSE;
		}

		Archive* archive = nullptr;
		ArchiveEntry* entry = nullptr;
		try {
			archive = new Archive();
			archive->Open(UString((const wchar_t*)file));
			entry = archive->OpenEntry(index, overwrite);
			(*handle)->archive = archive;
			(*handle)->entry = entry;
			return TRUE;
		}
		catch (CSystemException e) {
			if (*handle) {
				free(*handle);
			}
			if (entry) {
				delete entry;
			}
			if (archive) {
				delete archive;
			}
			//TODO: Warn.
			return FALSE;
		}
	}

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryPosition)(void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->GetPosition();
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return 0;
		}
	}

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryLength)(void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->GetSize();
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return 0;
		}
	}

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryAvailable)(void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->GetAvailable();
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return 0;
		}
	}

	DWORD ARCHIVEDEF(ARCHIVE_ReadEntry)(void* buffer, DWORD length, void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			DWORD count = entry->Read(buffer, length);
			if (!count) {
				QWORD position = entry->GetPosition();
				QWORD size = entry->GetSize();
				if (position < size) {
					position += length;
					if (position > size) {
						position = size;
					}
					entry->Buffer(position, BUFFER_TIMEOUT);
					count = entry->Read(buffer, length);
				}
			}
			return count;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return 0;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_SeekEntry)(QWORD offset, void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			if (!entry->Seek(offset)) {
				QWORD size = entry->GetSize();
				if (offset <= size) {
					entry->Buffer(offset, BUFFER_TIMEOUT);
					return entry->Seek(offset);
				}
			}
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_IsEOF)(void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->GetPosition() >= entry->GetSize();
		}
		catch (CSystemException e) {
			//TODO: Warn.
		}
	}

	VOID ARCHIVEDEF(ARCHIVE_CloseEntry)(void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			Archive* archive = (Archive*)handle->archive;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			archive->CloseEntry(entry);
			archive->Close();
			delete archive;
			free(handle);
		}
		catch (CSystemException e) {
			//TODO: Warn.
		}
	}
}