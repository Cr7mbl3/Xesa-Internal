#pragma once

template<typename T, typename ...Args>
constexpr auto CallVFunction(void* base, int index, Args... args) {
	return ((*reinterpret_cast<T(__thiscall * **)(void*, Args...)>(base))[index])(base, args...);
}
