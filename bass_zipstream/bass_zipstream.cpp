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

#define MAX_CONFIGS 10

	static DWORD config[MAX_CONFIGS] = { 0 };

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
		is_initialized = TRUE;
		return TRUE;
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_SetConfig)(ZS_ATTRIBUTE attrib, DWORD value) {
		config[attrib] = value;
		return TRUE;
	}

	BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_GetConfig)(ZS_ATTRIBUTE attrib, DWORD* value) {
		if (*value = config[attrib]) {
			return TRUE;
		}
		return FALSE;
	}

	HSTREAM BASSZIPSTREAMDEF(BASS_ZIPSTREAM_StreamCreateFile)(BOOL mem, const void* file, DWORD index, QWORD offset, QWORD length, DWORD flags) {
		try {
			ARCHIVE_ENTRY_HANDLE* handle;
			if (!ARCHIVE_OpenEntry(file, index, &handle)) {
				//TODO: Warn.
				return 0;
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