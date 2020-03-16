#include "WallHelper.h"

#include "../Interfaces.h"
#include "../SDK/Studio.h"

//idk anymore where this is from, got it from my old paste
bool TraceToExitRebuilt(Vector& end, trace_t& enter_trace, Vector start, Vector dir, trace_t* exit_trace) {
	float distance = 0.0f;

	while (distance <= 90.0f) {
		distance += 4.0f;
		end = start + dir * distance;
		 
		int point_contents = g_EngineTrace->GetPointContents(end, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL);

		if (point_contents & MASK_SHOT_HULL && (!(point_contents & CONTENTS_HITBOX)))
			continue;

		auto new_end = end - (dir * 4.0f);

		Ray_t ray;
		ray.Init(end, new_end);
		g_EngineTrace->TraceRay(ray, MASK_SHOT, 0, exit_trace);

		if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX) {
			ray.Init(end, start);
			CTraceFilter filter;
			filter.pSkip = (IHandleEntity*)exit_trace->hit_entity;
			g_EngineTrace->TraceRay(ray, 0x600400B, &filter, exit_trace);

			if ((exit_trace->fraction < 1.0f || exit_trace->allsolid) && !exit_trace->startsolid) {
				end = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!(exit_trace->fraction < 1.0 || exit_trace->allsolid || exit_trace->startsolid) || exit_trace->startsolid) {
			if (exit_trace->hit_entity) {
				if (enter_trace.hit_entity && enter_trace.hit_entity != g_EntityList->GetClientEntity(0))
					return true;
			}
			continue;
		}

		if (((exit_trace->surface.flags >> 7) & 1) && !((enter_trace.surface.flags >> 7) & 1))
			continue;

		if (exit_trace->plane.normal.Dot(dir) <= 1.0f) {
			float fraction = exit_trace->fraction * 4.0f;
			end = end - (dir * fraction);
			return true;
		}
	}
	return false;
}

//pasta from Osiris
float WallHelper::HandleBulletPenetration(surfacedata_t* enterSurfaceData, CGameTrace& enterTrace, const Vector& direction, Vector& result, float penetration, float damage)
{
	Vector end;
	CGameTrace exitTrace;
	__asm {
		mov ecx, end
		mov edx, enterTrace
	}
	if (!TraceToExitRebuilt(end, enterTrace, enterTrace.endpos, direction, &exitTrace))
		return -1.0f;

	surfacedata_t* exitSurfaceData = g_PhysicsSurface->GetSurfaceData(exitTrace.surface.surfaceProps);

	float damageModifier = 0.16f;
	float penetrationModifier = (enterSurfaceData->game.flPenetrationModifier + exitSurfaceData->game.flPenetrationModifier) / 2.0f;

	if (enterSurfaceData->game.material == 71 || enterSurfaceData->game.material == 89) {
		damageModifier = 0.05f;
		penetrationModifier = 3.0f;
	}
	else if (enterTrace.contents >> 3 & 1 || enterTrace.surface.flags >> 7 & 1) {
		penetrationModifier = 1.0f;
	}

	if (enterSurfaceData->game.material == exitSurfaceData->game.material) {
		if (exitSurfaceData->game.material == 85 || exitSurfaceData->game.material == 87)
			penetrationModifier = 3.0f;
		else if (exitSurfaceData->game.material == 76)
			penetrationModifier = 2.0f;
	}

	damage -= 11.25f / penetration / penetrationModifier + damage * damageModifier + (exitTrace.endpos - enterTrace.endpos).LengthSqr() / 24.0f / penetrationModifier;

	result = exitTrace.endpos;
	return damage;
}

//pasta from Osiris
bool WallHelper::IsBulletDamageOverMinDamage(C_BasePlayer* fromPlayer, C_BasePlayer* toPlayer, const Vector& destination, const CSWeaponInfo* weaponData, float minDamage)
{
	float damage{ static_cast<float>(weaponData->damage) };

	Vector start{ fromPlayer->m_vecOrigin() + fromPlayer->m_vecViewOffset() };
	Vector direction{ destination - start };
	direction /= direction.Length();

	int hitsLeft = 4;

	while (damage >= 1.0f && hitsLeft) {
		CGameTrace trace;
		Ray_t ray;
		CTraceFilter filter;
		filter.pSkip = fromPlayer;
		ray.Init(start, destination);
		g_EngineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);

		if (trace.fraction == 1.0f)
			break;

		if (trace.hit_entity == toPlayer && trace.hitgroup > HITGROUP_GENERIC && trace.hitgroup <= HITGROUP_RIGHTLEG) {
			damage = HitGroup::GetDamageMultiplier(trace.hitgroup) * damage * powf(weaponData->range_modifier, trace.fraction * weaponData->range / 500.0f);

			if (float armorRatio{ weaponData->armor_ratio / 2.0f }; HitGroup::IsArmored(trace.hitgroup, toPlayer->m_bHasHelmet()))
				damage -= (toPlayer->m_ArmorValue() < damage * armorRatio / 2.0f ? toPlayer->m_ArmorValue() * 4.0f : damage) * (1.0f - armorRatio);

			return damage >= minDamage;
		}
		const auto surfaceData = g_PhysicsSurface->GetSurfaceData(trace.surface.surfaceProps);

		if (surfaceData->game.flPenetrationModifier < 0.1f)
			break;

		damage = HandleBulletPenetration(surfaceData, trace, direction, start, weaponData->penetration, damage);
		hitsLeft--;
	}
	return false;
}