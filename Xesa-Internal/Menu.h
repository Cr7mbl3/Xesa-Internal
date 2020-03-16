#pragma once

#include "utils/Singleton.h"
#include "imgui/imgui.h"
#include "SDK/Color.h"

class Menu : public Singleton<Menu>
{
public:
	void Initialize();
	void Release();

	void OnDeviceLost();
	void OnDeviceReset();

	void Render();
	void Toggle();

	bool isOpened() const { return _visible; }

	bool isReleaseRequested() const { return _releaseRequested; }
private:
	void CreateStyle();

	ImGuiStyle	_style;
	bool		_visible;
	bool		_releaseRequested;

	void RenderAimTab();
	void RenderVisualsTab();
	void RenderPlayerTab();
	void RenderOthersTab();
};

namespace ImGui {

	bool BeginGroupBox(const char* name, const ImVec2& size_arg = ImVec2(0, 0));
	void EndGroupBox();

	bool ToggleButton(const char* label, bool* v, const ImVec2& size_arg);

	template<size_t N>
	void RenderTabs(const char* (&names)[N], int& active);

	template<size_t N>
	bool RenderSideBar(const char* const (&names)[N], int& active, bool same_line);

	template<size_t N>
	bool RenderSideBar(const char* const (&names)[N], int& active) { return RenderSideBar(names, active, true); };

	void HotKey(int& key);
}
