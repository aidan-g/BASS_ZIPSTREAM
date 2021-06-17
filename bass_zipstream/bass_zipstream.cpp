#pragma once

#include "bass_zipstream.h"
#include "ArchiveEntryInterface.h"
#include "Common.h"

extern "C" {

	BASS_FILEPROCS procs = {
		ARCHIVE_CloseEntry,
		ARCHIVE_GetEntryLength,
		ARCHIVE_ReadEntry,
		ARCHIVE_SeekEntry
	};

	BOOL is_initialized = FALSE;

	//I have no idea how to prevent linking against this routine in msvcrt.
	//It doesn't exist on Windows XP.
	//Hopefully it doesn't do anything important.
	int _except_handler4_common() {
		return 0;
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_Init)() {
		if (is_initialized) {
			return FALSE;
		}
		BASS_ZIPSTREAM_SetConfig(ZS_BUFFER_MIN, DEFAULT_BUFFER_MIN);
		BASS_ZIPSTREAM_SetConfig(ZS_BUFFER_TIMEOUT, DEFAULT_BUFFER_TIMEOUT);
		is_initialized = TRUE;
		return TRUE;
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_GetConfig)(ZS_ATTRIBUTE attrib, DWORD* value) {
		return GetConfig(attrib, value);
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_SetConfig)(ZS_ATTRIBUTE attrib, DWORD value) {
		return SetConfig(attrib, value);
	}

	HSTREAM BASSZIPSTREAMDEF(BASS_ZIPSTREAM_StreamCreateFile)(BOOL mem, const void* file, DWORD index, QWORD offset, QWORD length, DWORD flags) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle;
			if (!ARCHIVE_OpenEntry(file, index, &handle)) {
				//TODO: Warn.
				return 0;
			}
			DWORD min;
			if (!BASS_ZIPSTREAM_GetConfig(ZS_BUFFER_MIN, &min)) {
				min = DEFAULT_BUFFER_MIN;
			}
			FLOAT factor = (FLOAT)min / 100;
			if (factor > 0) {
				if (factor > 1) {
					factor = 1;
				}
				QWORD position = ARCHIVE_GetEntryLength(handle) * factor;
				ARCHIVE_BufferEntry(position, handle);
			}
			return BASS_StreamCreateFileUser(STREAMFILE_BUFFER, flags, &procs, handle);
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return 0;
		}
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_Free)() {
		if (!is_initialized) {
			return FALSE;
		}
		is_initialized = FALSE;
		return TRUE;
	}
}