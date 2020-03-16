#pragma once

#include <array>
#include <deque>

#include "../Interfaces.h"

#include "../utils/Singleton.h"

#include "../SDK/Entities.h"
#include "../SDK/ClientFrameStage.h"

#define TIME_TO_TICKS(time) static_cast<int>(0.5f + time / g_GlobalVars->interval_per_tick)

//structs are pasta from deadcell
struct LagRecord_t {
	LagRecord_t() : m_bonecount{ 0 }, m_simtime{ 0.f }, m_lby{ 0.f }{ }

	LagRecord_t(C_BasePlayer* player, std::deque< LagRecord_t > records) : m_priority{ 0 } {

		if (!records.empty()) {
			m_prev_record = &records.front();
		}
		else {
			m_prev_record = nullptr;
		}

		std::memcpy(m_matrix, player->m_matBoneCache().m_pMemory, player->m_iBoneCount() * sizeof(matrix3x4_t));

		m_bonecount = player->m_iBoneCount();

		m_tickcount = g_GlobalVars->tickcount;

		// save all data here.
		m_simtime = player->m_flSimulationTime();
		m_origin = player->m_vecOrigin();
		m_angles = player->m_angEyeAngles();
		m_lby = player->m_flLowerBodyYawTarget();
		m_flags = player->m_fFlags();
		m_vel = player->m_vecVelocity();

		m_mins = player->GetCollideable()->OBBMins();
		m_maxs = player->GetCollideable()->OBBMaxs();
	}

	bool is_valid();

	int m_priority;

	int m_tickcount;
	int m_flags;
	int m_bonecount;

	float m_lby;
	float m_simtime;

	Vector m_origin;
	QAngle m_angles;
	Vector m_mins;
	Vector m_maxs;
	Vector m_vel;

	matrix3x4_t m_matrix[128];

	LagRecord_t* m_prev_record;
};

struct PlayerLog_t {
	C_BasePlayer* m_player;
	std::deque< LagRecord_t > m_records;
};

class LagCompensation : public Singleton<LagCompensation> {
private:
	float m_flMaxTeleportDistanceSqr = 64.f;
	float m_flCurrentTeleportDistanceSqr = 0.f;
	int m_iChokedTicks;
	Vector m_vecLastSendPosition;
	bool m_bChokedLast;
	size_t m_max_records{ 12 };
public:
	std::array< PlayerLog_t, 64 > m_players;
	void Update();
	void OnProcessCmd(CUserCmd* cmd, bool& bSendPacket);
	void OnFrameNetUpdateEnd();

	float GetLerpTime();
	bool IsValid(float time);

	void UpdateAnimationData(C_BasePlayer* player);

	void FakeLag(bool& bSendPacket);
};