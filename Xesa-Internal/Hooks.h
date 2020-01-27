#pragma once

#include "Singleton.h"
#include <Windows.h>
#include "vfunc_hook.h"
#include "Interfaces.h"

namespace Hooks
{
	namespace index
	{
		constexpr auto CreateMove = 24;
		constexpr auto EndScene = 42;
		constexpr auto Reset = 16;
	}

	inline WNDPROC originalWndProc;

	inline vfunc_hook clientHook;
	inline vfunc_hook direct3dHook;

	void Initialize();
	void Release();

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
	bool __stdcall CreateMove(float inputSampleTime, CUserCmd* cmd);
	HRESULT __stdcall EndScene(IDirect3DDevice9* pDevice);
	HRESULT __stdcall Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
};
