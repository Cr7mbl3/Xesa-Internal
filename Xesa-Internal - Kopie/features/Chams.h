#pragma once

#include "../utils/Singleton.h"

#include "../SDK/IVModelRenderer.h"
#include "../SDK/Color.h"

class Chams : public Singleton<Chams> {
private:
	void ArmChams(const model_t* model, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
	void PlayerChams(const model_t* model, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
	
	IMaterial* materialRegular = nullptr;
	IMaterial* materialFlat = nullptr;
public:
	Chams();
	void OverrideMaterial(bool ignoreZ, bool flat, bool wireframe, bool glass, const Color& rgba);
	void OnDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
};