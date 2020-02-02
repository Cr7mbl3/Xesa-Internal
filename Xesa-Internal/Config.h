#pragma once

#include <memory>
#include <unordered_map>

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
	VALUE(bool, visual_glow, false);
	VALUE(bool, visual_sniperCrosshair, false);
	VALUE(bool, visual_recoilCrosshair, false);
	VALUE(bool, visual_grenadePrediction, false);

	VALUE(bool, misc_bhop, false);
	VALUE(bool, misc_fakelag, false);

	VALUE(int, menu_DisplayMode, 0);
};

inline Config config;

