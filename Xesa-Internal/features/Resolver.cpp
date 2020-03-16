#include "Resolver.h"

#include "../Interfaces.h"
#include "../Config.h"

void Resolver::ResolvePlayer(C_BasePlayer* player)
{
	//TODO: Add onshot resolve

	auto animState = player->GetAnimState();
	if (!animState)
		return;

	auto feetYaw = animState->m_flCurrentFeetYaw;
	//auto bodyMaxRotation = animState->m_flMaxBodyYaw();

	if (feetYaw <= 58 && feetYaw >= -58) {
		player->m_angEyeAngles().yaw -= player->GetMaxDesyncAngle();
	}
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
