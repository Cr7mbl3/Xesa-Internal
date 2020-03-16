#pragma once

#include "../vfunc.h"
#include "ConVar.h"
#include "Color.h"

class ICvar {
public:
	ConVar* FindVar(const char* var_name) {
		return CallVFunction<ConVar*, const char*>(this, 15, var_name);
	}
	//GamSense
	void const ConsoleColorPrintf(const Color& clr, const char* pFormat, ...)
	{
		if (pFormat == nullptr)
			return;

		char buf[8192];

		va_list list;
		va_start(list, pFormat);
		_vsnprintf_s(buf, sizeof(buf) - 1, pFormat, list);
		va_end(list);
		buf[sizeof(buf) - 1] = 0;

		CallVFunction<void>(this, 25, clr, buf, list);
	}
};