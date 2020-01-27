#pragma once

#include "ClientClass.h"
#include "../vfunc.h"

class IBaseClientDLL
{
public:
	ClientClass* GetAllClasses(void) 
	{
		return CallVFunction<ClientClass*>(this, 8);
	}

	bool DispatchUserMessage(int messageType, int arg, int arg1, void* data)
	{
		return CallVFunction<bool, int, int, int, void*>(this, 38, messageType, arg, arg1, data);
	}

};