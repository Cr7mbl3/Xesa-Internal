#include "Entities.h"

#include "Studio.h"
#include "ItemDefinitionIndexes.h"
#include "../Interfaces.h"
#include "../utils/Math.h"
#include "../vfunc.h"

//https://www.unknowncheats.me/forum/1903835-post9.html
Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t matrix[MAXSTUDIOBONES];

	if (!SetupBones(matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0))
		return Vector{};

	studiohdr_t* hdr = g_ModelInfo->GetStudiomodel(GetModel());

	if (!hdr)
		return Vector{};

	mstudiohitboxset_t* hitboxSet = hdr->GetHitboxSet(m_nHitboxSet());
	if(!hitboxSet)
		return Vector{};
	mstudiobbox_t* untransformedBox = hitboxSet->GetHitbox(hitbox_id);
	if (!untransformedBox)
		return Vector{};

	Vector bbmin = untransformedBox->bbmin;
	Vector bbmax = untransformedBox->bbmax;

	if (untransformedBox->m_flRadius != -1.f)
	{
		bbmin -= Vector(untransformedBox->m_flRadius, untransformedBox->m_flRadius, untransformedBox->m_flRadius);
		bbmax += Vector(untransformedBox->m_flRadius, untransformedBox->m_flRadius, untransformedBox->m_flRadius);
	}

	if (untransformedBox)
	{
		Vector vMin, vMax, vCenter, sCenter;
		Math::VectorTransform(bbmin, matrix[untransformedBox->bone], vMin);
		Math::VectorTransform(bbmax, matrix[untransformedBox->bone], vMax);

		vCenter = ((vMin + vMax) * 0.5f);

		return vCenter;
	}
	return Vector{};
}

CUtlVector<CAnimationLayer>& C_BaseEntity::GetAnimationOverlays() {
	return *reinterpret_cast<CUtlVector<CAnimationLayer>*>(uintptr_t(this) + 0x2980);
}
CBaseHandle* C_BaseEntity::m_hMyWeapons()
{
	return (CBaseHandle*)((uintptr_t)this + 0x2DF8);
}

void C_BaseEntity::SetAnimOverlay(int Index, CAnimationLayer layer)
{
	(*(CAnimationLayer **)((DWORD)this + 0x2990))[Index] = layer;
}

void C_BaseEntity::UpdateClientSideAnimation()
{
	CallVFunction<void>(this, 223);
}

CBaseAnimState* C_BaseEntity::GetAnimState()
{
	return *reinterpret_cast<CBaseAnimState**>(uintptr_t(this) + 0x3900);
}

void C_BaseEntity::SetAbsAngles(QAngle angle)
{
	using oSetAbsAnglesFn = void(__thiscall*)(C_BaseEntity*, const QAngle& angles);
	static oSetAbsAnglesFn oSetAbsAngles = (oSetAbsAnglesFn)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
	oSetAbsAngles(this, angle);
}

float C_BaseEntity::GetMaxDesyncAngle()
{//TODO: Check if this works
	CBaseAnimState* state = GetAnimState();
	if(!state)
		return 0.0f;

	float yawModifier = (state->m_flStopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(state->m_flFeetSpeedForwardsOrSideWays, 0.0f, 1.0f) + 1.0f;

	if (state->m_fDuckAmount > 0.0f)
		yawModifier += (state->m_fDuckAmount * std::clamp(state->m_flFeetSpeedUnknownForwardOrSideways, 0.0f, 1.0f) * (0.5f - yawModifier));

	return state->m_flUnknownVelocityLean * yawModifier; //unsure about m_flUnknownVelocityLean
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	ray.Init(GetEyePos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

int C_BasePlayer::m_nMoveType()
{
	//pasta from CSGOSimple
	return *(int*)((uintptr_t)this + 0x25C);
}

bool C_BasePlayer::CanShoot()
{
	float serverTime = g_GlobalVars->curtime;

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*) g_EntityList->GetClientEntityFromHandle(m_hActiveWeapon());

	//if(m_flNextAttack() > serverTime)
	if (m_flNextAttack() > serverTime)
		return false;

	if (!weapon)
		return false;

	//TODO: Check for famas/glock burst thingy here

	if (weapon->m_iClip1() <= 0)
		return false;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER && weapon->m_flPostponeFireReadyTime() > serverTime)
		return false;

	return weapon->m_flNextPrimaryAttack() <= serverTime;
}

void C_BasePlayer::InvalidateBoneCache()
{
	static auto invalidate_bone_cache = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81");
	static auto model_bone_counter = **reinterpret_cast<uintptr_t * *>(invalidate_bone_cache + 10);
	*reinterpret_cast<unsigned int*>(uintptr_t(this) + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*reinterpret_cast<unsigned int*>(uintptr_t(this) + 0x2690) = (model_bone_counter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}
