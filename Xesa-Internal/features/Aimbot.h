#pragma once

#include "../utils/Singleton.h"
#include "../SDK/CUserCmd.h"
#include "../utils/Math.h"
#include "../SDK/Studio.h"
#include "../SDK/Entities.h"
#include <vector>

struct AimbotData 
{
	Vector aimpoint;
	C_BasePlayer* player;

	AimbotData(Vector point, C_BasePlayer* _player) {
		aimpoint = point;
		player = _player;
	}
};

class Aimbot : public Singleton<Aimbot> {
public:
	void AddHitpoints(std::vector<AimbotData>&, C_BasePlayer*);
	void Run(CUserCmd*, bool&);
};