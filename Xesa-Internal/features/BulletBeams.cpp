#include "BulletBeams.h"

#include <iostream>

#include "../Config.h"

void BulletBeams::Initialize()
{
	g_GameEventManager->AddListener(listener, "bullet_impact", false);
}

void BulletBeams::Release()
{
	g_GameEventManager->RemoveListener(listener);
}

void BulletBeams::OnDoPostScreenEffects()
{
	if (!g_Engine->IsInGame() || !g_LocalPlayer || !config.visual_bulletBeams)
	{
		impacts.clear();
		return;
	}

	for (size_t i = 0; i < impacts.size(); i++)
	{
		auto impact = impacts[i];

		if (!impact.player || impact.player->IsDormant())
			continue;

		BeamInfo_t beam_info;
		beam_info.m_vecStart = impact.start;

		if (impact.player == g_LocalPlayer)
			beam_info.m_vecStart.z -= 2.f;

		beam_info.m_vecEnd = impact.pos;
		beam_info.m_nType = TE_BEAMPOINTS;
		beam_info.m_pszModelName = "sprites/purplelaser1.vmt";
		beam_info.m_nModelIndex = -1;
		beam_info.m_flHaloScale = 0.0f;
		beam_info.m_flLife = 4.f;
		beam_info.m_flWidth = 2.0f;
		beam_info.m_flEndWidth = 2.0f;
		beam_info.m_flFadeLength = 0.0f;
		beam_info.m_flAmplitude = 2.0f;
		beam_info.m_flBrightness = 255.f;
		beam_info.m_flSpeed = 0.2f;
		beam_info.m_nStartFrame = 0;
		beam_info.m_flFrameRate = 0.f;
		beam_info.m_flRed = float(impact.color.r());
		beam_info.m_flGreen = float(impact.color.g());
		beam_info.m_flBlue = float(impact.color.b());
		beam_info.m_nSegments = 2;
		beam_info.m_bRenderable = true;
		beam_info.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

		const auto beam = g_ViewRenderBeams->CreateBeamPoints(beam_info);
		if (beam) {
			g_ViewRenderBeams->DrawBeam(beam);
		}
	}

	impacts.clear();
}

void BulletBeams::OnBulletImpact(IGameEvent* event)
{
	if (!g_Engine->IsInGame() || !g_LocalPlayer || !config.visual_bulletBeams)
		return;

	const auto userid = g_Engine->GetPlayerForUserID(event->GetInt("userid"));

	static float last_curtime;

	const auto clientEntity = g_EntityList->GetClientEntity(userid);
	if (!clientEntity || !clientEntity->GetBaseEntity()->IsPlayer())
		return;

	const auto player = (C_BasePlayer*) clientEntity;

	if (player->IsDormant())
		return;

	const Vector pos(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));

	Color col = Color(255, 255, 255);

	Impact_Info_t impact(g_GlobalVars->curtime, player->m_vecOrigin() + player->m_vecViewOffset(), pos, player, col);

	/*if (last_curtime == g_GlobalVars->curtime)
	{
		if (!impacts.empty() && impact.Length() < (impacts.end() - 1)->Length() || impacts.empty())
			return;

		impacts.pop_back();
	}*/

	last_curtime = g_GlobalVars->curtime;

	impacts.push_back(impact);

}

void BulletBeams::BulletBeamsListener::FireGameEvent(IGameEvent* event)
{
	if (!strcmp(event->GetName(), "bullet_impact")) 
	{
		Get().OnBulletImpact(event);
	}
}
