#include "Interfaces.h"

#include <iostream>

void Interfaces::Initialize()
{
	INIT_INTERFACE(g_Cvar, ICvar, Modules::vstdlib, "VEngineCvar0");
	INIT_INTERFACE(g_Client, IBaseClientDLL, Modules::client_panorama, "VClient0");
	INIT_INTERFACE(g_EntityList, IClientEntityList, Modules::client_panorama, "VClientEntityList0");
	INIT_INTERFACE(g_Surface, ISurface, Modules::vguimatsurface, "VGUI_Surface0");
	INIT_INTERFACE(g_Engine, IVEngineClient, Modules::engine, "VEngineClient0");
	INIT_INTERFACE(g_Prediction, IPrediction, Modules::client_panorama, "VClientPrediction0");
	INIT_INTERFACE(g_GameMovement, CGameMovement, Modules::client_panorama, "GameMovement0");
	INIT_INTERFACE(g_EngineTrace, IEngineTrace, Modules::engine, "EngineTraceClient0");
	INIT_INTERFACE(g_ModelInfo, IVModelInfoClient, Modules::engine, "VModelInfoClient0");
	INIT_INTERFACE(g_PhysicsSurface, IPhysicsSurfaceProps, Modules::vphysics, "VPhysicsSurfaceProps0");
	INIT_INTERFACE(g_InputSystem, IInputSystem, Modules::inputsystem, "InputSystemVersion0");
	INIT_INTERFACE(g_GameEventManager, IGameEventManager2, Modules::engine, "GAMEEVENTSMANAGER002");
	INIT_INTERFACE(g_ModelRender, IVModelRender, Modules::engine, "VEngineModel0");
	INIT_INTERFACE(g_MaterialSystem, IMaterialSystem, Modules::materialsystem, "VMaterialSystem0");
	
	OINIT_INTERFACE(g_ClientMode, IClientMode, g_Client, 10, 5);
	OINIT_INTERFACE(g_GlobalVars, CGlobalVarsBase, g_Client, 0, 27);
	OINIT_INTERFACE(g_ViewRender, IViewRender, g_Client, 28, 8);

	PATTERN_SCAN(g_D3DDevice9, **(IDirect3DDevice9***), Modules::shaderapidx9, "A1 ? ? ? ? 50 8B 08 FF 51 0C", 1);
	PATTERN_SCAN(g_Input, *(CInput**), Modules::client_panorama, "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10", 1);
	PATTERN_SCAN(g_LocalPlayer, *(C_LocalPlayer*), Modules::client_panorama, "8B 0D ? ? ? ? 83 FF FF 74 07", 2);
	PATTERN_SCAN(g_GlowObjectManager, *(CGlowObjectManager**), Modules::client_panorama, "0F 11 05 ? ? ? ? 83 C8 01", 3);
	PATTERN_SCAN(g_MoveHelper, **(IMoveHelper***), Modules::client_panorama, "8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01", 2);
	PATTERN_SCAN(g_ViewRenderBeams, *(IViewRenderBeams**), Modules::client_panorama, "8D 43 FC B9 ? ? ? ? 50 A1", 4);
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
	//https://www.unknowncheats.me/forum/counterstrike-global-offensive/165359-easy-createinterface.html
	ULONG CreateInterface = (ULONG)GetProcAddress(hModule, "CreateInterface");
	ULONG ShortJump = (ULONG)CreateInterface + 5;
	ULONG Jump = (((ULONG)CreateInterface + 5) + *(ULONG*)ShortJump) + 4;
	InterfaceReg* interface_reg = **(InterfaceReg***)(Jump + 6);

	do {
		if (interface_reg)
		{
			if (strstr(interface_reg->m_pName, name) && (strlen(interface_reg->m_pName) - strlen(name)) < 5)
			{
				T* foundInterface = reinterpret_cast<std::add_pointer_t<T * (const char* name, int* returnCode)>>(CreateInterface)(interface_reg->m_pName, nullptr);
				if (!foundInterface) {
					continue;
				}
				std::cout << "Found " << interface_reg->m_pName << " for " << name << std::endl;
				return foundInterface;
			}
		}

	} while (interface_reg = interface_reg->m_pNext);

	MessageBoxA(nullptr, (std::ostringstream{} << "Cannot find " << name << " interface!").str().c_str(), "Xesa", MB_OK | MB_ICONERROR);
	std::exit(EXIT_FAILURE);
}