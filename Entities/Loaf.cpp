#include "Loaf.h"

Loaf::Loaf(olc::vf2d p, Rect* r) : Entity(LOAF, p, r)
{
	image.Load("Resources/Art/Loaf.png");
	vImageOffset = { 0, 0 };
}

Loaf::~Loaf()
{
	delete pRect;
}

void Loaf::Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug)
{
	pge->DrawDecal(vPos - camera_pos, image.Decal());
}




