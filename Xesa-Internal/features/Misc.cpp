#include "Misc.h"

#include "../Interfaces.h"

#include "../SDK/CUserCmd.h"
#include "../SDK/Entities.h"

namespace Misc {

	void SpoofSvCheats(int value)
	{
		static ConVar* sv_cheats = Interfaces::Get().Cvar->FindVar("sv_cheats");
		sv_cheats->SetValue(1);
	}

	void GrenadePrediction()
	{
		static ConVar* cl_grenadepreview = Interfaces::Get().Cvar->FindVar("cl_grenadepreview");
		cl_grenadepreview->SetValue(1);
	}

	void SniperCrosshair()
	{
		static ConVar* weapon_debug_spread_show = Interfaces::Get().Cvar->FindVar("weapon_debug_spread_show");
		weapon_debug_spread_show->SetValue(3);
	}

	void RecoilCrosshair() {
		static ConVar * cl_crosshair_recoil = Interfaces::Get().Cvar->FindVar("cl_crosshair_recoil");
		cl_crosshair_recoil->SetValue(1);
	}

	void BunnyHop(CUserCmd* cmd)
	{
		C_LocalPlayer local = Interfaces::Get().LocalPlayer;
		if (local->m_fFlags() & (1 << 0) /*  ON_GROUND */)
			return;
		cmd->buttons &= ~IN_JUMP;
	}

}