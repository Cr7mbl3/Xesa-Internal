#pragma once

#include <Windows.h>

#include "vfunc_hook.h"
#include "Interfaces.h"

#include "utils/Singleton.h"

#include "SDK/ClientFrameStage.h"
#include "SDK/IClientMode.h"
#include "SDK/IGameEventManager.h"

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
		constexpr auto OverrideView = 18;
		constexpr auto ListLeavesInBox = 6;
		constexpr auto RenderSmokeOverlay = 41;
		constexpr auto DrawModelExecute = 21;
	}

	inline WNDPROC originalWndProc;

	inline vfunc_hook clientmodeHook;
	inline vfunc_hook direct3dHook;
	inline vfunc_hook surfaceHook;
	inline vfunc_hook svcheatsHook;
	inline vfunc_hook clientHook;
	inline vfunc_hook viewrenderHook;
	inline vfunc_hook modelrenderHook;
	//inline vfunc_hook bspqueryHook;

	void Initialize();
	void Release();

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam);
	bool __stdcall CreateMove(float inputSampleTime, CUserCmd* cmd);
	HRESULT __stdcall Present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion);
	HRESULT __stdcall EndScene(IDirect3DDevice9* pDevice);
	HRESULT __stdcall Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	void __fastcall LockCursor(void* _this);
	int __fastcall DoPostScreenEffects(void* _this, int edx, int a1);
	bool __fastcall SvCheatsGetBool(PVOID pConVar);
	void __stdcall FrameStageNotify(ClientFrameStage_t stage);
	void __fastcall OverrideView(void*, int, CViewSetup*);
	int __stdcall ListLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax);
	void __fastcall RenderSmokeOverlay(uintptr_t ecx, uintptr_t edx, bool a1);
	void __fastcall DrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld);
};

namespace Vars 
{
	inline QAngle serverAngle;
	inline QAngle fakeAngle;
	inline QAngle lbyAngle;

	inline CUtlVector<float> sentPoseParameter;
	inline CUtlVector<CAnimationLayer> sentAnimOverlays;
	inline float sentAnimStateYaw;
}
