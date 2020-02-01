#include "LagCompensation.h"

#include "../Interfaces.h"

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
