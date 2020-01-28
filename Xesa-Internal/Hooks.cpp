#include "Hooks.h"

#include "SDK/CUserCmd.h"

#include "Menu.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#include "features/Misc.h"
#include "features/Glow.h"

namespace Hooks {

	void Initialize()
	{
		originalWndProc = WNDPROC(SetWindowLongA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(WndProc)));
		clientmodeHook.setup(Interfaces::Get().ClientMode);
		direct3dHook.setup(Interfaces::Get().D3DDevice9);
		surfaceHook.setup(Interfaces::Get().Surface);

		clientmodeHook.hook_index(index::CreateMove, CreateMove);
		clientmodeHook.hook_index(index::DoPostScreenEffects, DoPostScreenEffects);
		direct3dHook.hook_index(index::EndScene, EndScene);
		direct3dHook.hook_index(index::Reset, Reset);
		surfaceHook.hook_index(index::LockCursor, LockCursor);
	}

	void Release()
	{
		Glow::Release();
		Misc::SpoofSvCheats(0);

		clientmodeHook.unhook_all();
		direct3dHook.unhook_all();
		surfaceHook.unhook_all();
		SetWindowLongPtrA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndProc));
	}

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_KEYDOWN && LOWORD(wParam) == VK_INSERT) {
			Menu::Get().Toggle();
		}

		LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		if (Menu::Get().isOpened() && /*msg >= WM_INPUT && */ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
			return true;

		return CallWindowProc(originalWndProc, window, msg, wParam, lParam);
	}

	bool __stdcall CreateMove(float inputSampleTime, CUserCmd* cmd) {
		static auto oCreateMove = clientmodeHook.get_original<decltype(&CreateMove)>(index::CreateMove);
		auto result = oCreateMove(inputSampleTime, cmd);

		static int counter = 0;
		counter++;
		if (counter > 64) { //To get a bit more performance, some features will be only called every 64 ticks
			counter = 0;
			Misc::SpoofSvCheats(1);
			Misc::GrenadePrediction();
			Misc::SniperCrosshair();
			Misc::RecoilCrosshair();
		}
		Misc::BunnyHop(cmd);
		return result;
	}

	HRESULT __stdcall EndScene(IDirect3DDevice9* pDevice) {
		static auto oEndScene = direct3dHook.get_original<decltype(&EndScene)>(index::EndScene);

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
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


		return oEndScene(pDevice);

	}

	HRESULT __stdcall Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
		static auto oReset = direct3dHook.get_original<decltype(&Reset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(pDevice, pPresentationParameters);

		if(!FAILED(hr))
			Menu::Get().OnDeviceReset();

		return hr;
	}

	void __stdcall LockCursor() {
		static auto oLockCursor = surfaceHook.get_original<decltype(&LockCursor)>(index::LockCursor);
		if (Menu::Get().isOpened()) {
			Interfaces::Get().Surface->UnlockCursor();
			return;
		}
		oLockCursor();
	}

	int __fastcall DoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmodeHook.get_original<decltype(&DoPostScreenEffects)>(index::DoPostScreenEffects);

		if (Interfaces::Get().LocalPlayer) {
			Glow::Run();
		}

		return oDoPostScreenEffects(Interfaces::Get().ClientMode, edx, a1);
	}

	
}
