#include "CBaseAnimState.h"

//pasta from StickRpg Improved

float& CBaseAnimState::m_flAbsRotation()
{
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x80);
}

float CBaseAnimState::m_flMinBodyYaw()
{
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x330);
}

float CBaseAnimState::m_flMaxBodyYaw()
{
	return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(this) + 0x334);
}
