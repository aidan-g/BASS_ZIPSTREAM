#pragma once

#include "../bass/bass.h"

#ifndef ARCHIVEDEF
#define ARCHIVEDEF(f) WINAPI f
#endif

#if __cplusplus
extern "C" {
#endif

	typedef struct ARCHIVE_FORMAT {
		wchar_t name[50];
		wchar_t extensions[MAX_PATH];
	} ARCHIVE_FORMAT;

	typedef struct ARCHIVE_ENTRY {
		wchar_t path[MAX_PATH];
		QWORD size;
	} ARCHIVE_ENTRY;

	typedef struct ARCHIVE_PASSWORD {
		wchar_t path[MAX_PATH];
		wchar_t password[50];
	}ARCHIVE_PASSWORD;

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_Create)(void** instance);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_GetFormatCount)(void* instance, DWORD* count);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_GetFormat)(void* instance, ARCHIVE_FORMAT* format, DWORD index);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_Open)(void* instance, const void* file);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_GetEntryCount)(void* instance, DWORD* count);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_GetEntry)(void* instance, ARCHIVE_ENTRY* entry, DWORD index);

	__declspec(dllexport) VOID ARCHIVEDEF(ARCHIVE_Close)(void* instance);

	__declspec(dllexport) VOID ARCHIVEDEF(ARCHIVE_Release)(void* instance);

	__declspec(dllexport) BOOL ARCHIVEDEF(ARCHIVE_Cleanup)();

	_declspec(dllexport) VOID ARCHIVEDEF(ARCHIVE_GetPassword)(BOOL(*prompt)(ARCHIVE_PASSWORD* password));

#if __cplusplus
}
#endif