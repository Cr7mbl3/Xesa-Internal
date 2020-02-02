#include "Hooks.h"

#include <intrin.h>

#include "SDK/CUserCmd.h"

#include "Menu.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#include "features/Misc.h"
#include "features/Glow.h"
#include "features/LagCompensation.h"
#include "features/EnginePrediction.h"

#include "Config.h"

namespace Hooks {

	void Initialize()
	{
		originalWndProc = WNDPROC(SetWindowLongA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(WndProc)));
		clientmodeHook.setup(g_ClientMode);
		direct3dHook.setup(g_D3DDevice9);
		surfaceHook.setup(g_Surface);
		svcheatsHook.setup(g_Cvar->FindVar("sv_cheats"));
		clientHook.setup(g_Client);

		clientmodeHook.hook_index(index::CreateMove, CreateMove);
		clientmodeHook.hook_index(index::DoPostScreenEffects, DoPostScreenEffects);
		direct3dHook.hook_index(index::d3d9_Present, Present);
		direct3dHook.hook_index(index::d3d9_EndScene, EndScene);
		direct3dHook.hook_index(index::d3d9_Reset, Reset);
		surfaceHook.hook_index(index::LockCursor, LockCursor);
		svcheatsHook.hook_index(index::ConVar_GetBool, SvCheatsGetBool);
		clientHook.hook_index(index::FrameStageNotify, FrameStageNotify);
	}

	void Release()
	{
		Glow::Release();

		clientmodeHook.unhook_all();
		direct3dHook.unhook_all();
		surfaceHook.unhook_all();
		svcheatsHook.unhook_all();
		clientHook.unhook_all();
		SetWindowLongPtrA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndProc));
	}

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		if (msg == WM_KEYDOWN && LOWORD(wParam) == VK_INSERT) {
			Menu::Get().Toggle();
		}

		LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		if (Menu::Get().isOpened() && ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
			return true;

		return CallWindowProc(originalWndProc, window, msg, wParam, lParam);
	}

	bool __stdcall CreateMove(float inputSampleTime, CUserCmd* cmd) 
	{
		static auto oCreateMove = clientmodeHook.get_original<decltype(&CreateMove)>(index::CreateMove);
		auto result = oCreateMove(inputSampleTime, cmd);

		if (!cmd || !cmd->command_number)
			return result;

		//sendPacket pointer from osiris
		uintptr_t* framePointer;
		__asm mov framePointer, ebp;
		bool& bSendPacket = *reinterpret_cast<bool*>(*framePointer - 0x1C);

		LagCompensation::Get().Update();

		static int counter = 0;
		counter++;
		if (counter > 64) { //To get a bit more performance, some features will be only called every 64 ticks
			counter = 0;
			Misc::GrenadePrediction();
			Misc::SniperCrosshair();
			Misc::RecoilCrosshair();
		}
		Misc::BunnyHop(cmd);

		EnginePrediction::StartPrediction(cmd); {



		} EnginePrediction::EndPrediction();

		LagCompensation::Get().FakeLag(bSendPacket);
		LagCompensation::Get().OnProcessCmd(cmd, bSendPacket);
		return result;
	}

	HRESULT __stdcall Present(IDirect3DDevice9* device, const RECT* src, const RECT* dest, HWND windowOverride, const RGNDATA* dirtyRegion)
	{
		static auto oPresent = direct3dHook.get_original<decltype(&Present)>(index::d3d9_Present);
		if (Menu::Get().isOpened() && config.menu_DisplayMode == DISPLAYMODE_PRESENT)
		{
			//pDevice methods pasted from Osiris
			device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE);
			IDirect3DVertexDeclaration9* vertexDeclaration;
			device->GetVertexDeclaration(&vertexDeclaration);

			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Menu::Get().Render();

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

			device->SetVertexDeclaration(vertexDeclaration);
			vertexDeclaration->Release();
		}

		return oPresent(device, src, dest, windowOverride, dirtyRegion);
	}

	HRESULT __stdcall EndScene(IDirect3DDevice9* pDevice) 
	{
		static auto oEndScene = direct3dHook.get_original<decltype(&EndScene)>(index::d3d9_EndScene);

		if (Menu::Get().isOpened() && config.menu_DisplayMode == DISPLAYMODE_ENDSCENE)
		{
			//pDevice methods pasted from CSGOSimple
			DWORD colorwrite, srgbwrite;
			IDirect3DVertexDeclaration9* vert_dec = nullptr;
			IDirect3DVertexShader9* vert_shader = nullptr;
			DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
			pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
			pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
			pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

			pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
			pDevice->GetVertexDeclaration(&vert_dec);
			pDevice->GetVertexShader(&vert_shader);
			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
			pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
			pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			Menu::Get().Render();

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
			pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
			pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
			pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
			pDevice->SetVertexDeclaration(vert_dec);
			pDevice->SetVertexShader(vert_shader);
		}
		return oEndScene(pDevice);
	}

	HRESULT __stdcall Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) 
	{
		static auto oReset = direct3dHook.get_original<decltype(&Reset)>(index::d3d9_Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(pDevice, pPresentationParameters);

		if(!FAILED(hr))
			Menu::Get().OnDeviceReset();

		return hr;
	}

	void __stdcall LockCursor() 
	{
		static auto oLockCursor = surfaceHook.get_original<decltype(&LockCursor)>(index::LockCursor);
		if (Menu::Get().isOpened()) {
			g_Surface->UnlockCursor();
			return;
		}
		oLockCursor();
	}

	int __fastcall DoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmodeHook.get_original<decltype(&DoPostScreenEffects)>(index::DoPostScreenEffects);

		if (g_LocalPlayer) {
			Glow::Run();
		}

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}

	bool __fastcall SvCheatsGetBool(PVOID pConVar)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");
		static auto oGetBool = svcheatsHook.get_original<bool(__thiscall*)(PVOID)>(index::ConVar_GetBool); //This crashes when called like other hooks? TODO: fix, this triggers me
		if (!oGetBool)
			return false;
		return reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think) || oGetBool(pConVar);
	}

	void __stdcall FrameStageNotify(ClientFrameStage_t stage)
	{
		static auto oFrameStageNotify = clientHook.get_original<decltype(&FrameStageNotify)>(index::FrameStageNotify);

		oFrameStageNotify(stage);
	}

	
}
