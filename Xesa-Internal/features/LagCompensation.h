#pragma once

#include "../utils/Singleton.h"
#include "../SDK/Entities.h"
#include "../Interfaces.h"

class LagCompensation : public Singleton<LagCompensation> {
private:
	float m_flMaxTeleportDistanceSqr = 64.f;
	float m_flCurrentTeleportDistanceSqr = 0.f;
	int m_iChockedTicks;
	Vector m_vecLastSendPosition;
	bool m_bChokedLast;
public:
	void Update();
	void OnProcessCmd(CUserCmd* cmd, bool& bSendPacket);
	float GetLerpTime();

	void FakeLag(bool& bSendPacket);
};