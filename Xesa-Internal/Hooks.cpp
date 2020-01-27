#include "Hooks.h"

#include "SDK/CUserCmd.h"
#include "Menu.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

namespace Hooks {

	void Initialize()
	{

		originalWndProc = WNDPROC(SetWindowLongA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(WndProc)));

		clientHook.setup(Interfaces::Get().ClientMode);
		direct3dHook.setup(Interfaces::Get().D3DDevice9);

		clientHook.hook_index(24, CreateMove);
		direct3dHook.hook_index(index::EndScene, EndScene);
		direct3dHook.hook_index(index::Reset, Reset);

		static ConVar* sv_cheats = Interfaces::Get().Cvar->FindVar("sv_cheats");
		sv_cheats->SetValue(1);
		static ConVar* cl_grenadepreview = Interfaces::Get().Cvar->FindVar("cl_grenadepreview");
		cl_grenadepreview->SetValue(1);
		static ConVar* weapon_debug_spread_show = Interfaces::Get().Cvar->FindVar("weapon_debug_spread_show");
		weapon_debug_spread_show->SetValue(3);
	}

	void Release()
	{
		clientHook.unhook_all();
		direct3dHook.unhook_all();

		SetWindowLongPtrA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndProc));

		static ConVar* sv_cheats = Interfaces::Get().Cvar->FindVar("sv_cheats");
		sv_cheats->SetValue(0);
		static ConVar* cl_grenadepreview = Interfaces::Get().Cvar->FindVar("cl_grenadepreview");
		cl_grenadepreview->SetValue(0);
		static ConVar* weapon_debug_spread_show = Interfaces::Get().Cvar->FindVar("weapon_debug_spread_show");
		weapon_debug_spread_show->SetValue(3);
	}

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) {
		if (msg == WM_KEYDOWN && LOWORD(wParam) == VK_INSERT) {
			Menu::Get().Toggle();
		}

		LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		if (Menu::Get().isOpened() && msg >= WM_INPUT && !ImGui_ImplWin32_WndProcHandler(window, msg, wParam, lParam))
			return true;

		return CallWindowProc(originalWndProc, window, msg, wParam, lParam);
	}

	bool __stdcall CreateMove(float inputSampleTime, CUserCmd* cmd) {
		static auto oCreateMove = clientHook.get_original<decltype(&CreateMove)>(index::CreateMove);
		return oCreateMove(inputSampleTime, cmd);
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

	
}
