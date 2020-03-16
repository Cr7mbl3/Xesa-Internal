#pragma once

#include <deque>

#include "../SDK/IGameEventManager.h"
#include "../SDK/Entities.h"
#include "../utils/Singleton.h"
#include "../Interfaces.h"

struct Impact_Info_t
{
	Vector			start;
	Vector			pos;
	Color			color;
	C_BasePlayer* player;
	float			time;

	Impact_Info_t(const float time, const Vector start, const Vector pos, C_BasePlayer* player, const Color col = Color(255, 255, 255))
	{
		this->player = player;
		this->start = start;
		this->pos = pos;
		this->color = col;
		this->time = time;
	}

	float Length() const
	{
		return start.DistTo(this->pos);
	}

	bool operator==(const Impact_Info_t& info) const {
		return this->player == info.player && this->time == info.time;
	}
};

class BulletBeams : public Singleton<BulletBeams> {
public:
	void Initialize();
	void Release();

	void OnDoPostScreenEffects();
	void OnBulletImpact(IGameEvent*);

	class BulletBeamsListener : public IGameEventListener2 {
		virtual void	FireGameEvent(IGameEvent* event);
		int				GetEventDebugID() override { return EVENT_DEBUG_ID_INIT; };
	};
	BulletBeamsListener* listener = new BulletBeamsListener();
private:
	std::deque<Impact_Info_t> impacts;
};
