#include "EnginePrediction.h"

#include "../Interfaces.h"
#include "../SDK/checksum_md5.h"

CMoveData data;
float old_cur_time;
float old_frame_time;
int* prediction_random_seed;

void EnginePrediction::StartPrediction(CUserCmd* cmd)
{
	if (!g_LocalPlayer)
		return;

	static bool initialized = false;
	if (!initialized) {
		prediction_random_seed = *reinterpret_cast<int**>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);
		initialized = true;
	}

	*prediction_random_seed = MD5_PseudoRandom(cmd->command_number) & 0x7FFFFFFF;

	old_cur_time = g_GlobalVars->curtime;
	old_frame_time = g_GlobalVars->frametime;

	g_GlobalVars->curtime = g_LocalPlayer->m_nTickBase() * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	g_GameMovement->StartTrackPredictionErrors(g_LocalPlayer);

	memset(&data, 0, sizeof(data));
	g_MoveHelper->SetHost(g_LocalPlayer);
	g_Prediction->SetupMove(g_LocalPlayer, cmd, g_MoveHelper, &data);
	g_GameMovement->ProcessMovement(g_LocalPlayer, &data);
	g_Prediction->FinishMove(g_LocalPlayer, cmd, &data);
}

void EnginePrediction::EndPrediction()
{
	g_GameMovement->FinishTrackPredictionErrors(g_LocalPlayer);
	g_MoveHelper->SetHost(0);

	*prediction_random_seed = -1;

	g_GlobalVars->curtime = old_cur_time;
	//TODO: og; is this right? g_GlobalVars->frametime = old_cur_time;
	g_GlobalVars->frametime = old_frame_time;
}
