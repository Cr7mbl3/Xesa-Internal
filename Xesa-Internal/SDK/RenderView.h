#pragma once

#include "../vfunc.h"
#include "Color.h"

//pasta from Pener SDK
class CRenderView
{
public:
	void SetBlend(float alpha)
	{
		CallVFunction<void, float>(this, 4, alpha);
	}
};