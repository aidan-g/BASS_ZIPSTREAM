#include "Archive.h"
#include "ArchiveEntry.h"
#include "ArchiveEntryInterface.h"
#include "Config.h"
#include "Common.h"

extern "C" {

	DWORD GetTimeout() {
		DWORD timeout;
		if (!GetConfig(ZS_BUFFER_TIMEOUT, &timeout)) {
			timeout = DEFAULT_BUFFER_TIMEOUT;
		}
		return timeout;
	}

	BOOL ARCHIVEDEF(ARCHIVE_OpenEntry)(const void* file, DWORD index, ARCHIVE_ENTRY_HANDLE** handle) {

		if (!file || !handle) {
			return FALSE;
		}

		*handle = (ARCHIVE_ENTRY_HANDLE*)malloc(sizeof(ARCHIVE_ENTRY_HANDLE));
		if (!*handle) {
			//TODO: Warn.
			return FALSE;
		}

		Archive* archive = nullptr;
		ArchiveEntry* entry = nullptr;
		UString fileName = UString((const wchar_t*)file);
		try {
			archive = new Archive();
			archive->Open(fileName);
			entry = archive->OpenEntry(index);
			if (entry->GetSize()) {
				(*handle)->archive = archive;
				(*handle)->entry = entry;
				return TRUE;
			}
			else {
				//TODO: Warn, not a file.
			}
		}
		catch (CSystemException e) {
			//TODO: Warn.
		}
		if (*handle) {
			free(*handle);
			*handle = NULL;
		}
		if (entry) {
			delete entry;
		}
		if (archive) {
			delete archive;
		}
		return FALSE;
	}

	QWORD ARCHIVEDEF(ARCHIVE_GetEntryPosition)(void* user) {
		if (!user) {
			return 0;
		}
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
		if (!user) {
			return 0;
		}
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
		if (!user) {
			return 0;
		}
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

	BOOL ARCHIVEDEF(ARCHIVE_GetEntryResult)(HRESULT* result, void* user) {
		if (!result || !user) {
			return 0;
		}
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			if (entry->IsCompleted()) {
				*result = entry->GetResult();
				return TRUE;
			}
			else {
				*result = S_FALSE;
				return FALSE;
			}
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_BufferEntry)(QWORD position, void* user) {
		if (!user) {
			return FALSE;
		}
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->Buffer(position, GetTimeout());
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	DWORD ARCHIVEDEF(ARCHIVE_ReadEntry)(void* buffer, DWORD length, void* user) {
		return ARCHIVE_ReadEntry2(buffer, 0, length, user);
	}

	DWORD ARCHIVEDEF(ARCHIVE_ReadEntry2)(void* buffer, DWORD offset, DWORD length, void* user) {
		if (!buffer || !length || !user) {
			return 0;
		}
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			DWORD count = entry->Read(buffer, offset, length);
			if (!count) {
				QWORD position = entry->GetPosition();
				QWORD size = entry->GetSize();
				if (position < size) {
					position += length;
					if (position > size) {
						position = size;
					}
					entry->Buffer(position, GetTimeout());
					count = entry->Read(buffer, offset, length);
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
		if (!user) {
			return FALSE;
		}
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			if (!entry->Seek(offset)) {
				QWORD size = entry->GetSize();
				if (offset <= size) {
					entry->Buffer(offset, GetTimeout());
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
		if (!user) {
			return FALSE;
		}
		try {
			ARCHIVE_ENTRY_HANDLE* handle = (ARCHIVE_ENTRY_HANDLE*)user;
			ArchiveEntry* entry = (ArchiveEntry*)handle->entry;
			return entry->GetPosition() >= entry->GetSize();
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	VOID ARCHIVEDEF(ARCHIVE_CloseEntry)(void* user) {
		if (!user) {
			return;
		}
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