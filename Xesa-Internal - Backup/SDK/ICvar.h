#pragma once

#include "../vfunc.h"
#include "ConVar.h"

class ICvar {
public:
	ConVar* FindVar(const char* var_name) {
		return CallVFunction<ConVar*, const char*>(this, 15, var_name);
	}
};