#pragma once

#include <sstream>
#include <type_traits>
#include <Windows.h>
#include <d3d9.h>

#include "Utils.h"
#include "utils/Singleton.h"

#include "SDK/Entities.h"
#include "SDK/ICvar.h"
#include "SDK/IBaseClientDLL.h"
#include "SDK/IClientEntityList.h"
#include "SDK/CInput.h"
#include "SDK/ISurface.h"
#include "SDK/GlowOutlineEffect.h"
#include "SDK/GlobalVars.h"
#include "SDK/IVEngineClient.h"
#include "SDK/IPrediction.h"
#include "SDK/IEngineTrace.h"
#include "SDK/IVModelInfoClient.h"
#include "SDK/IPhysics.h"
#include "SDK/IClientMode.h"
#include "SDK/IInputSystem.h"
#include "SDK/IGameEventManager.h"
#include "SDK/IViewRenderBeams.h"
#include "SDK/IVModelRenderer.h"
#include "SDK/IMaterialSystem.h"

#define MODULE(name, libstr) inline HMODULE name = Interfaces::findModule(libstr)
#define SETUP_INTERFACE(type, name) inline type name = nullptr
#define UNDFEFINED_INTERFACE(type, name) inline type name
#define INIT_INTERFACE(name, type, modulename, version) name = Interfaces::findInterface<type>(modulename, version); interfaces_count++
#define OINIT_INTERFACE(name, type, base, index, offset) name = **reinterpret_cast<type***>((*reinterpret_cast<uintptr_t**>(base))[index] + offset); interfaces_count++
#define PATTERN_SCAN(name, type, module, pattern, offset) name = type(Utils::PatternScan(module, pattern) + offset); interfaces_count++;

typedef void* (__cdecl* InstantiateInterfaceFn)();
class IViewRender;

class InterfaceReg {
public:
	InstantiateInterfaceFn m_CreateFn;
	const char* m_pName;
	InterfaceReg* m_pNext;
};

namespace Interfaces {
	inline int interfaces_count;
	void Initialize();
	HMODULE findModule(const wchar_t*);
	template <typename T>
	T* findInterface(HMODULE, const char*);
}

namespace Modules {
	MODULE(vstdlib, L"vstdlib");
	MODULE(client_panorama, L"client_panorama.dll");
	MODULE(shaderapidx9, L"shaderapidx9.dll");
	MODULE(vguimatsurface, L"vguimatsurface.dll");
	MODULE(engine, L"engine.dll");
	MODULE(vphysics, L"vphysics.dll");
	MODULE(inputsystem, L"inputsystem.dll");
	MODULE(materialsystem, L"materialsystem.dll");
}

SETUP_INTERFACE(ICvar*, g_Cvar);
SETUP_INTERFACE(IBaseClientDLL*, g_Client);
SETUP_INTERFACE(IClientMode*, g_ClientMode);
SETUP_INTERFACE(IClientEntityList*, g_EntityList);
SETUP_INTERFACE(ISurface*, g_Surface);
SETUP_INTERFACE(IDirect3DDevice9*, g_D3DDevice9);
SETUP_INTERFACE(CInput*, g_Input);
SETUP_INTERFACE(CGlowObjectManager*, g_GlowObjectManager);
SETUP_INTERFACE(CGlobalVarsBase*, g_GlobalVars);
SETUP_INTERFACE(IVEngineClient*, g_Engine);
SETUP_INTERFACE(IPrediction*, g_Prediction);
SETUP_INTERFACE(CGameMovement*, g_GameMovement);
SETUP_INTERFACE(IMoveHelper*, g_MoveHelper);
SETUP_INTERFACE(IEngineTrace*, g_EngineTrace);
SETUP_INTERFACE(IVModelInfo*, g_ModelInfo);
SETUP_INTERFACE(IPhysicsSurfaceProps*, g_PhysicsSurface);
SETUP_INTERFACE(IInputSystem*, g_InputSystem);
SETUP_INTERFACE(IGameEventManager2*, g_GameEventManager);
SETUP_INTERFACE(IViewRenderBeams*, g_ViewRenderBeams);
SETUP_INTERFACE(IVModelRender*, g_ModelRender);
SETUP_INTERFACE(IMaterialSystem*, g_MaterialSystem);
SETUP_INTERFACE(IViewRender*, g_ViewRender);

UNDFEFINED_INTERFACE(C_LocalPlayer, g_LocalPlayer);
