#pragma once

class ConVar {
public:

	float GetFloat() {
		using original_fn = float(__thiscall*)(ConVar*);
		return (*(original_fn**)this)[11](this);
	}

	int GetInt() {
		using original_fn = int(__thiscall*)(ConVar*);
		return (*(original_fn**)this)[11](this);
	}

	bool GetBool() {
		using original_fn = bool(__thiscall*)(ConVar*);
		return (*(original_fn * *)this)[12](this);
	}

	const char* GetString() {
		using original_fn = const char*(__thiscall*)(ConVar*);
		return (*(original_fn**)this)[13](this);
	}

	void SetValue(const char* value) {
		using original_fn = void(__thiscall*)(ConVar*, const char*);
		return (*(original_fn**)this)[14](this, value);
	}

	void SetValue(float value) {
		using original_fn = void(__thiscall*)(ConVar*, float);
		return (*(original_fn**)this)[15](this, value);
	}

	void SetValue(int value) {
		using original_fn = void(__thiscall*)(ConVar*, int);
		return (*(original_fn**)this)[16](this, value);
	}

	void SetValue(bool value) {
		using original_fn = void(__thiscall*)(ConVar*, int);
		return (*(original_fn * *)this)[16](this, static_cast<int>(value));
	}

};