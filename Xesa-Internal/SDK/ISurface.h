#pragma once

#include "../vfunc.h"

class ISurface {
public:
	void UnlockCursor() {
		CallVFunction<void>(this, 66);
	}
};