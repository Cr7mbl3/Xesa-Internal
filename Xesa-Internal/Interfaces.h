#pragma once

#include <sstream>
#include <type_traits>
#include <Windows.h>
#include <d3d9.h>

#include "Utils.h"
#include "Singleton.h"

#include "SDK/ICvar.h"
#include "SDK/IBaseClientDLL.h"
#include "SDK/IClientEntityList.h"
#include "SDK/CInput.h"
#include "SDK/Entities.h"
#include "SDK/ISurface.h"
#include "SDK/GlowOutlineEffect.h"

class CClientMode;

class Interfaces : public Singleton<Interfaces>
{
	HMODULE vstdlib = findModule(L"vstdlib");
	HMODULE client_panorama = findModule(L"client_panorama.dll");
	HMODULE shaderapidx9 = findModule(L"shaderapidx9.dll");
	HMODULE vguimatsurface = findModule(L"vguimatsurface.dll");
public:
	ICvar* Cvar = findInterface<ICvar>(vstdlib, "VEngineCvar007");
	IBaseClientDLL* Client = findInterface<IBaseClientDLL>(client_panorama, "VClient018");
	CClientMode* ClientMode = **reinterpret_cast<CClientMode***>((*reinterpret_cast<uintptr_t**>(Client))[10] + 5);
	IClientEntityList* EntityList = findInterface<IClientEntityList>(client_panorama, "VClientEntityList003");
	ISurface* Surface = findInterface<ISurface>(vguimatsurface, "VGUI_Surface031");

	IDirect3DDevice9* D3DDevice9 = **(IDirect3DDevice9***)(Utils::PatternScan(shaderapidx9, "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);
	CInput* Input = *(CInput**)(Utils::PatternScan(client_panorama, "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10") + 1);
	C_LocalPlayer LocalPlayer = *(C_LocalPlayer*)(Utils::PatternScan(client_panorama, "8B 0D ? ? ? ? 83 FF FF 74 07") + 2);
	CGlowObjectManager* GlowObjectManager = *(CGlowObjectManager**)(Utils::PatternScan(client_panorama, "0F 11 05 ? ? ? ? 83 C8 01") + 3);

	static HMODULE findModule(const wchar_t* name) noexcept 
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
	static auto findInterface(HMODULE hModule, const char* name) noexcept
	{
		T* foundInterface = reinterpret_cast<std::add_pointer_t<T * (const char* name, int* returnCode)>>(GetProcAddress(hModule, "CreateInterface"))(name, nullptr);

		if (!foundInterface) {
			MessageBoxA(nullptr, (std::ostringstream{} << "Cannot find " << name << " interface!").str().c_str(), "Xesa", MB_OK | MB_ICONERROR);
			std::exit(EXIT_FAILURE);
		}

		return foundInterface;
	}
};
