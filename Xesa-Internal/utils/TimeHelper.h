#pragma once

#include <windows.h>

class TimeHelper {
private:
	ULONGLONG _start;
public:
	TimeHelper() : _start(GetTickCount64()) {

	}
	void Reset();
	ULONGLONG GetMs();
	bool HasReached(ULONGLONG);
	bool HasReachedReset(ULONGLONG);
};