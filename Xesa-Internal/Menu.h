#pragma once

#include "utils/Singleton.h"
#include "imgui/imgui.h"

class Menu : public Singleton<Menu>
{
public:
	void Initialize();
	void Release();

	void OnDeviceLost();
	void OnDeviceReset();

	void Render();
	void Toggle();

	bool isOpened() const { return _visible; };
private:
	void CreateStyle();

	ImGuiStyle	_style;
	bool		_visible;
};

namespace ImGui {
	bool BeginGroupBox(const char* name, const ImVec2& size_arg = ImVec2(0, 0));
	void EndGroupBox();
}
