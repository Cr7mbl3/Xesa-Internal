#pragma once

#include <memory>
#include <unordered_map>

#define VALUE(type, var, value) Var<type> var = { #var, value };

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
	VALUE(bool, visual_glow, true);
	VALUE(bool, visual_sniperCrosshair, true);
	VALUE(bool, visual_recoilCrosshair, true);
	VALUE(bool, visual_grenadePrediction, true);
	VALUE(bool, misc_bhop, true);
};

inline Config config;

