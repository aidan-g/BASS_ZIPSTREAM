#include "Config.h"

static DWORD config[MAX_CONFIGS] = { 0 };

BOOL GetConfig(ZS_ATTRIBUTE attrib, DWORD* value) {
	if (*value = config[attrib]) {
		return TRUE;
	}
	return FALSE;
}

BOOL SetConfig(ZS_ATTRIBUTE attrib, DWORD value) {
	config[attrib] = value;
	return TRUE;
}