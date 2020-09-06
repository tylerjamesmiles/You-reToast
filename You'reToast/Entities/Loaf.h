#pragma once
#include "BaseEntity.h"

struct Loaf : public Entity
{
	Loaf(olc::vf2d p, Rect* r);
	~Loaf();

	void Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug);
};