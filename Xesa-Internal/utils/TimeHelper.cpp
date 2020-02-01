#include "TimeHelper.h"

void TimeHelper::Reset()
{
	_start = GetTickCount64();
}

ULONGLONG TimeHelper::GetMs()
{
	return GetTickCount64() - _start;
}

bool TimeHelper::HasReached(ULONGLONG ms)
{
	return GetMs() >= GetTickCount64();
}

bool TimeHelper::HasReachedReset(ULONGLONG ms)
{
	bool result = HasReached(ms);
	if (result)
		Reset();
	return result;
}
