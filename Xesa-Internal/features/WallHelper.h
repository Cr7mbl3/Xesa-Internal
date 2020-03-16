#pragma once

#include "../SDK/Entities.h"
#include "../SDK/IPhysics.h"
#include "../SDK/IEngineTrace.h"
#include "../SDK/Vector.h"


namespace WallHelper 
{

	float HandleBulletPenetration(surfacedata_t*, CGameTrace&, const Vector&, Vector& result, float penetration, float damage);
	bool IsBulletDamageOverMinDamage(C_BasePlayer* fromPlayer, C_BasePlayer* toPlayer, const Vector& destination, const CSWeaponInfo* weaponData, float minDamage);

}