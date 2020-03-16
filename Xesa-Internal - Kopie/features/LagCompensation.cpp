#include "LagCompensation.h"

#include "../Interfaces.h"
#include "../Config.h"
#include "../utils/Math.h"

void LagCompensation::Update()
{
	m_flCurrentTeleportDistanceSqr = g_LocalPlayer->m_angAbsOrigin().DistTo(m_vecLastSendPosition);
}

void LagCompensation::OnProcessCmd(CUserCmd* cmd, bool& bSendPacket)
{
	if (bSendPacket) {
		m_iChokedTicks = 0;
		m_vecLastSendPosition = g_LocalPlayer->m_angAbsOrigin();
		m_bChokedLast = false;
	}
	else {
		m_iChokedTicks++;
		m_bChokedLast = true;
	}
}

//https://github.com/monthyx1337/GamSensFinal
void LagCompensation::OnFrameNetUpdateEnd()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++) {
		IClientEntity* clientEntity = g_EntityList->GetClientEntity(i);

		if (!clientEntity || clientEntity == g_LocalPlayer)
			continue;

		C_BaseEntity* baseEntity = clientEntity->GetBaseEntity();
		if (!baseEntity || baseEntity->IsDormant() || !baseEntity->IsPlayer())
			continue;

		C_BasePlayer* basePlayer = (C_BasePlayer*)baseEntity;

		if (basePlayer->m_lifeState() != LIFE_ALIVE)
			continue;

		PlayerLog_t* entry = &m_players.at(i - 1);

		entry->m_player = basePlayer;

		basePlayer->SetInterpolationFlag(!config.misc_lc_disable_interpolation);

		if(entry->m_records.empty() || basePlayer->m_flSimulationTime() != entry->m_records.front().m_simtime) {
			UpdateAnimationData(basePlayer);

			auto lag_record = LagRecord_t(basePlayer, entry->m_records);

			// only push valid records.
			if (lag_record.is_valid()) {
				entry->m_records.push_front(lag_record);
			}

			size_t size = entry->m_records.size();

			// too many records...
			if (size > m_max_records)
				entry->m_records.resize(m_max_records);
		}
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

bool LagCompensation::IsValid(float time)
{
	auto network = g_Engine->GetNetChannelInfo();
	if (!network)
		return false;
	static ConVar* max_unlag = g_Cvar->FindVar("sv_maxunlag");
	float serverTime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	auto delta = std::clamp(network->GetLatency(FLOW_OUTGOING) + network->GetLatency(FLOW_INCOMING) + GetLerpTime(), 0.f, max_unlag->GetFloat()) - (serverTime - time);
	return std::fabsf(delta) <= 0.2f;
}

void LagCompensation::UpdateAnimationData(C_BasePlayer* player) {
	CBaseAnimState* animstate = player->GetAnimState();
	if (!animstate)
		return;

	float backup_curtime = g_GlobalVars->curtime;
	float backup_frametime = g_GlobalVars->frametime;

	g_GlobalVars->curtime = player->m_flSimulationTime();
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	int backup_flags = player->m_fFlags();
	int backup_eflags = player->m_eFlags();

	player->m_absVelocity() = player->m_vecVelocity();

	int last_update = animstate->m_iLastClientSideAnimationUpdateFramecount;
	if (last_update == g_GlobalVars->framecount)
		animstate->m_iLastClientSideAnimationUpdateFramecount = last_update - 1;

	player->ClientSideAnimation() = true; {

		player->UpdateClientSideAnimation();

	} player->ClientSideAnimation() = false;

	player->m_fFlags() = backup_flags;
	player->m_eFlags() = backup_eflags;

	g_GlobalVars->curtime = backup_curtime;
	g_GlobalVars->frametime = backup_frametime;

	player->InvalidateBoneCache();

	player->m_iLastSetupBonesFrame() = 0;

	player->SetupBones(nullptr, -1, 0x7FF00, g_GlobalVars->curtime);
}

void LagCompensation::FakeLag(bool& bSendPacket)
{
	if (!config.misc_lc_fakelag || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
		return;

	//auto distance_per_tick = g_LocalPlayer->m_vecVelocity().length2D() * g_GlobalVars->interval_per_tick;
	//int shouldChoke = min(std::ceilf( m_flMaxTeleportDistanceSqr / max(1.f, distance_per_tick)), 15);
	int shouldChoke = 5;

	if (m_iChokedTicks <= shouldChoke)
		bSendPacket = false;
}

bool LagRecord_t::is_valid()
{
	return LagCompensation::Get().IsValid(m_simtime);
}
