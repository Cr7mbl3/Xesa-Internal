#include "Resolver.h"

#include <iostream>

#include "../Interfaces.h"
#include "../Config.h"

void Resolver::ResolvePlayer(C_BasePlayer* player)
{
	//TODO: Add onshot resolve

	ResolverInfo info = m_info[player->EntIndex()];
	info.m_originalEyeAngles = player->m_angEyeAngles();

	//check if lby updated
	if (player->m_flLowerBodyYawTarget() != info.m_flLowerBodyYawTarget || info.m_flNextLbyUpdate > g_GlobalVars->curtime) {

		float offset = player->m_vecVelocity().Length2D() > 0.1f ? 0.2f : 1.1f;
		info.m_flNextLbyUpdate = g_GlobalVars->curtime + offset;
		player->m_angEyeAngles().yaw = player->m_flLowerBodyYawTarget();
	}

	//TODO: add desync resolver

	info.m_bFakeLagging = info.m_flSimulationTime == player->m_flSimulationTime();
	info.m_flSimulationTime = player->m_flSimulationTime();
	info.m_flLowerBodyYawTarget = player->m_flLowerBodyYawTarget();
	info.m_resolvedEyeAngles = player->m_angEyeAngles();
	m_info[player->EntIndex()] = info;
}

void Resolver::ResolvePlayers()
{
	if (!config.aim_resolver_enable)
		return; 

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

		ResolvePlayer(basePlayer);
	}
}
