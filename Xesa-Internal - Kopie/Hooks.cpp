#include "Hooks.h"

#include <intrin.h>
#include <algorithm>
#include <iostream>

#include "Menu.h"
#include "Config.h"

#include "utils/Math.h"

#include "SDK/CUserCmd.h"
#include "SDK/IPhysics.h"

#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#include "features/Misc.h"
#include "features/Glow.h"
#include "features/LagCompensation.h"
#include "features/EnginePrediction.h"
#include "features/Aimbot.h"
#include "features/AntiAim.h"
#include "features/BulletBeams.h"
#include "features/Chams.h"

namespace Hooks {

	void Initialize()
	{
		originalWndProc = WNDPROC(SetWindowLongA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(WndProc)));
		clientmodeHook.setup(g_ClientMode);
		direct3dHook.setup(g_D3DDevice9);
		surfaceHook.setup(g_Surface);
		svcheatsHook.setup(g_Cvar->FindVar("sv_cheats"));
		clientHook.setup(g_Client);
		viewrenderHook.setup(g_ViewRender);
		modelrenderHook.setup(g_ModelRender);
		//bspqueryHook.setup(g_Engine->GetBSPTreeQuery());

		clientmodeHook.hook_index(index::CreateMove, CreateMove);
		clientmodeHook.hook_index(index::DoPostScreenEffects, DoPostScreenEffects);
		direct3dHook.hook_index(index::d3d9_Present, Present);
		direct3dHook.hook_index(index::d3d9_EndScene, EndScene);
		direct3dHook.hook_index(index::d3d9_Reset, Reset);
		surfaceHook.hook_index(index::LockCursor, LockCursor);
		svcheatsHook.hook_index(index::ConVar_GetBool, SvCheatsGetBool);
		clientHook.hook_index(index::FrameStageNotify, FrameStageNotify);
		clientmodeHook.hook_index(index::OverrideView, OverrideView);
		viewrenderHook.hook_index(index::RenderSmokeOverlay, RenderSmokeOverlay);
		modelrenderHook.hook_index(index::DrawModelExecute, DrawModelExecute);
		//bspqueryHook.hook_index(index::ListLeavesInBox, ListLeavesInBox);

