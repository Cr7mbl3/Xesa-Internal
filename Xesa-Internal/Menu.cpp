#include "Menu.h"

#include <winerror.h>
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"
#include "Interfaces.h"
#include "Config.h"


void Menu::Initialize()
{

	ImGui::CreateContext();

	D3DDEVICE_CREATION_PARAMETERS params;

	if (FAILED(g_D3DDevice9->GetCreationParameters(&params)))
		throw std::runtime_error("[InputSys] GetCreationParameters failed.");

	ImGui_ImplWin32_Init(params.hFocusWindow);
	ImGui_ImplDX9_Init(g_D3DDevice9);

	CreateStyle();

	_visible = true;
}

void Menu::Release()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
	ImGui::GetIO().MouseDrawCursor = _visible;

	if (!_visible)
		return;
	
	ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ 1000, 420 }, ImGuiSetCond_Once);

	if (ImGui::Begin("Xesa",
		&_visible,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize)) {

		ImGui::BeginGroupBox("##body_content");
		{
			ImGui::Checkbox("Glow", config.visual_glow);
			ImGui::Checkbox("Bunny Hop", config.misc_bhop);
			ImGui::Checkbox("Recoil Crosshair", config.visual_recoilCrosshair);
			ImGui::Checkbox("Sniper Crosshair", config.visual_sniperCrosshair);
			ImGui::Checkbox("Grenade Prediction", config.visual_grenadePrediction);
		}
		ImGui::EndGroupBox();

		ImGui::End();
	}
}

void Menu::Toggle()
{
	_visible = !_visible;
}

void Menu::CreateStyle()
{
	//Thanks to Extasy Hosting https://www.unknowncheats.me/forum/members/576479.html for releasing this style in UC
	//https://www.unknowncheats.me/forum/c-and-c-/189635-imgui-style-settings.html
	
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoTooltip);
	_style.WindowPadding = ImVec2(13, 13);
	_style.WindowRounding = 5.0f;
	_style.FramePadding = ImVec2(5, 5);
	_style.FrameRounding = 4.0f;
	_style.ItemSpacing = ImVec2(12, 8);
	_style.ItemInnerSpacing = ImVec2(8, 6);
	_style.IndentSpacing = 25.0f;
	_style.ScrollbarSize = 15.0f;
	_style.ScrollbarRounding = 9.0f;
	_style.GrabMinSize = 5.0f;
	_style.GrabRounding = 3.0f;

	ImGui::GetStyle() = _style;

	ImGui::StyleColorsXesa();
	
	
}

bool ImGui::BeginGroupBox(const char* name, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;

	window->DC.CursorPos.y += GImGui->FontSize / 2;
	const ImVec2 content_avail = ImGui::GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	if (size.x <= 0.0f) {
		size.x = ImMax(content_avail.x, 4.0f) - fabsf(size.x); // Arbitrary minimum zero-ish child size of 4.0f (0.0f causing too much issues)
	}
	if (size.y <= 0.0f) {
		size.y = ImMax(content_avail.y, 4.0f) - fabsf(size.y);
	}

	ImGui::SetNextWindowSize(size);
	bool ret;
	ImGui::Begin(name, &ret, flags);
	//bool ret = ImGui::Begin(name, NULL, size, -1.0f, flags);

	window = ImGui::GetCurrentWindow();

	auto padding = ImGui::GetStyle().WindowPadding;

	auto text_size = ImGui::CalcTextSize(name, NULL, true);

	if (text_size.x > 1.0f) {
		window->DrawList->PushClipRectFullScreen();
		//window->DrawList->AddRectFilled(window->DC.CursorPos - ImVec2{ 4, 0 }, window->DC.CursorPos + (text_size + ImVec2{ 4, 0 }), GetColorU32(ImGuiCol_ChildWindowBg));
		//RenderTextClipped(pos, pos + text_size, name, NULL, NULL, GetColorU32(ImGuiCol_Text));
		window->DrawList->PopClipRect();
	}
	//if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
	//	ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

	return ret;
}

void ImGui::EndGroupBox()
{
	ImGui::EndChild();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DC.CursorPosPrevLine.y -= GImGui->FontSize / 2;
}