#include "Misc.h"

#include <math.h>

#include "../Interfaces.h"
#include "../Config.h"

#include "../SDK/CUserCmd.h"
#include "../SDK/Entities.h"

namespace Misc {

	void GrenadePrediction()
	{
		static ConVar* cl_grenadepreview = g_Cvar->FindVar("cl_grenadepreview");
		if(config.visual_grenadePrediction != cl_grenadepreview->GetBool())
			cl_grenadepreview->SetValue(config.visual_grenadePrediction);
	}

	void SniperCrosshair()
	{
		static ConVar* weapon_debug_spread_show = g_Cvar->FindVar("weapon_debug_spread_show");
		int expected = config.visual_sniperCrosshair ? 2 : 0;
		if (expected != weapon_debug_spread_show->GetInt())
			weapon_debug_spread_show->SetValue(expected);
	}

	void RecoilCrosshair() {
		static ConVar* cl_crosshair_recoil = g_Cvar->FindVar("cl_crosshair_recoil");
		cl_crosshair_recoil->SetValue(config.visual_recoilCrosshair);
	}

	void BunnyHop(CUserCmd* cmd)
	{
		C_LocalPlayer local = g_LocalPlayer;
		if (!config.misc_bhop || local->m_fFlags() & (1 << 0) /*  ON_GROUND */)
			return;
		cmd->buttons &= ~IN_JUMP;
	}

}