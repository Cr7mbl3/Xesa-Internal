#include "Misc.h"

#include <math.h>

#include "../Interfaces.h"
#include "../Config.h"
#include "../utils/Math.h"

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
		static bool wasOnGround = false;
		bool onGround = g_LocalPlayer->m_fFlags() & FL_ONGROUND;
		if (config.misc_bhop && !onGround)
			cmd->buttons &= ~IN_JUMP;
		wasOnGround = onGround;
	}

	//pasta from CSGOSimple
	void Thirdperson()
	{
		static bool lastState = config.misc_thirdperson;
		if (lastState != config.misc_thirdperson) {
			KeyBinds::thirdpersonState = config.misc_thirdperson;
		}
		lastState = config.misc_thirdperson;

		if (!config.misc_thirdperson)
			return;
		
		if (!g_LocalPlayer)
			return;

		static float lastTime{ 0.0f };

		if (GetAsyncKeyState(config.misc_thirdperson_key) && g_GlobalVars->realtime - lastTime > 0.5f) {
			KeyBinds::thirdpersonState = !KeyBinds::thirdpersonState;
			lastTime = g_GlobalVars->realtime;
		}

		if (KeyBinds::thirdpersonState && g_LocalPlayer->IsAlive())
		{
			if (!g_Input->m_fCameraInThirdPerson)
			{
				g_Input->m_fCameraInThirdPerson = true;
			}

			float dist = config.misc_thirdperson_distance;

			QAngle view;
			g_Engine->GetViewAngles(&view);
			trace_t tr;
			Ray_t ray;

			Vector desiredCamOffset = Vector(cos(DEG2RAD(view.yaw)) * dist,
				sin(DEG2RAD(view.yaw)) * dist,
				sin(DEG2RAD(-view.pitch)) * dist
			);

			//cast a ray from the Current camera Origin to the Desired 3rd person Camera origin
			ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
			CTraceFilter traceFilter;
			traceFilter.pSkip = g_LocalPlayer;
			g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);

			Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;

			float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));// Pythagorean

			bool horOK = distance2D > (dist - 2.0f);
			bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);

			float cameraDistance;

			if (horOK && vertOK)  // If we are clear of obstacles
			{
				cameraDistance = dist; // go ahead and set the distance to the setting
			}
			else
			{
				if (vertOK) // if the Vertical Axis is OK
				{
					cameraDistance = distance2D * 0.95f;
				}
				else// otherwise we need to move closer to not go into the floor/ceiling
				{
					cameraDistance = abs(diff.z) * 0.95f;
				}
			}
			g_Input->m_fCameraInThirdPerson = true;

			g_Input->m_vecCameraOffset.z = cameraDistance;
		}
		else
		{
			g_Input->m_fCameraInThirdPerson = false;
		}

	}

	void FixAnimationLOD()
	{
		if (!config.misc_lc_fixAnimationLOD)
			return;

		for (int i = 1; i < g_EntityList->GetHighestEntityIndex(); i++) {
			IClientEntity* clientEntity = g_EntityList->GetClientEntity(i);

			if (!clientEntity || clientEntity == g_LocalPlayer)
				continue;

			C_BaseEntity* baseEntity = clientEntity->GetBaseEntity();
			if (!baseEntity || baseEntity->IsDormant() || !baseEntity->IsPlayer())
				continue;

			C_BasePlayer* basePlayer = (C_BasePlayer*)baseEntity;

			if (!basePlayer->IsAlive())
				continue;

			//https://github.com/DoubloRev/suicide.cc/blob/master/AYYWARE%20CSGO/Hooks.cpp
			*(int*)((uintptr_t)basePlayer + 0xA30) = g_GlobalVars->framecount;
			*(int*)((uintptr_t)basePlayer + 0xA28) = 0;
		}
	}

}