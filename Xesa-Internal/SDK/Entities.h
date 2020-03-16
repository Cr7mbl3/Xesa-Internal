#pragma once

#include "../NetVars.h"
#include "../vfunc.h"
#include "../utils/Math.h"
#include "Vector.h"
#include "QAngle.h"
#include "IClientEntity.h"
#include "EHandle.h"
#include "CBaseAnimState.h"
#include "UtlVector.h"
#include "ItemDefinitionIndexes.h"

#define	FL_ONGROUND				(1 << 0)
#define	FL_DUCKING				(1 << 1)
#define	FL_WATERJUMP			(1 << 2)
#define	FL_ONTRAIN				(1 << 3)
#define	FL_INRAIN				(1 << 4)
#define	FL_FROZEN				(1 << 5)
#define	FL_ATCONTROLS			(1 << 6)
#define	FL_CLIENT				(1 << 7)
#define	FL_FAKECLIENT			(1 << 8)
#define	FL_INWATER				(1 << 10)

#define	LIFE_ALIVE				0
#define	LIFE_DYING				1
#define	LIFE_DEAD				2
#define LIFE_RESPAWNABLE		3
#define LIFE_DISCARDBODY		4

//pasta from CSGOSimple
enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,
	MOVETYPE_LAST = MOVETYPE_CUSTOM,
	MOVETYPE_MAX_BITS = 4,
	MAX_MOVETYPE
};

//https://github.com/monthyx1337/GamSensFinal
class CSWeaponInfo
{
public:
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* szBulletType; //0x0080 
	char* weapon_name;			//0x008C 
	char _0x0090[56];
	int iWeaponType;
	char _0x0094[4];
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	std::byte full_auto;				//0x00EC 
	char _0x00ED[3];
	int	damage;				//0x00F0 
	float armor_ratio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float penetration;		//0x00FC 
	char _0x0100[8];
	float range;				//0x0108 
	float range_modifier;		//0x010C 
	char _0x0110[16];
	std::byte silencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};//Size=0x240

