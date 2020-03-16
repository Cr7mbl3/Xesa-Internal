#pragma once

#include <memory>
#include <unordered_map>

#include "SDK/Color.h"

#define VALUE(type, var, value) Var<type> var = { #var, value };

enum Menu_DisplayModes {
	DISPLAYMODE_PRESENT, 
	DISPLAYMODE_ENDSCENE
};

static const char* Menu_DisplayModeNames[] = {
	"Present",
	"EndScene"
};

template <typename T>
class Var {
public:
	std::string name;
	std::shared_ptr<T> value;
	int32_t size;
	Var(std::string name, T v) : name(name) {
		value = std::make_shared<T>(v);
		size = sizeof(T);
	}
	operator T() { return *value; }
	operator T*() { return &*value; }
	operator T() const { return *value; }
};

class Config
{
public:
	VALUE(bool, aim_ragebot_enable, false);
	VALUE(float, aim_ragebot_hitchance, 69.f);
	VALUE(float, aim_ragebot_min_damage, 20.f);
	VALUE(bool, aim_antiaim_enable, false);
	VALUE(bool, aim_resolver_enable, false);

	VALUE(bool, visual_glow, false);

	VALUE(bool, visual_chams_arms, false);
	VALUE(bool, visual_chams_arms_ingorez, false);
	VALUE(bool, visual_chams_arms_flat, false);
	VALUE(bool, visual_chams_arms_wireframe, false);
	VALUE(bool, visual_chams_arms_glass, false);
	VALUE(Color, visual_chams_arms_occluded, Color(255, 255, 255, 255));
	VALUE(Color, visual_chams_arms_visible, Color(255, 255, 255, 255));

	VALUE(bool, visual_chams_enemies, false);
	VALUE(bool, visual_chams_enemies_ingorez, false);
	VALUE(bool, visual_chams_enemies_flat, false);
	VALUE(bool, visual_chams_enemies_wireframe, false);
	VALUE(bool, visual_chams_enemies_glass, false);
	VALUE(Color, visual_chams_enemies_occluded, Color(255, 0, 0, 255));
	VALUE(Color, visual_chams_enemies_visible, Color(0, 255, 0, 255));

	VALUE(bool, visual_sniperCrosshair, false);
	VALUE(bool, visual_recoilCrosshair, false);
	VALUE(bool, visual_grenadePrediction, false);
	VALUE(bool, visual_bulletBeams, false);

	VALUE(bool, visual_removals_postProcessing, false);
	VALUE(bool, visual_removals_flash_effect, false);
	VALUE(bool, visual_removals_smokes, false);
	VALUE(bool, visual_removals_hands, false);
	VALUE(bool, visual_removals_sleeves, false);

	VALUE(bool, misc_bhop, false);
	VALUE(bool, misc_thirdperson, false);
	VALUE(int, misc_thirdperson_key, true);
	VALUE(float, misc_thirdperson_distance, 150.f);

	VALUE(bool, misc_lc_fakelag, false);
	VALUE(bool, misc_lc_disable_interpolation, false);
	VALUE(bool, misc_lc_fixAnimationLOD, false);
	VALUE(bool, misc_lc_disable_occlusion_check, false); //check if this should be under LagComp category

	VALUE(int, menu_DisplayMode, 0);
};

inline Config config;

