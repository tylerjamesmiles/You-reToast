#pragma once
#include "BaseEntity.h"
#include "Appliance.h"

struct Bread : public Entity
{
	Bread(olc::vf2d p, Rect* r);

	void ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge);
	void Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug);
};