//https://github.com/monthyx1337/GamSensFinal
class CAnimationLayer
{
public:
	char  pad_0000[20];
	unsigned int m_nOrder; //0x0014
	unsigned int m_nSequence; //0x0018
	float m_flPrevCycle; //0x001C
	float m_flWeight; //0x0020
	float m_flWeightDeltaRate; //0x0024
	float m_flPlaybackRate; //0x0028
	float m_flCycle; //0x002C
	void* m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

//pasta from CSGOSimple
enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

//NetVars are not completed
class C_PlantedC4
{
public:
	NETVAR(bool, m_bBombTicking, "DT_PlantedC4", "m_bBombTicking");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(float, m_flTimerLength, "DT_PlantedC4", "m_flTimerLength");
	NETVAR(float, m_flDefuseLength, "DT_PlantedC4", "m_flDefuseLength");
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
};

class C_BaseEntity : public IClientEntity
{
public:
	NETVAR(int32_t, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");
	NETVAR(QAngle, m_vecAngles, "DT_BaseEntity", "m_vecAngles");
	NETVAR(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");

	NETVAR(bool, m_bShouldGlow, "DT_DynamicProp", "m_bShouldGlow");
	NETVAR(bool, m_bSpotted, "DT_DynamicProp", "m_bSpotted");

	NETVAR(bool, m_bClientSideAnimation, "DT_BaseAnimating", "m_bClientSideAnimation");
	NETVAR(CUtlVector<float>, m_flPoseParameter, "DT_BaseAnimating", "m_flPoseParameter");
	OFFSET(CUtlVector<matrix3x4_t>, m_matBoneCache, 0x2910);

	bool IsPlayer()
	{
		return CallVFunction<bool>(this, 157 + 0ull);
	}

	//sets the interpolation flag of the entity (0 is off, 1 is on)
	void SetInterpolationFlag(int flag)
	{
		auto var_map = reinterpret_cast<uintptr_t>(this) + 36;
		auto var_map_list_count = *reinterpret_cast<int*>(var_map + 20);

		for (auto index = 0; index < var_map_list_count; index++)
			* reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(var_map) + index * 12) = flag;
	}

	//below pasted from GamSense
	//I HAVE NETVAR FOR IT AWIHFAOJGAAWGAWJGHAFWJWAPD
	/*bool& ClientSideAnimation()
	{
		auto m_bClientSideAnimation = NetVars::Get()["DT_BaseAnimating"]["m_bClientSideAnimation"]; //TODO: Use NetVar() define instead?
		return *reinterpret_cast<bool*>(uintptr_t(this) + m_bClientSideAnimation);
	}*/

	CUtlVector<CAnimationLayer>& GetAnimationOverlays();
	CBaseHandle* m_hMyWeapons();
	void SetAnimOverlay(int, CAnimationLayer);
	void UpdateClientSideAnimation();
	CBaseAnimState* GetAnimState();
	void SetAbsAngles(QAngle angle);
	float GetMaxDesyncAngle();
};

class C_EconItemView
{
private:
	using str_32 = char[32];
public:
	NETVAR(int32_t, m_bInitialized, "DT_BaseAttributableItem", "m_bInitialized");
	NETVAR(int32_t, m_iItemDefinitionIndex, "DT_BaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iEntityLevel, "DT_BaseAttributableItem", "m_iEntityLevel");
	NETVAR(int32_t, m_iAccountID, "DT_BaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDLow, "DT_BaseAttributableItem", "m_iItemIDLow");
	NETVAR(int32_t, m_iItemIDHigh, "DT_BaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "DT_BaseAttributableItem", "m_iEntityQuality");
	NETVAR(str_32, m_iCustomName, "DT_BaseAttributableItem", "m_szCustomName");
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:

	NETVAR(int32_t, m_iItemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_nFallbackStatTrak, "CBaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "CBaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "CBaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "CBaseAttributableItem", "m_flFallbackWear");
	NETVAR(int32_t, m_iAccountID, "CBaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "CBaseAttributableItem", "m_iEntityQuality");
	PTRNETVAR(char, m_szCustomName, "CBaseAttributableItem", "m_szCustomName");
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:

	NETVAR(float_t, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float_t, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");

	NETVAR(int32_t, m_iItemDefinitionIndex, "CBaseCombatWeapon", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iClip1, "CBaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iViewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(float_t, m_fAccuracyPenalty, "CWeaponCSBase", "m_fAccuracyPenalty");
	NETVAR(int32_t, m_zoomLevel, "CWeaponCSBaseGun", "m_zoomLevel");
	NETVAR(bool, m_bPinPulled, "CBaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "CBaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "CWeaponCSBase", "m_flPostponeFireReadyTime");
	NETVAR(float_t, m_fLastShotTime, "CWeaponCSBase", "m_fLastShotTime");

	//https://github.com/monthyx1337/GamSensFinal
	float GetInaccuracy() {
		return CallVFunction<float>(this, 482);
	}

	//from Osiris
	CSWeaponInfo* GetCSWpnData() {
		return CallVFunction<CSWeaponInfo*>(this, 460);
	}
	//from CSGOSimple
	C_EconItemView& m_Item() //TODO: Check if NetVar works: "DT_BaseAttributableItem", "m_Item"
	{
		return *(C_EconItemView*)this;
	}

	//from CSGOSimple
	bool IsKnife() {
		if (m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) 
			return false;
		return GetCSWpnData()->iWeaponType == WEAPONTYPE_KNIFE;
	}

};

class C_BasePlayer : public C_BaseEntity
{
public:
	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles[0]");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHeavyArmor, "DT_CSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(float, m_flFlashMaxAlpha, "DT_CSPlayer", "m_flFlashMaxAlpha");
	NETVAR(int32_t, m_iAccount, "DT_CSPlayer", "m_iAccount");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");

	NETVAR(Vector, m_ragPos, "DT_Ragdoll", "m_ragPos");

	NETVAR(int32_t, m_iHealth, "DT_BasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "DT_BasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "DT_BasePlayer", "m_fFlags");

	NETVAR(int32_t, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(float, m_flMaxspeed, "DT_BasePlayer", "m_flMaxspeed");

	NETVAR(QAngle, m_angAbsAngles, "DT_BaseEntity", "m_angAbsAngles");
	NETVAR(Vector, m_angAbsOrigin, "DT_BaseEntity", "m_angAbsOrigin");
	NETVAR(float, m_flDuckSpeed, "DT_BaseEntity", "m_flDuckSpeed");
	NETVAR(float, m_flDuckAmount, "DT_BaseEntity", "m_flDuckAmount");

	NETVAR(int32_t, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	NETVAR(float, m_flCycle, "DT_BaseAnimating", "m_flCycle");

	NETVAR(int, m_nSequence, "DT_BaseViewModel", "m_nSequence");

	NETVAR(float, m_flNextAttack, "DT_BaseCombatCharacter", "m_flNextAttack");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");

	//TODO: Add Offset define
	OFFSET(int, m_iBoneCount, 0x291C);
	int& m_eFlags()
	{
		return *(int*)((uintptr_t)this + 0xE8); 
	}

	Vector& m_absVelocity()
	{
		return *(Vector*)((uintptr_t)this + 0x94);
	}

	int& m_iLastSetupBonesFrame()
	{
		return *(int*)((uintptr_t)this + 0xA68);
	}

	//TODO: I dont think this works //TODO: Mby i fixed it?
	float GetMaxDesyncAngle(CBaseAnimState* override_animstate = nullptr, bool jitter = false) {
		float max_desync_angle = 0.f;

		auto anim_state = override_animstate != nullptr ? override_animstate : this->GetAnimState();
		if (!anim_state)
			return max_desync_angle;

		float duck_amount = anim_state->m_fDuckAmount;
		float speed_fraction = max(0, min(anim_state->m_flFeetSpeedForwardsOrSideWays, 1));
		float speed_factor = max(0, min(1, anim_state->m_flFeetSpeedForwardsOrSideWays));

		float yaw_modifier = (((anim_state->m_flStopToFullRunningFraction * -0.3f) - 0.2f) * speed_fraction) + 1.0f;

		if (duck_amount > 0.f) {
			yaw_modifier += ((duck_amount * speed_factor) * (0.5f - yaw_modifier));
		}

		max_desync_angle = anim_state->m_flMaxBodyYaw() * yaw_modifier;

		if (jitter) {
			if (const auto yaw_feet_delta = anim_state->m_flGoalFeetYaw - anim_state->m_flEyeYaw; yaw_feet_delta < max_desync_angle) {
				max_desync_angle = 180.f;
			}
		}

		return max_desync_angle;
	}

	Vector GetEyePos()
	{
		return m_vecOrigin() + m_vecViewOffset();
	}

	bool IsAlive()
	{
		return m_lifeState() == LIFE_ALIVE;
	}

	Vector        GetHitboxPos(int);
	bool          CanSeePlayer(C_BasePlayer*, int);
	bool          CanSeePlayer(C_BasePlayer*, const Vector&);
	int           m_nMoveType();
	bool          CanShoot();
	void          InvalidateBoneCache();
};

class C_LocalPlayer
{
	friend bool operator==(const C_LocalPlayer& lhs, void* rhs);
public:
	C_LocalPlayer() : m_local(nullptr) {}

	operator bool() const { return *m_local != nullptr; }
	operator C_BasePlayer* () const { return *m_local; }

	C_BasePlayer* operator->() { return *m_local; }

private:
	C_BasePlayer** m_local;
};