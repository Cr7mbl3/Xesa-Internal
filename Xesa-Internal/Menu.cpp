#include "Menu.h"

#include <winerror.h>
#include <sstream>
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

		static const char* CategoryNames[] = {
			"Aim",
			"Visuals",
			"Player",
			"Other"
		};

		enum Category {
			CATEGORY_AIM,
			CATEGORY_VISUAL,
			CATEGORY_PLAYER,
			CATEGORY_OTHER
		};

		static int ActiveTab = CATEGORY_OTHER;
		ImGui::RenderTabs(CategoryNames, ActiveTab);

		switch (ActiveTab) {
		case CATEGORY_AIM:
			RenderAimTab();
			break;
		case CATEGORY_VISUAL:
			RenderVisualsTab();
			break;
		case CATEGORY_PLAYER:
			RenderPlayerTab();
			break;
		case CATEGORY_OTHER:
			RenderOthersTab();
			break;
		}

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

void Menu::RenderAimTab()
{
	static int ActiveTab = 0;
	static const char* TabNames[] = {
		"Legit",
		"Rage", 
		"Anti-Aim", 
		"Resolver"
	};
	enum Tabs {
		TAB_LEGIT,
		TAB_RAGE,
		TAB_ANTIAIM,
		TAB_RESOLVER
	};
	ImGui::RenderSideBar(TabNames, ActiveTab);

	ImGui::BeginGroupBox("#aim_content");
	{
		switch (ActiveTab) {
		case TAB_LEGIT:
			ImGui::TextDisabled("Nothing to see here");
			break;
		case TAB_RAGE:
			ImGui::Checkbox("Enable Ragebot", config.aim_ragebot_enable);
			ImGui::SliderFloat("Hitchance", config.aim_ragebot_hitchance, 0.f, 100.f, "%.0f%");
			ImGui::SliderFloat("Minimum Damage", config.aim_ragebot_min_damage, 0.1f, 100.f, "%.0f");
			break; 
		case TAB_ANTIAIM:
			ImGui::Checkbox("Enable AntiAim", config.aim_antiaim_enable);
			break;
		case TAB_RESOLVER:
			ImGui::Checkbox("Enable Resolver", config.aim_resolver_enable);		
			break;
		}

	}
	ImGui::EndGroupBox();
}

void Menu::RenderVisualsTab()
{
	static int ActiveTab = 0;
	static const char* TabNames[] = {
			"ESP",
			"Chams",
			"Removals",
			"Other"
	};
	enum Tabs {
		TAB_ESP,
		TAB_CHAMS,
		TAB_REMOVALS,
		TAB_OTHER,
	};
	ImGui::RenderSideBar(TabNames, ActiveTab);

	ImGui::BeginGroupBox("#visuals_content");
	{
		switch (ActiveTab) {
			case TAB_ESP:
				ImGui::Checkbox("Glow", config.visual_glow);
				break; 
			case TAB_CHAMS:
				ImGui::Text("Arms");
				ImGui::Checkbox("Arms Chams", config.visual_chams_arms);
				if (config.visual_chams_arms) {
					//ImGui::SameLine();
					//ImGui::ColorPicker("Arms Chams Color Visible", config.visual_chams_arms_visible, true);
					//ImGui::SameLine();
					//ImGui::ColorPicker("Arms Chams Color Occluded", config.visual_chams_arms_occluded, true);

					ImGui::Checkbox("Arms Ignore Z", config.visual_chams_arms_ingorez);
					ImGui::Checkbox("Arms Flat", config.visual_chams_arms_flat);
					ImGui::Checkbox("Arms Wireframe", config.visual_chams_arms_wireframe);
					ImGui::Checkbox("Arms Glass", config.visual_chams_arms_glass);
				}


				ImGui::Text("Enemies");
				ImGui::Checkbox("Enemies Chams", config.visual_chams_enemies);
				if (config.visual_chams_enemies) {
					//ImGui::SameLine();
					//ImGui::ColorPicker("Arms Chams Color Visible", config.visual_chams_arms_visible, true);
					//ImGui::SameLine();
					//ImGui::ColorPicker("Arms Chams Color Occluded", config.visual_chams_arms_occluded, true);

					ImGui::Checkbox("Enemies Ignore Z", config.visual_chams_enemies_ingorez);
					ImGui::Checkbox("Enemies Flat", config.visual_chams_enemies_flat);
					ImGui::Checkbox("Enemies Wireframe", config.visual_chams_enemies_wireframe);
					ImGui::Checkbox("Enemies Glass", config.visual_chams_enemies_glass);
				}
				break;
			case TAB_REMOVALS:
				ImGui::Checkbox("Remove Post Processing", config.visual_removals_postProcessing);
				ImGui::Checkbox("Remove Flash Effect", config.visual_removals_flash_effect);
				ImGui::Checkbox("Remove Smokes", config.visual_removals_smokes);
				ImGui::Checkbox("Remove Hands", config.visual_removals_hands);
				if(!config.visual_removals_hands)
					ImGui::Checkbox("Remove Sleeves", config.visual_removals_sleeves);
				ImGui::Checkbox("Remove Scope", config.visual_removals_scope);
				break;
			case TAB_OTHER:
				ImGui::Checkbox("Recoil Crosshair", config.visual_recoilCrosshair);
				ImGui::Checkbox("Sniper Crosshair", config.visual_sniperCrosshair);
				ImGui::Checkbox("Grenade Prediction", config.visual_grenadePrediction);
				ImGui::Checkbox("Bullet beams", config.visual_bulletBeams);
				break;
		}
	}
	ImGui::EndGroupBox();
}

