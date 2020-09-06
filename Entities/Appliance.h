#pragma once
#include "BaseEntity.h"
#include "../Math/CollisionRect.h"

struct Appliance : public Entity
{
	float fSpeed = 0.5f;
	float fMaxSpeed = 130.0f;
	bool bAsleep = false;
	olc::vf2d vAvgPos = olc::vf2d{};
	olc::vf2d vPlayerPos = olc::vf2d{};

	Timer SleepTimer;

	Appliance(olc::vf2d p, Rect* r);

	void ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge);

	virtual void Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug);
};