#include "Misc.h"

#include "../Interfaces.h"

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

}