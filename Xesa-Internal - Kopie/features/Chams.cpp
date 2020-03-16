#include "Chams.h"

#include <iostream>

#include "../Interfaces.h"
#include "../Hooks.h"
#include "../Config.h"

Chams::Chams()
{
	materialRegular = g_MaterialSystem->FindMaterial("debug/debugambientcube");
	materialFlat = g_MaterialSystem->FindMaterial("debug/debugdrawflat");
}

void Chams::OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba) {
	IMaterial* material = flat ? materialFlat : materialRegular;

	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, ignoreZ);

	if (glass) {
		material = materialFlat;
		material->AlphaModulate(0.45f);
	}
	else {
		material->AlphaModulate(rgba.a() / 255.0f);
	}

	material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, wireframe);
	material->ColorModulate(rgba.r() / 255.0f, rgba.g() / 255.0f, rgba.b() / 255.0f);

	g_ModelRender->ForcedMaterialOverride(material);
}

//TODO: create fnDME in constructor
void Chams::ArmChams(const model_t* model, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static auto oDrawModelExecute = Hooks::modelrenderHook.get_original<decltype(&Hooks::DrawModelExecute)>(Hooks::index::DrawModelExecute);
	if (!config.visual_chams_arms)
		return;

	auto material = g_MaterialSystem->FindMaterial(model->szName, TEXTURE_GROUP_MODEL);
	if (!material)
		return;

	if (config.visual_chams_arms_ingorez) {
		OverrideMaterial(true, config.visual_chams_arms_flat, config.visual_chams_arms_wireframe, false, config.visual_chams_arms_occluded);
		oDrawModelExecute(g_ModelRender, 0, ctx, state, info, matrix);
		OverrideMaterial(false, config.visual_chams_arms_flat, config.visual_chams_arms_wireframe, false, config.visual_chams_arms_visible);
	}
	else {
		OverrideMaterial(false, config.visual_chams_arms_flat, config.visual_chams_arms_wireframe, config.visual_chams_arms_glass, config.visual_chams_arms_visible);
	}
}

void Chams::PlayerChams(const model_t* model, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	static auto oDrawModelExecute = Hooks::modelrenderHook.get_original<decltype(&Hooks::DrawModelExecute)>(Hooks::index::DrawModelExecute);

	C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(info.entity_index);

	if (player && player->IsAlive() && g_LocalPlayer) {
		bool enemy = player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
		if (enemy && config.visual_chams_enemies) {
			if (config.visual_chams_enemies_ingorez) {
				OverrideMaterial(true, config.visual_chams_enemies_flat, config.visual_chams_enemies_wireframe, false, config.visual_chams_enemies_occluded);
				oDrawModelExecute(g_ModelRender, 0, ctx, state, info, matrix);
				OverrideMaterial(false, config.visual_chams_enemies_flat, config.visual_chams_enemies_wireframe, false, config.visual_chams_enemies_visible);
			}
			else {
				OverrideMaterial(false, config.visual_chams_enemies_flat, config.visual_chams_enemies_wireframe, config.visual_chams_enemies_glass, config.visual_chams_enemies_visible);
			}
		}
	}
}

void Chams::OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	auto mdl = info.pModel;
	if (strstr(mdl->szName, "arms") != nullptr) {
		if(!config.visual_removals_hands)
			ArmChams(mdl, ctx, state, info, matrix);
	}
	if (strstr(mdl->szName, "models/player") != nullptr) {
		PlayerChams(mdl, ctx, state, info, matrix);
	}
}