		BulletBeams::Get().Initialize();
	}

	void Release()
	{
		Glow::Release();
		BulletBeams::Get().Release();

		clientmodeHook.unhook_all();
		direct3dHook.unhook_all();
		surfaceHook.unhook_all();
		svcheatsHook.unhook_all();
		clientHook.unhook_all();
		viewrenderHook.unhook_all();
		modelrenderHook.unhook_all();
		SetWindowLongPtrA(FindWindowW(L"Valve001", nullptr), GWLP_WNDPROC, LONG_PTR(originalWndProc));
	}

	LRESULT __stdcall WndProc(HWND window, UINT msg, WPARAM wParam, LPARAM lParam) 
	{
		if (msg == WM_KEYDOWN && LOWORD(wParam) == VK_INSERT) {
			Menu::Get().Toggle();
			if (Menu::Get().isOpened()) {
				ImGui::GetIO().MouseDown[0] = false;
				//TODO: g_InputSystem->ResetInputState();
			}
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

		float yaw = cmd->viewangles.yaw;

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

		LagCompensation::Get().FakeLag(bSendPacket);

		AntiAim::Get().Run(cmd, bSendPacket);

		EnginePrediction::StartPrediction(cmd); {

			Aimbot::Get().Run(cmd, bSendPacket);

		} EnginePrediction::EndPrediction();

		//Movement fix
		cmd->viewangles.Normalize();
		auto degreesToRadians = [](float degrees) constexpr noexcept { return degrees * static_cast<float>(3.14159265358979323846) / 180.0f; }; //TODO: Use Math.h DEG2RAD instead
		float oldYaw = yaw + (yaw < 0.0f ? 360.0f : 0.0f);
		float newYaw = cmd->viewangles.yaw + (cmd->viewangles.yaw < 0.0f ? 360.0f : 0.0f);
		float yawDelta = newYaw < oldYaw ? fabsf(newYaw - oldYaw) : 360.0f - fabsf(newYaw - oldYaw);
		yawDelta = 360.0f - yawDelta;
		const float forwardmove = cmd->forwardmove;
		const float sidemove = cmd->sidemove;
		cmd->forwardmove = std::cos(degreesToRadians(yawDelta)) * forwardmove + std::cos(degreesToRadians(yawDelta + 90.0f)) * sidemove;
		cmd->sidemove = std::sin(degreesToRadians(yawDelta)) * forwardmove + std::sin(degreesToRadians(yawDelta + 90.0f)) * sidemove;

		//Angle clamping
		//TODO: mby add pitch clamping
		float vayaw = cmd->viewangles.yaw;

		while (vayaw < -180.f)
			vayaw += 180.f * 2;
		while (vayaw > 180.f)
			vayaw -= 180.f * 2;

		cmd->viewangles.yaw = vayaw;
		cmd->viewangles.roll = 0.0f;
		cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
		cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);

		if (g_LocalPlayer && g_LocalPlayer->IsAlive()) {
			Vars::lbyAngle = QAngle(cmd->viewangles.pitch, g_LocalPlayer->m_flLowerBodyYawTarget(), 0.f);

			static int sendPackets = 0;

			if (!bSendPacket || sendPackets > 16) {
				Vars::serverAngle = cmd->viewangles;
			}
			if (bSendPacket) {
				Vars::fakeAngle = cmd->viewangles;
			}

			if (bSendPacket)
				sendPackets++;
			else
				sendPackets = 0;
		}
		LagCompensation::Get().OnProcessCmd(cmd, bSendPacket);
		return false;
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

		static bool removedFlash = false;

		if (config.visual_removals_flash_effect != removedFlash) 
		{
			IMaterial* flash = g_MaterialSystem->FindMaterial("effects\\flashbang", TEXTURE_GROUP_CLIENT_EFFECTS);
			IMaterial* flashWhite = g_MaterialSystem->FindMaterial("effects\\flashbang_white", TEXTURE_GROUP_CLIENT_EFFECTS);

			if (flash && flashWhite)
			{
				flash->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, config.visual_removals_flash_effect);
				flashWhite->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, config.visual_removals_flash_effect);

			}
			removedFlash = config.visual_removals_flash_effect;
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

	void __fastcall LockCursor(void* _this)
	{
		static auto oLockCursor = surfaceHook.get_original<decltype(&LockCursor)>(index::LockCursor);
		if (Menu::Get().isOpened()) {
			g_Surface->UnlockCursor();
			return;
		}
		oLockCursor(g_Surface);
	}

	int __fastcall DoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmodeHook.get_original<decltype(&DoPostScreenEffects)>(index::DoPostScreenEffects);

		BulletBeams::Get().OnDoPostScreenEffects();


		if (g_Engine->IsInGame()) {
			//TODO: Move to visuals class
			static auto disablePostProcessing = *(bool**)(Utils::PatternScan(Modules::client_panorama, "83 EC 4C 80 3D") + 5);
			*disablePostProcessing = config.visual_removals_postProcessing; //TODO: Check if it is still disabled after releasing
		}

		Glow::Run();

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}

	bool __fastcall SvCheatsGetBool(PVOID pConVar)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");
		static auto oGetBool = svcheatsHook.get_original<bool(__thiscall*)(PVOID)>(index::ConVar_GetBool); //This crashes when called like other hooks? TODO: fix, this triggers me //mby change fastcall to thiscall?
		if (!oGetBool)
			return false;
		return reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think) || oGetBool(pConVar);
	}

	void __stdcall FrameStageNotify(ClientFrameStage_t stage)
	{
		static auto oFrameStageNotify = clientHook.get_original<decltype(&FrameStageNotify)>(index::FrameStageNotify);

		if (g_Engine->IsInGame()) 
		{
			switch (stage)
			{
			case FRAME_RENDER_START:
				if (g_Engine->IsInGame())
				{
					if (g_LocalPlayer && g_LocalPlayer->IsAlive() && Misc::KeyBinds::thirdpersonState) {
						g_Prediction->SetLocalViewangles(Vars::serverAngle);

						//TODO: Thirdperson anim fix
						//mby https://www.unknowncheats.me/forum/counterstrike-global-offensive/348353-accurate-local-player.html
					}

					static bool removedSmokes = false;

					if (config.visual_removals_smokes != removedSmokes) {
						/*TODO: Check if this is needed
						static DWORD smokeCount = *(DWORD*)Utils::PatternScan(Modules::client_panorama, "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0") + 8;
						//std::cout << smokeCount << std::endl;
						smokeCount = 0;
						*/
						//pasta from DEADCELL
						static const std::vector< const char* > vistasmoke_mats = {
							"particle/vistasmokev1/vistasmokev1_fire",
							"particle/vistasmokev1/vistasmokev1_smokegrenade",
							"particle/vistasmokev1/vistasmokev1_emods",
							"particle/vistasmokev1/vistasmokev1_emods_impactdust",
						};
						if (g_Engine->IsConnected()) {
							for (auto mat_s : vistasmoke_mats) {
								IMaterial* mat = g_MaterialSystem->FindMaterial(mat_s, TEXTURE_GROUP_OTHER);
								mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
								mat->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, config.visual_removals_smokes);
							}
						}
					}



					Misc::FixAnimationLOD();
				}
				break;
			case FRAME_NET_UPDATE_END:
				LagCompensation::Get().OnFrameNetUpdateEnd();
				break;
			}
		}

		oFrameStageNotify(stage);
	}

	void __fastcall OverrideView(void* _this, int edx, CViewSetup* viewSetup)
	{
		static auto oOverrideView = clientmodeHook.get_original<void(__thiscall*)(void*, CViewSetup*)>(index::OverrideView);
		if (g_Engine->IsInGame() && viewSetup) {
			Misc::Thirdperson();
		}
		oOverrideView(g_ClientMode, viewSetup);
	}

	int __stdcall ListLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax)
	{
		static auto listLeaves = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");

		return 0;
	}

	void __fastcall RenderSmokeOverlay(uintptr_t ecx, uintptr_t edx, bool a1)
	{
		static auto oRenderSmokeOverlay = viewrenderHook.get_original<void(__thiscall*)(uintptr_t, bool)>(index::RenderSmokeOverlay);
		if(!config.visual_removals_smokes)
			oRenderSmokeOverlay(ecx, a1);
	}

	//pppppasta from CSGOSimple
	void __fastcall DrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = modelrenderHook.get_original<decltype(&DrawModelExecute)>(index::DrawModelExecute);

		if (g_ModelRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
		}

		auto mdl = pInfo.pModel;
		
		if (strstr(mdl->szName, "models/player") != nullptr) {
			//C_BasePlayer* player = (C_BasePlayer*) g_EntityList->GetClientEntity(pInfo.entity_index);
			//TODO: Mby add 'no rendering for mates/enemies...'
		}
		else if (strstr(mdl->szName, "sleeve") != nullptr && config.visual_removals_sleeves) {

			auto material = g_MaterialSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
			if (!material)
				return;

			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			g_ModelRender->ForcedMaterialOverride(material);
		}
		else if (strstr(mdl->szName, "arms") != nullptr && config.visual_removals_hands) {
			auto material = g_MaterialSystem->FindMaterial(mdl->szName, TEXTURE_GROUP_MODEL);
			if (!material)
				return;

			material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			g_ModelRender->ForcedMaterialOverride(material);
		}

		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

		g_ModelRender->ForcedMaterialOverride(nullptr);
	}

}
