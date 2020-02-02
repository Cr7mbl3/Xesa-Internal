#include "LagCompensation.h"

#include <math.h>

#include "../Interfaces.h"
#include "../Config.h"

void LagCompensation::Update()
{
	Vector delta = g_LocalPlayer->m_angAbsOrigin() - m_vecLastSendPosition;
	m_flCurrentTeleportDistanceSqr = delta.length2D();
}

void LagCompensation::OnProcessCmd(CUserCmd* cmd, bool& bSendPacket)
{
	if (bSendPacket) {
		m_iChockedTicks = 0;
		m_vecLastSendPosition = g_LocalPlayer->m_angAbsOrigin();
		m_bChokedLast = false;
	}
	else {
		m_iChockedTicks++;
		m_bChokedLast = true;
	}
}

float LagCompensation::GetLerpTime()
{
	static ConVar* cl_interp = g_Cvar->FindVar("cl_interp");
	static ConVar* cl_updaterate = g_Cvar->FindVar("cl_updaterate");
	static ConVar* cl_interp_ratio = g_Cvar->FindVar("cl_interp_ratio");
	static ConVar* sv_maxupdaterate = g_Cvar->FindVar("sv_maxupdaterate");
	static ConVar* sv_minupdaterate = g_Cvar->FindVar("sv_minupdaterate");
	static ConVar* sv_client_min_interp_ratio = g_Cvar->FindVar("sv_client_min_interp_ratio");
	static ConVar* sv_client_max_interp_ratio = g_Cvar->FindVar("sv_client_max_interp_ratio");

	auto interp = cl_interp->GetFloat();
	auto updaterate = cl_updaterate->GetFloat();
	auto interp_ratio = cl_interp_ratio->GetFloat();
	auto maxupdaterate = sv_maxupdaterate->GetFloat();
	auto minupdaterate = sv_minupdaterate->GetFloat();
	auto client_min_interp_ratio = sv_client_min_interp_ratio->GetFloat();
	auto client_max_interp_ratio = sv_client_max_interp_ratio->GetFloat();

	if (client_max_interp_ratio > interp_ratio)
		interp_ratio = client_min_interp_ratio;

	if (interp_ratio > client_max_interp_ratio)
		interp_ratio = client_max_interp_ratio;

	if (maxupdaterate <= updaterate)
		updaterate = maxupdaterate;

	if (minupdaterate > updaterate)
		updaterate = minupdaterate;

	auto v20 = interp_ratio / updaterate;
	if (v20 <= interp)
		return interp;
	else
		return v20;
}

void LagCompensation::FakeLag(bool& bSendPacket)
{
	if (!g_LocalPlayer || !config.misc_fakelag)
		return;

	auto distance_per_tick = g_LocalPlayer->m_vecVelocity().length2D() * g_GlobalVars->interval_per_tick;
	int shouldChoke = min(std::ceilf( m_flMaxTeleportDistanceSqr / max(1.f, distance_per_tick)), 15);

	if (g_LocalPlayer->m_lifeState() == 0 && m_iChockedTicks <= shouldChoke)
		bSendPacket = false;
}
