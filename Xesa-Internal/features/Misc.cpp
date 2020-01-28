#include "Misc.h"

#include "../Interfaces.h"
#include "../Config.h"

#include "../SDK/CUserCmd.h"
#include "../SDK/Entities.h"

namespace Misc {

	void GrenadePrediction()
	{
		static ConVar* cl_grenadepreview = Interfaces::Get().Cvar->FindVar("cl_grenadepreview");
		cl_grenadepreview->SetValue(config.visual_grenadePrediction);
	}

	void SniperCrosshair()
	{
		static ConVar* weapon_debug_spread_show = Interfaces::Get().Cvar->FindVar("weapon_debug_spread_show");
		weapon_debug_spread_show->SetValue(config.visual_sniperCrosshair ? 3 : 0);
	}

	void RecoilCrosshair() {
		static ConVar * cl_crosshair_recoil = Interfaces::Get().Cvar->FindVar("cl_crosshair_recoil");
		cl_crosshair_recoil->SetValue(config.visual_recoilCrosshair);
	}

	void BunnyHop(CUserCmd* cmd)
	{
		C_LocalPlayer local = Interfaces::Get().LocalPlayer;
		if (!config.misc_bhop || local->m_fFlags() & (1 << 0) /*  ON_GROUND */)
			return;
		cmd->buttons &= ~IN_JUMP;
	}

}