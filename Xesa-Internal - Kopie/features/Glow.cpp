#include "Glow.h"

#include "../Config.h"
#include "../Interfaces.h"

namespace Glow {

	void Run()
	{
		if (!g_LocalPlayer)
			return;

		int32_t localTeamNum = g_LocalPlayer->m_iTeamNum();
		for (auto i = 0; i < g_GlowObjectManager->m_GlowObjectDefinitions.m_Size; i++) {
			auto& glowObject = g_GlowObjectManager->m_GlowObjectDefinitions[i];
			auto entity = (IClientNetworkable*) glowObject.m_pEntity;
			if (!entity || entity->IsDormant()) {
				continue;
			}
			auto player = (C_BasePlayer*) entity;
			if (config.visual_glow && player->m_lifeState() == 0 && player->m_iTeamNum() != localTeamNum && entity->GetClientClass()->m_ClassID == 40) {
				glowObject.m_flRed = 1.f;
				glowObject.m_flGreen = 1.f;
				glowObject.m_flBlue = 1.f;
				glowObject.m_flAlpha = 0.5f;
				glowObject.m_bRenderWhenOccluded = true;
				glowObject.m_bRenderWhenUnoccluded = false;
			}
			else {
				glowObject.m_flAlpha = 0.f;
			}
		}
	}

	void Release()
	{
		for (auto i = 0; i < g_GlowObjectManager->m_GlowObjectDefinitions.m_Size; i++) {
			auto& glowObject = g_GlowObjectManager->m_GlowObjectDefinitions[i];
			auto entity = (IClientNetworkable*) glowObject.m_pEntity;

			if (glowObject.IsUnused())
				continue;

			if (!entity || entity->IsDormant())
				continue;

			glowObject.m_flAlpha = 0.0f;
		}
	}
}
