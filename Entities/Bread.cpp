#include "Bread.h"

Bread::Bread(olc::vf2d p, Rect* r) : Entity(BREAD, p, r) {
	image.Load("Resources/Art/Bread.png");
	vImageOffset = { 0, 0 };
}

void Bread::ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge)
{
	vAcc *= 0.90f;
	vVel = vAcc;
	if (vVel.mag() < 5.0f)
		bKillMe = true;
}

void Bread::Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug) {
	pge->DrawPartialDecal(
		vPos - camera_pos,
		image.Decal(),
		{ 0, 0 },
		{ 16, 16 }
	);
}