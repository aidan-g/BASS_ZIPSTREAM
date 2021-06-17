#pragma once

#include "Config.h"

#include "../bass/bass.h"
#include "../bass/bass_addon.h"

#ifndef BASSZIPSTREAMDEF
#define BASSZIPSTREAMDEF(f) WINAPI f
#endif

#if __cplusplus
extern "C" {
#endif

	__declspec(dllexport) BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_Init)();

	__declspec(dllexport) BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_SetConfig)(ZS_ATTRIBUTE attrib, DWORD value);

	__declspec(dllexport) BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_GetConfig)(ZS_ATTRIBUTE attrib, DWORD* value);

	__declspec(dllexport) HSTREAM BASSZIPSTREAMDEF(BASS_ZIPSTREAM_StreamCreateFile)(BOOL mem, const void* file, DWORD index, QWORD offset, QWORD length, DWORD flags);

	__declspec(dllexport) BOOL BASSZIPSTREAMDEF(BASS_ZIPSTREAM_Free)();

#if __cplusplus
}
#endif