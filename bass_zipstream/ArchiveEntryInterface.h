#pragma once

#include "../bass/bass.h"

#ifndef ARCHIVEDEF
#define ARCHIVEDEF(f) WINAPI f
#endif

#if __cplusplus
extern "C" {
#endif

	typedef struct ARCHIVE_ENTRY_HANDLE {
		void* archive;
		void* entry;
	} ARCHIVE_ENTRY_HANDLE;

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_OpenEntry)(const void* file, DWORD index, BOOL overwrite, ARCHIVE_ENTRY_HANDLE** handle);

	__declspec(dllexport) QWORD ARCHIVEDEF(ARCHIVE_GetEntryPosition)(void* user);

	__declspec(dllexport) QWORD ARCHIVEDEF(ARCHIVE_GetEntryLength)(void* user);

	__declspec(dllexport) QWORD ARCHIVEDEF(ARCHIVE_GetEntryAvailable)(void* user);

	__declspec(dllexport) DWORD ARCHIVEDEF(ARCHIVE_ReadEntry)(void* buffer, DWORD length, void* user);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_SeekEntry)(QWORD offset, void* user);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_IsEOF)(void* user);

	_declspec(dllexport) VOID ARCHIVEDEF(ARCHIVE_CloseEntry)(void* user);

#if __cplusplus
}
#endif