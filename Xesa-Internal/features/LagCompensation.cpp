#include "LagCompensation.h"

#include "../Interfaces.h"
#include "../Config.h"
#include "../utils/Math.h"
#include "../Hooks.h"
#include "../SDK/Entities.h"

void LagCompensation::Update()
{
	m_flCurrentTeleportDistanceSqr = g_LocalPlayer->m_angAbsOrigin().DistTo(m_vecLastSendPosition);
}

void LagCompensation::OnProcessCmd(CUserCmd* cmd, bool& bSendPacket)
{
	if (m_iChokedTicks >= 16)
		bSendPacket = true;

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

	//TODO: Fix UpdateLocalAnimationData();

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
			//UpdateAnimationData(basePlayer);

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
	/*CBaseAnimState* animstate = player->GetAnimState();
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

	player->SetupBones(nullptr, -1, 0x7FF00, g_GlobalVars->curtime);*/
	CBaseAnimState* animstate = player->GetAnimState();

	if (!animstate)
		return;

	auto curtime = g_GlobalVars->curtime;
	auto frametime = g_GlobalVars->frametime;
	auto absoluteframtime = g_GlobalVars->absoluteframetime;
	auto interp_amount = g_GlobalVars->interpolation_amount;
	auto framecount = g_GlobalVars->framecount;
	auto tickcount = g_GlobalVars->tickcount;
	auto realtime = g_GlobalVars->realtime;
	g_GlobalVars->realtime = player->m_flSimulationTime();
	g_GlobalVars->curtime = player->m_flSimulationTime();
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->absoluteframetime = g_GlobalVars->interval_per_tick;
	g_GlobalVars->framecount = TIME_TO_TICKS(player->m_flSimulationTime());
	g_GlobalVars->tickcount = TIME_TO_TICKS(player->m_flSimulationTime());
	g_GlobalVars->interpolation_amount = 0.0f;
	g_GlobalVars->curtime = player->m_flSimulationTime();
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	int backup_flags = player->m_fFlags();
	int backup_eflags = player->m_eFlags();

	player->m_absVelocity() = player->m_vecVelocity();

	//if (animstate->m_iLastClientSideAnimationUpdateFramecount >= TIME_TO_TICKS(player->m_flSimulationTime()))
		//animstate->m_iLastClientSideAnimationUpdateFramecount = TIME_TO_TICKS(player->m_flSimulationTime()) - 1;

	int last_update = animstate->m_iLastClientSideAnimationUpdateFramecount;
	if (last_update == g_GlobalVars->framecount)
		animstate->m_iLastClientSideAnimationUpdateFramecount = last_update - 1;

	player->m_bClientSideAnimation() = true;
	player->UpdateClientSideAnimation();
	player->m_bClientSideAnimation() = false;

	player->m_fFlags() = backup_flags;
	player->m_eFlags() = backup_eflags;

	g_GlobalVars->curtime = curtime;
	g_GlobalVars->frametime = frametime;
	g_GlobalVars->realtime = realtime;
	g_GlobalVars->curtime = curtime;
	g_GlobalVars->absoluteframetime = absoluteframtime;
	g_GlobalVars->interpolation_amount = interp_amount;
	g_GlobalVars->framecount = framecount;
	g_GlobalVars->tickcount = tickcount;

	player->InvalidateBoneCache();

	player->m_iLastSetupBonesFrame() = 0;

	player->SetupBones(nullptr, -1, 0x7FF00, g_GlobalVars->curtime);
}

void LagCompensation::UpdateLocalAnimationData()
{
	if (!g_LocalPlayer || !g_LocalPlayer->IsAlive()) 
		return;
	
	CBaseAnimState* animstate = g_LocalPlayer->GetAnimState();

	if (!animstate)
		return;
	
	float old_curtime = g_GlobalVars->curtime, old_frametime = g_GlobalVars->frametime;

	g_Prediction->RunCommand(g_LocalPlayer, Vars::lastCmd, g_MoveHelper);

	g_GlobalVars->curtime = g_GlobalVars->realtime * g_LocalPlayer->GetAnimationOverlays()[0].m_flWeight;

	if (g_LocalPlayer->m_fFlags() &= FL_ONGROUND) {
		g_LocalPlayer->GetAnimationOverlays()[6].m_flWeight = 0.f;
		//cheat::local->poseparam(ePoses::MOVE_YAW ) = 1.f / cheat::local->simulationtime();
	}

	//TODO: This is aids, pls fix modulu??
	float temp_curtime = g_GlobalVars->curtime;
	while (temp_curtime >= 2.f) {
		temp_curtime -= 2.f;
	}

	bool sometimes = temp_curtime == 0.f;


	//g_GlobalVars->frametime = sometimes ? (cheat::local->poseparam(ePoses::BODY_YAW) * cheat::globals->tick_interval) : (g_GlobalVars->tickbase() * g_GlobalVars->tick_interval);

	float corrected_abs_yaw = animstate->m_flAbsRotation(); //TODO: THIS SHOULD BE OLD ROTATION

	// Always negate the old position.
	if (corrected_abs_yaw > 0.f)
		corrected_abs_yaw = -corrected_abs_yaw;

	// The clients abs yaw is desynced when the yaw is at a certain position at a certain time.
	// We correct it by negating the old value which is negated when required, to mimic server functionality.
	//TODO: Idk why this module thingy doesn't work if ((g_LocalPlayer->m_angEyeAngles().yaw >= -45.f && g_LocalPlayer->m_angEyeAngles().yaw <= 45.f) && ((g_GlobalVars->curtime % 2.f) == 0.f))
	if ((g_LocalPlayer->m_angEyeAngles().yaw >= -45.f && g_LocalPlayer->m_angEyeAngles().yaw <= 45.f) && sometimes)
		g_LocalPlayer->SetAbsAngles(QAngle(0.f, animstate->m_flAbsRotation() - corrected_abs_yaw, 0.f));

	// Rebuilding bones with corrected values, we'll need to do this twice.
	// Once here, and once after updating bones. This is to maintain correct shootpos.
	matrix3x4_t temp_matrix[126];
	g_LocalPlayer->SetupBones(temp_matrix, g_LocalPlayer->m_iBoneCount(), 0x00000100, g_GlobalVars->curtime);

	// Forcing the client to update anims under all circumstances.
	//animstate->last_update_framecount = 0.f;

	// Update clientside anims.
	g_LocalPlayer->m_bClientSideAnimation() = true;
	g_LocalPlayer->UpdateClientSideAnimation();
	g_LocalPlayer->m_bClientSideAnimation() = false;

	// Explained above.
	g_LocalPlayer->SetupBones(temp_matrix, 126 /*126 instead?*/, 0x00000100, g_GlobalVars->curtime);

	// Restore values to not mess with the game.
	g_GlobalVars->curtime = old_curtime;
	g_GlobalVars->frametime = old_frametime;
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
