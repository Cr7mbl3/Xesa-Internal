#include "Interfaces.h"

void Interfaces::Initialize()
{
	INIT_INTERFACE(g_Cvar, ICvar, Modules::vstdlib, "VEngineCvar007");
	INIT_INTERFACE(g_Client, IBaseClientDLL, Modules::client_panorama, "VClient018");
	INIT_INTERFACE(g_EntityList, IClientEntityList, Modules::client_panorama, "VClientEntityList003");
	INIT_INTERFACE(g_Surface, ISurface, Modules::vguimatsurface, "VGUI_Surface031");

	OINIT_INTERFACE(g_ClientMode, CClientMode, g_Client, 10, 5);
	OINIT_INTERFACE(g_Globals, CGlobalVarsBase, g_Client, 0, 27);

	PATTERN_SCAN(g_D3DDevice9, **(IDirect3DDevice9***), Modules::shaderapidx9, "A1 ? ? ? ? 50 8B 08 FF 51 0C", 1);
	PATTERN_SCAN(g_Input, *(CInput**), Modules::client_panorama, "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10", 1);
	PATTERN_SCAN(g_LocalPlayer, *(C_LocalPlayer*), Modules::client_panorama, "8B 0D ? ? ? ? 83 FF FF 74 07", 2);
	PATTERN_SCAN(g_GlowObjectManager, *(CGlowObjectManager**), Modules::client_panorama, "0F 11 05 ? ? ? ? 83 C8 01", 3);
}

HMODULE Interfaces::findModule(const wchar_t* name)
{
	HMODULE hModule = GetModuleHandleW(name);
	if (!hModule) {
		MessageBoxA(nullptr, (std::ostringstream{} << "Cannot find " << name << " module!").str().c_str(), "Xesa", MB_OK | MB_ICONERROR);
		std::exit(EXIT_FAILURE);
	}
	return hModule;
}

//edited pasterino from Osiris
template <typename T>
T* Interfaces::findInterface(HMODULE hModule, const char* name)
{
	T* foundInterface = reinterpret_cast<std::add_pointer_t<T * (const char* name, int* returnCode)>>(GetProcAddress(hModule, "CreateInterface"))(name, nullptr);

	if (!foundInterface) {
		MessageBoxA(nullptr, (std::ostringstream{} << "Cannot find " << name << " interface!").str().c_str(), "Xesa", MB_OK | MB_ICONERROR);
		std::exit(EXIT_FAILURE);
	}

	return foundInterface;
}