void Menu::RenderPlayerTab()
{
	static int ActiveTab = 0;
	static const char* TabNames[] = {
		"Movement",
		"World",
		"Lag Compensation"
	};
	enum Tabs {
		TAB_MOVEMENT,
		WORLD,
		TAB_LAGCOMPENSATION,
	};
	ImGui::RenderSideBar(TabNames, ActiveTab);

	ImGui::BeginGroupBox("#player_content");
	{
		switch (ActiveTab) {
		case TAB_MOVEMENT:
			ImGui::Checkbox("Bunny Hop", config.misc_bhop);
			break;
		case WORLD:
			ImGui::Checkbox("Thirdperson", config.misc_thirdperson);
			ImGui::SameLine();
			ImGui::HotKey(*config.misc_thirdperson_key);
			if (config.misc_thirdperson) 
			{
				ImGui::SliderFloat("Thirdperson Distance", config.misc_thirdperson_distance, 100.f, 250.f, "%.0f");
			}
			break;
		case TAB_LAGCOMPENSATION:
			ImGui::Checkbox("Fake Lag", config.misc_lc_fakelag);
			ImGui::Checkbox("Disable Interpolation", config.misc_lc_disable_interpolation); 
			ImGui::Checkbox("Fix Animation LOD", config.misc_lc_fixAnimationLOD);
			ImGui::Checkbox("Disable Occlusion Check", config.misc_lc_disable_occlusion_check); //TODO: check if this should be under LagComp category
			break;
		}
		
	}
	ImGui::EndGroupBox();
}

void Menu::RenderOthersTab()
{
	static int ActiveTab = 0;
	static const char* TabNames[] = {
		"Cheat",
		"Menu"
	};
	enum Tabs {
		TAB_CHEAT,
		TAB_MENU,
	};
	ImGui::RenderSideBar(TabNames, ActiveTab);

	ImGui::BeginGroupBox("#other_content");
	{
		switch (ActiveTab) {
		case TAB_CHEAT:
			if (ImGui::Button("Unload", ImVec2{ 150, 25 })) {
				_releaseRequested = true;
			}
			break;
		case TAB_MENU:
			ImGui::Combo("Menu Render Mode", config.menu_DisplayMode, Menu_DisplayModeNames, IM_ARRAYSIZE(Menu_DisplayModeNames));
			break;
		}
	}
	ImGui::EndGroupBox();
}

//pasta from CSGOSimple
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

	window = ImGui::GetCurrentWindow();

	auto padding = ImGui::GetStyle().WindowPadding;

	auto text_size = ImGui::CalcTextSize(name, NULL, true);

	if (text_size.x > 1.0f) {
		window->DrawList->PushClipRectFullScreen();
		window->DrawList->PopClipRect();
	}

	return ret;
}

void ImGui::EndGroupBox()
{
	ImGui::EndChild();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DC.CursorPosPrevLine.y -= GImGui->FontSize / 2;
}

bool ImGui::ToggleButton(const char* label, bool* v, const ImVec2& size_arg)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = ImGui::GetColorU32((hovered && held || *v) ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
	ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	if (pressed)
		* v = !*v;

	return pressed;
}

template<size_t N>
void ImGui::RenderTabs(const char* (&names)[N], int& active)
{
	float button_width = (ImGui::GetCurrentWindow()->Size.x - ImGui::GetStyle().WindowPadding.x * (N + 1)) / N;
	float button_height = 25;
	bool values[N] = { false };

	values[active] = true;

	for (auto i = 0; i < N; ++i) {
		if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ button_width, button_height })) {
			active = i;
		}
		if (i < N - 1)
			ImGui::SameLine();
	}
}

template<size_t N>
bool ImGui::RenderSideBar(const char* const (&names)[N], int& active, bool same_line)
{
	ImGui::PushItemWidth(160.0f);
	bool result = ImGui::ListBox("", &active, names, IM_ARRAYSIZE(names));
	ImGui::PopItemWidth();
	if(same_line)
		ImGui::SameLine();
	return result;
}

//pasta from Osiris
void ImGui::HotKey(int& key)
{
	if (!g_InputSystem)
		return;

	if (key) 
	{
		ImGui::Text("[ %s ]", g_InputSystem->VirtualKeyToString(key));
	}
	else 
	{
		ImGui::TextUnformatted("[ key ]");
	}

	if (!ImGui::IsItemHovered())
		return;

	ImGui::SetTooltip("Press any key to change keybind");
	ImGuiIO& io = ImGui::GetIO();
	for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++)
		if (ImGui::IsKeyPressed(i) && i != VK_INSERT)
			key = i != VK_ESCAPE ? i : 0;

	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
		if (ImGui::IsMouseDown(i) && i + (i > 1 ? 2 : 1) != VK_INSERT)
			key = i + (i > 1 ? 2 : 1);
}