#pragma once

#include "../vfunc.h"
#include "BaseHandle.h"
#include "IHandleEntity.h"

class IClientEntity;

class IClientEntityList {
	
public:
	IClientEntity* GetClientEntity(int entnum) {
		return CallVFunction<IClientEntity*, int>(this, 3, entnum);
	}

	IClientEntity* GetClientEntityFromHandle(CBaseHandle handle) {
		return CallVFunction<IClientEntity*, CBaseHandle>(this, 4, handle);
	}

	int GetHighestEntityIndex() {
		return CallVFunction<int>(this, 6);
	}
};