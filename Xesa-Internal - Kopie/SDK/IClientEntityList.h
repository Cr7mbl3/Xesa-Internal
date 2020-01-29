#pragma once

#include "../vfunc.h"

class IClientEntity;

class IClientEntityList {
	
public:
	IClientEntity* GetClientEntity(int entnum) {
		return CallVFunction<IClientEntity*, int>(this, 3, entnum);
	}

	int GetHighestEntityIndex() {
		return CallVFunction<int>(this, 6);
	}
};