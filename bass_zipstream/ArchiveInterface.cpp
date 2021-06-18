#include "Archive.h"
#include "ArchiveInterface.h"
#include "ArchiveExtractPrompt.h"
#include "Common.h"

#include <wchar.h>

extern "C" {

#define CLAMP(length, arr) min(length, sizeof(arr) / sizeof(arr[0]))

#define COPYSTRING(dst ,src) wmemcpy(dst, src, CLAMP(src.Len() + 1, dst))

	BOOL ARCHIVEDEF(ARCHIVE_Create)(void** instance) {
		try {
			*instance = new Archive();
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_GetFormatCount)(void* instance, DWORD* count) {
		try {
			Archive* archive = (Archive*)instance;
			*count = archive->GetFormatCount();
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_GetFormat)(void* instance, ARCHIVE_FORMAT* format, DWORD index) {
		try {
			Archive* archive = (Archive*)instance;
			UString name;
			UString extensions;
			archive->GetFormat(name, extensions, index);
			COPYSTRING(format->name, name);
			COPYSTRING(format->extensions, extensions);
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_Open)(void* instance, const void* file) {
		try {
			Archive* archive = (Archive*)instance;
			UString fileName = UString((const wchar_t*)file);
			archive->Open(fileName);
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_GetEntryCount)(void* instance, DWORD* count) {
		try {
			Archive* archive = (Archive*)instance;
			*count = archive->GetEntryCount();
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	BOOL ARCHIVEDEF(ARCHIVE_GetEntry)(void* instance, ARCHIVE_ENTRY* entry, DWORD index) {
		try {
			Archive* archive = (Archive*)instance;
			UString path;
			archive->GetEntry(path, entry->size, index);
			COPYSTRING(entry->path, path);
			return TRUE;
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	VOID ARCHIVEDEF(ARCHIVE_Close)(void* instance) {
		try {
			Archive* archive = (Archive*)instance;
			archive->Close();
		}
		catch (CSystemException e) {
			//TODO: Warn.
		}
	}

	VOID ARCHIVEDEF(ARCHIVE_Release)(void* instance) {
		ARCHIVE_Close(instance);
		delete instance;
	}

	BOOL ARCHIVEDEF(ARCHIVE_Cleanup)() {
		try {
			return Archive::Cleanup();
		}
		catch (CSystemException e) {
			//TODO: Warn.
			return FALSE;
		}
	}

	static BOOL(*__prompt1__)(ARCHIVE_PASSWORD* password) = NULL;

	static bool __prompt2__(UString fileName, UString& password) {
		if (!__prompt1__) {
			return false;
		}
		ARCHIVE_PASSWORD result = { 0 };
		COPYSTRING(result.path, fileName);
		if (!__prompt1__(&result)) {
			return false;
		}
		password = result.password;
		return true;
	}

	VOID ARCHIVEDEF(ARCHIVE_GetPassword)(BOOL(*prompt)(ARCHIVE_PASSWORD* password)) {
		__prompt1__ = prompt;
		ArchiveExtractPrompt::Handler = &__prompt2__;
	}
}