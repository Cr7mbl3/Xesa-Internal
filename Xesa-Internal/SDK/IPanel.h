#pragma once

#include "../vfunc.h"

class IPanel
{
public:
	const char* GetName(unsigned int vguiPanel)
	{
		return CallVFunction<const char*, unsigned int>(this, 36, vguiPanel);
	}
};