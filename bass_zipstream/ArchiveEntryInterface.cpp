#include "Archive.h"
#include "ArchiveEntry.h"
#include "ArchiveEntryInterface.h"
#include "Common.h"

extern "C" {

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

	DWORD ARCHIVEDEF(ARCHIVE_ReadEntry)(void* buffer, DWORD length, void* user) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->Read(buffer, length);
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
			return entry->Seek(offset);
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
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