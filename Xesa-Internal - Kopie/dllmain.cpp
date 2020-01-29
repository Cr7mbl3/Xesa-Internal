#include <windows.h>
#include <iostream>

#include "NetVars.h"
#include "Hooks.h"
#include "Menu.h"

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
	ULONGLONG startup = GetTickCount64();
#ifdef _DEBUG
	AllocConsole();
	SetConsoleTitleW(L"Counter-Strike: Global Offensive");
	freopen_s(reinterpret_cast<FILE**>stdin, "CONIN$", "r", stdin);
	freopen_s(reinterpret_cast<FILE**>stdout, "CONOUT$", "w", stdout);

	//display red warning bc console can get you untrusted (idk i heared it on yt)
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12); //12 for dark red
	std::cout << "Warning: Do not use Debug Build on VAC secured servers!" << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
#endif
	ULONGLONG start = GetTickCount64();
	NetVars::Get().Initialize();
	std::cout << NetVars::Get().netvars << " NetVars dumped in " << GetTickCount64() - start << "ms" << std::endl;
	start = GetTickCount64();
	Menu::Get().Initialize();
	Hooks::Initialize();
	std::cout << "Menu and Hooks initialized in " << GetTickCount64() - start << "ms" << std::endl;
	std::cout << "Finished in " << GetTickCount64() - startup << "ms!" << std::endl;

	std::cout << "Xesa alpha" << std::endl;
	std::cout << "Built on: " << __DATE__ << " " << __TIME__ << std::endl;
	

	while (!GetAsyncKeyState(VK_DELETE)) {
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