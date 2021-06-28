#pragma once

#include "bass_zipstream.h"
#include "ArchiveEntryInterface.h"
#include "Common.h"
#include "ErrorInterface.h"

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
		InitConfig();
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
			//Open the entry.
			ARCHIVE_ENTRY_HANDLE* handle;
			if (!ARCHIVE_OpenEntry(file, index, &handle)) {
				return 0;
			}
			//Ensure the minimum buffer percent is available.
			DWORD min;
			if (!BASS_ZIPSTREAM_GetConfig(ZS_BUFFER_MIN, &min)) {
				min = DEFAULT_BUFFER_MIN;
			}
			FLOAT factor = (FLOAT)min / 100;
			if (factor > 0) {
				if (factor > 1) {
					factor = 1;
				}
				QWORD position = (QWORD)((FLOAT)ARCHIVE_GetEntryLength(handle) * factor);
				ARCHIVE_BufferEntry(position, handle);
			}
			//Determine whether we should double buffer (using the BASS stream buffer).
			DWORD doubleBuffer;
			if (!BASS_ZIPSTREAM_GetConfig(ZS_DOUBLE_BUFFER, &doubleBuffer)) {
				doubleBuffer = DEFAULT_DOUBLE_BUFFER;
			}
			DWORD system;
			if (doubleBuffer) {
				system = STREAMFILE_BUFFER;
			}
			else {
				system = STREAMFILE_NOBUFFER;
			}
			//Attempt to create the stream, if this fails ARCHIVE_CloseEntry will be called immidiately.
			return BASS_StreamCreateFileUser(system, flags, &procs, handle);
		}
		catch (CSystemException e) {
			ARCHIVE_SetLastError(e.ErrorCode);
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