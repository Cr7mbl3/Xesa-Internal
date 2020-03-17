#pragma once

#include <array>

#include "../utils/Singleton.h"
#include "../SDK/Entities.h"

struct ResolverInfo {
	float m_flSimulationTime;
	QAngle m_originalEyeAngles;
	QAngle m_resolvedEyeAngles;
	float m_flLowerBodyYawTarget;
	float m_flNextLbyUpdate;
	bool m_bFakeLagging;
};

class Resolver : public Singleton<Resolver> {
private:
	std::array<ResolverInfo, 65> m_info; //store resolverinfo for each player 16 ticks
public:
	void ResolvePlayer(C_BasePlayer* player);
	void ResolvePlayers();
};