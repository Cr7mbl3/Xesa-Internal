#pragma once

#include "../vfunc.h"

class IInputSystem
{
public:
	//all pasta from Osiris
	constexpr auto ButtonCodeToString(int buttonCode)
	{
		return CallVFunction<const char*, int>(this, 40, buttonCode);
	}

	constexpr auto VirtualKeyToButtonCode(int virtualKey)
	{
		if (virtualKey <= VK_XBUTTON2) {
			if (virtualKey > VK_CANCEL) 
				virtualKey--;
			return virtualKey + 106;
		}
		return CallVFunction<int, int>(this, 45, virtualKey);
	}

	constexpr auto VirtualKeyToString(int virtualKey)
	{
		return ButtonCodeToString(VirtualKeyToButtonCode(virtualKey));
	}
};