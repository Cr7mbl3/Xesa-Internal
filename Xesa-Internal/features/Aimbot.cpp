#include "Aimbot.h"

#include <iostream>

#include "WallHelper.h"
#include "LagCompensation.h"

#include "../Interfaces.h"
#include "../Config.h"

//https://github.com/monthyx1337/GamSensFinal
float Hitchance(C_BaseCombatWeapon* weapon)
{
	if (config.aim_ragebot_hitchance > 0.f)
	{
		float inaccuracy = weapon->GetInaccuracy();
		if (inaccuracy == 0.f)
			inaccuracy = 0.0000001f;
		return 1.f / inaccuracy;
	}
	return 101.f;
}

void Aimbot::AddHitpoints(std::vector<AimbotData>& vec, C_BasePlayer* basePlayer)
{

	matrix3x4_t matrix[MAXSTUDIOBONES];

	if (!basePlayer->SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
		return;

	studiohdr_t* hdr = g_ModelInfo->GetStudiomodel(basePlayer->GetModel());
	if (!hdr)
		return;

	mstudiohitboxset_t* hitboxSet = hdr->GetHitboxSet(basePlayer->m_nHitboxSet());
	if (!hitboxSet)
		return;

	for (int hitbox = HITBOX_HEAD; hitbox < HITBOX_MAX; hitbox++) 
	{
		mstudiobbox_t* untransformedBox = hitboxSet->GetHitbox(hitbox);
		if (!untransformedBox)
			continue;

		float mod = untransformedBox->m_flRadius != -1.f ? untransformedBox->m_flRadius : 0.f;
		Vector min, max;
		Math::VectorTransform(untransformedBox->bbmax + mod, matrix[untransformedBox->bone], max);
		Math::VectorTransform(untransformedBox->bbmax - mod, matrix[untransformedBox->bone], min);

		auto center = (min + max) * 0.5f;
		QAngle curAngles = Math::DirectionToAngle(center - g_LocalPlayer->GetEyePos());
		Vector forward;
		Math::AngleVectors(curAngles, forward);

		Vector right = forward.Cross(Vector(0, 0, 1));
		Vector left = Vector(-right.x, -right.y, right.z);
		Vector top = Vector(0, 0, 1);
		Vector bottom = Vector(0, 0, -1);

		vec.push_back(AimbotData( center, basePlayer ));
		vec.push_back(AimbotData( right * (untransformedBox->m_flRadius * 0.6f), basePlayer ));
		vec.push_back(AimbotData( left * (untransformedBox->m_flRadius * 0.6f), basePlayer ));
	}
}

void Aimbot::Run(CUserCmd* cmd, bool& bSendPacket)
{
	if (!config.aim_ragebot_enable || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*) g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hActiveWeapon());
	
	if (!weapon || weapon->IsKnife())
		return;

	if (!g_LocalPlayer->CanShoot())
		return;

	if (Hitchance(weapon) < config.aim_ragebot_hitchance)
		return;

	static std::vector<AimbotData> data;

	for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++) {
		IClientEntity* clientEntity = g_EntityList->GetClientEntity(i);

		if (!clientEntity || clientEntity == g_LocalPlayer)
			continue;

		C_BaseEntity* baseEntity = clientEntity->GetBaseEntity();
		if (!baseEntity || baseEntity->IsDormant() || !baseEntity->IsPlayer() || baseEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;

		C_BasePlayer* basePlayer = (C_BasePlayer*) baseEntity;

		if (basePlayer->m_lifeState() != LIFE_ALIVE || basePlayer->m_bGunGameImmunity())
			continue;

		AddHitpoints(data, basePlayer);
	}

	for (AimbotData data : data)
	{
		Vector point = data.aimpoint;

		if (point.IsZero())
			continue;

		if (WallHelper::IsBulletDamageOverMinDamage(g_LocalPlayer, data.player, point, weapon->GetCSWpnData(), config.aim_ragebot_min_damage)) {
			cmd->buttons |= IN_ATTACK;

			Vector src = g_LocalPlayer->m_vecOrigin() + g_LocalPlayer->m_vecViewOffset();
			QAngle angle = Math::DirectionToAngle(point - src);
			//static ConVar* weapon_recoil_scale = g_Cvar->FindVar("weapon_recoil_scale");
			cmd->viewangles = angle;
			//cmd->viewangles -= g_LocalPlayer->m_aimPunchAngle() * weapon_recoil_scale->GetFloat();
			cmd->viewangles -= g_LocalPlayer->m_aimPunchAngle() * 2.f;

			float time = data.player->m_flSimulationTime() + LagCompensation::Get().GetLerpTime();
			if (LagCompensation::Get().IsValid(time))
				cmd->tick_count = TIME_TO_TICKS(time);

			bSendPacket = false;
			return;
		}
	}
	data.clear();
}
