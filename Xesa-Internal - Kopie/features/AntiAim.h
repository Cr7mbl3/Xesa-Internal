#pragma once

#include "../utils/Singleton.h"
#include "../SDK/CUserCmd.h"

class AntiAim : public Singleton<AntiAim> {
public:
	void Run(CUserCmd*, bool& bSendPacket);
};