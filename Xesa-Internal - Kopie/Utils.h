#pragma once

#include <Windows.h>
#include <stdint.h>

namespace Utils
{
	uint8_t* PatternScan(void* module, const char* signature);
}

