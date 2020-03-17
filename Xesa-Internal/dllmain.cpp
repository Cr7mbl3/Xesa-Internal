#include <windows.h>
#include <iostream>

#include "Interfaces.h"
#include "NetVars.h"
#include "Hooks.h"
#include "Menu.h"

#include "utils/TimeHelper.h"

DWORD WINAPI Release(LPVOID hModule) {
	Hooks::Release();
	Menu::Get().Release();
	std::cout << "Released!" << std::endl;

#ifdef _DEBUG
	FreeConsole();
#endif
	
	Sleep(100);
	FreeLibraryAndExitThread((HMODULE)hModule, EXIT_SUCCESS);
	return EXIT_SUCCESS;
}

DWORD WINAPI Initialize(LPVOID hModule) {
#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleW(L"Counter-Strike: Global Offensive");
	freopen_s(reinterpret_cast<FILE**>stdin, "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>stdout, "CONOUT$", "w", stdout);

	//display red warning bc console might get you untrusted (idk i heared it on yt)
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); //12 for dark red
	std::cout << "Warning: Do not use Debug Build on VAC secured servers!" << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //reset color
#endif
	TimeHelper startup_timer = TimeHelper();
	TimeHelper timer = TimeHelper();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10); //10 for green

	Interfaces::Initialize();

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14); //14 for yellow
	std::cout << Interfaces::interfaces_count << " Interfaces initialized in " << timer.GetMs() << "ms" << std::endl;
	timer.Reset();

	NetVars::Get().Initialize();
	std::cout << NetVars::Get().netvars << " NetVars dumped in " << timer.GetMs() << "ms" << std::endl;
	timer.Reset();

	Menu::Get().Initialize();
	Hooks::Initialize();
	std::cout << "Menu and Hooks initialized in " << timer.GetMs() << "ms" << std::endl;

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15); //reset color

	std::cout << "Finished in " << startup_timer.GetMs() << "ms!" << std::endl;

	std::cout << "Xesa alpha" << std::endl;
	std::cout << "Built on: " << __DATE__ << " " << __TIME__ << std::endl;

	while (!Menu::Get().isReleaseRequested()) {
		Sleep(100);
	}

	return Release(hModule);
}


BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD dReason, LPVOID lpReserved) {

	switch (dReason) {
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, Initialize, hModule, 0, nullptr);
		break;
	default:
		break;
	}

	return TRUE;
}