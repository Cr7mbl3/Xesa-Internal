#include "AntiAim.h"

#include "../Config.h"
#include "../Interfaces.h"

void AntiAim::Run(CUserCmd* cmd, bool& bSendPacket)
{
	if (!config.aim_antiaim_enable || !g_LocalPlayer || !g_LocalPlayer->IsAlive() || cmd->buttons & IN_USE)
		return;

	if ((cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2) && g_LocalPlayer->CanShoot())
		return;

	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP)
		return;

	cmd->viewangles.yaw -= 180.f;

	if (bSendPacket) {
		cmd->viewangles.yaw -= g_LocalPlayer->GetMaxDesyncAngle();
	}

	cmd->viewangles.pitch = 89.f;
}
