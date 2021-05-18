#pragma once

#include "../7z/CPP/Common/MyException.h"

static void OK(HRESULT result) {
	if (result == S_OK) {
		return;
	}
	throw CSystemException(result);
}