#pragma once

#include "../utils/Singleton.h"
#include "../SDK/CUserCmd.h"

class AntiAim : public Singleton<AntiAim> {
public:
	bool Freestanding(CUserCmd*);
	void Run(CUserCmd*, bool&);
};