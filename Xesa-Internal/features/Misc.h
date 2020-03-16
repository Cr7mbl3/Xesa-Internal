#pragma once

class CUserCmd;

namespace Misc {

	namespace KeyBinds 
	{
		inline bool thirdpersonState;
	}

	void GrenadePrediction();
	void SniperCrosshair();
	void RecoilCrosshair();
	void BunnyHop(CUserCmd*);
	void Thirdperson();
	void FixAnimationLOD();

}