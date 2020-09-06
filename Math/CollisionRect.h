#pragma once
#include "../Utility/olcPixelGameEngine.h"
#include "../Utility/Utility.h"

struct Entity;

struct Ray
{
	olc::vf2d vOrigin;
	olc::vf2d vDir;
	Entity* pCaster;
};

struct Rect
{
	olc::vf2d vPos;
	olc::vf2d vSize;

	Entity* pOwner = nullptr;
	std::vector<Entity*> vecContacts;

	Rect();
	Rect(olc::vf2d s);
	Rect(olc::vf2d p, olc::vf2d s);

	olc::vf2d BRight() const;
	olc::vf2d Middle() const;
	Rect ExpandedBy(const Rect& r) const;
	void Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge) const;
};

struct Collision
{
	bool bExists;
	Entity* pEntity;	// the colliding entity. not the collided-with.
	Entity* pWith;
	olc::vf2d vNormal;
	float fTime;
	olc::vf2d vContactPoint;
};

bool ShapesOverlap(const Rect& r1, const Rect& r2);
olc::vf2d AmountOverlap(const Rect& r1, const Rect& r2);
Collision Raycast(const Entity& e, const Rect& r, float fFrameRate);
