#pragma once

#include "utils/Singleton.h"
#include <Windows.h>
#include "vfunc_hook.h"
#include "Interfaces.h"
#include "SDK/ClientFrameStage.h"

namespace Hooks
{
	namespace index
	{
		constexpr auto CreateMove = 24;
		constexpr auto d3d9_Present = 18;
		constexpr auto d3d9_EndScene = 42;
		constexpr auto d3d9_Reset = 16;
		constexpr auto LockCursor = 67;
		constexpr auto DoPostScreenEffects = 44;
		constexpr auto ConVar_GetBool = 13;
		constexpr auto FrameStageNotify = 37;
	}

	inline WNDPROC originalWndProc;

	inline vfunc_hook clientmodeHook;
	inline vfunc_hook direct3dHook;
	inline vfunc_hook surfaceHook;
	inline vfunc_hook svcheatsHook;
	inline vfunc_hook clientHook;

	void Initialize();
	void Release();

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
	bool __stdcall CreateMove(float inputSampleTime, CUserCmd* cmd);
	HRESULT __stdcall Present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);
	HRESULT __stdcall EndScene(IDirect3DDevice9* pDevice);
	HRESULT __stdcall Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	void __stdcall LockCursor();
	int __fastcall DoPostScreenEffects(void* _this, int edx, int a1);
	bool __fastcall SvCheatsGetBool(PVOID pConVar);
	void __stdcall FrameStageNotify(ClientFrameStage_t stage);
};
