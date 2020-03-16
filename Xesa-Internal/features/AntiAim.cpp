#include "AntiAim.h"

#include "../Config.h"
#include "../Interfaces.h"

#include "../features/WallHelper.h"

//pasta from Elysium
bool AntiAim::Freestanding(CUserCmd* cmd)
{
	std::vector<C_BasePlayer*> enemies;

	for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++) {
		IClientEntity* clientEntity = g_EntityList->GetClientEntity(i);

		if (!clientEntity || clientEntity == g_LocalPlayer)
			continue;

		C_BaseEntity* baseEntity = clientEntity->GetBaseEntity();
		if (!baseEntity || baseEntity->IsDormant() || !baseEntity->IsPlayer() || baseEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		C_BasePlayer* basePlayer = (C_BasePlayer*)baseEntity;

		if (basePlayer->m_lifeState() != LIFE_ALIVE)
			continue;

		enemies.push_back(basePlayer);
	}

	auto eyepos = g_LocalPlayer->GetEyePos();
	auto origin = g_LocalPlayer->m_vecOrigin();
	auto headpos = g_LocalPlayer->GetHitboxPos(0);

	auto checkWallThickness = [&](C_BasePlayer* pPlayer, Vector newhead) -> float
	{
		Ray_t ray;
		trace_t trace1, trace2;
		Vector endpos1, endpos2;
		Vector eyepos = pPlayer->GetEyePos();
		CTraceFilterSkipTwoEntities filter(pPlayer, g_LocalPlayer);

		ray.Init(newhead, eyepos);
		g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace1);

		if (trace1.DidHit())
			endpos1 = trace1.endpos;
		else
			return 0.f;

		ray.Init(eyepos, newhead);
		g_EngineTrace->TraceRay(ray, MASK_SHOT_BRUSHONLY, &filter, &trace2);

		if (trace2.DidHit())
			endpos2 = trace2.endpos;

		float add = newhead.DistTo(eyepos) - g_LocalPlayer->GetEyePos().DistTo(eyepos) + 3.f;
		return endpos1.DistTo(endpos2) + add / 3;
	};

	Vector besthead;
	float bestrotation = 0.f;
	float highestthickness = 0.f;
	float step = M_PI * 2.0 / 15;
	float radius = Vector(headpos - origin).Length2D();

	for (float rotation = 0; rotation < (M_PI * 2.0); rotation += step)
	{
		float totalthickness = 0.f;
		Vector newhead(radius * cos(rotation) + eyepos.x, radius * sin(rotation) + eyepos.y, eyepos.z);

		for (auto current : enemies)
			totalthickness += checkWallThickness(current, newhead);

		if (totalthickness > highestthickness)
		{
			highestthickness = totalthickness;
			bestrotation = rotation;
			besthead = newhead;
		}
	}

	if (highestthickness != 0)
	{
		cmd->viewangles.yaw = RAD2DEG(bestrotation);
		return true;
	}
	return false;
}

void AntiAim::Run(CUserCmd* cmd, bool& bSendPacket)
{
	if (!config.aim_antiaim_enable || !g_LocalPlayer || !g_LocalPlayer->IsAlive() || cmd->buttons & IN_USE)
		return;

	if ((cmd->buttons & IN_ATTACK || (cmd->buttons & IN_ATTACK2 /*TODO: Check for resolver*/)) && g_LocalPlayer->CanShoot())
		return;

	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	if (!config.misc_lc_fakelag) {
		bSendPacket = g_GlobalVars->tickcount % 2;
	}
	cmd->viewangles.pitch = 89.f;

	static float lastReal = 0.f;

	if (!bSendPacket) {
		cmd->viewangles.yaw -= 180.f;
		Freestanding(cmd);
		lastReal = cmd->viewangles.yaw;
	}
	else {
		cmd->viewangles.yaw = lastReal - g_LocalPlayer->GetMaxDesyncAngle();
	}
}
