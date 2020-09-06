#include "CollisionRect.h"
#include "../Entities/BaseEntity.h"

Rect::Rect() : vPos({}), vSize({}), pOwner(nullptr) {}
Rect::Rect(olc::vf2d s) : vSize(s) {}
Rect::Rect(olc::vf2d p, olc::vf2d s) : vPos(p), vSize(s) {}


olc::vf2d Rect::BRight() const
{
	return vPos + vSize;
}

olc::vf2d Rect::Middle() const
{
	return vPos + vSize / 2.0f;
}

Rect Rect::ExpandedBy(const Rect& r) const
{
	return Rect(vPos - r.vSize / 2.0f, vSize + r.vSize);
}

void Rect::Render(olc::vf2d camera_vPos, olc::PixelGameEngine* pge) const
{
	olc::Pixel color = (pOwner) ? olc::GREEN : olc::DARK_GREEN;
	pge->DrawRect(vPos - camera_vPos, vSize, color);
}

bool operator<(const olc::vf2d& v1, const olc::vf2d& v2) { return v1.x < v2.x&& v1.y < v2.y; }
bool operator>(const olc::vf2d& v1, const olc::vf2d& v2) { return v1.x > v2.x && v1.y > v2.y; }
olc::vf2d VecMin(const olc::vf2d& v1, const olc::vf2d& v2) { return { std::min(v1.x, v2.x), std::min(v1.y, v2.y) }; };
olc::vf2d VecMax(const olc::vf2d& v1, const olc::vf2d& v2) { return { std::max(v1.x, v2.x), std::max(v1.y, v2.y) }; };


bool ShapesOverlap(const Rect& r1, const Rect& r2)
{
	return (r1.vPos < r2.BRight() && r1.BRight() > r2.vPos);
}

olc::vf2d AmountOverlap(const Rect& r1, const Rect& r2)
{
	olc::vf2d o = VecMin(r1.BRight(), r2.BRight()) - VecMax(r1.vPos, r2.vPos);
	return (o.x < o.y) ?
		(r1.vPos.x <= r2.vPos.x) ? olc::vf2d{ -o.x, 0 } : olc::vf2d{ o.x, 0 } :
		(r1.vPos.y <= r2.vPos.y) ? olc::vf2d{ 0, -o.y } : olc::vf2d{ 0, o.y };
}

Collision Raycast(const Entity& e, const Rect& r, float fFrameRate)
{
	if (e.vVel.mag() < 0.01f)
		return Collision{ false };

	Ray ray = e.GetRay(fFrameRate);
	Rect rect = r.ExpandedBy(*e.pRect);

	// Cache division
	olc::vf2d vInvdir = 1.0f / ray.vDir;

	// Calculate intersections with rectangle bounding axes
	olc::vf2d t_near = (rect.vPos - ray.vOrigin) * vInvdir;
	olc::vf2d t_far = (rect.vPos + rect.vSize - ray.vOrigin) * vInvdir;
	if (std::isnan(t_far.y) || std::isnan(t_far.x)) return Collision{ false };
	if (std::isnan(t_near.y) || std::isnan(t_near.x)) return Collision{ false };

	// Sort distances
	if (t_near.x > t_far.x) std::swap(t_near.x, t_far.x);
	if (t_near.y > t_far.y) std::swap(t_near.y, t_far.y);
	if (t_near.x > t_far.y || t_near.y > t_far.x) return Collision{ false };

	// Closest 'time' will be the first contact
	float t_hit_near = std::max(t_near.x, t_near.y);
	float t_hit_far = std::min(t_far.x, t_far.y);
	if (t_hit_far < 0) return Collision{ false }; // ray is pointing away
	if (t_hit_near < 0.0f || t_hit_near > 1.0f) return Collision{ false };

	// Contact point of collision from parametric line equation
	olc::vf2d point = ray.vOrigin + ray.vDir * t_hit_near;
	olc::vf2d normal = (t_near.x > t_near.y) ?
		(vInvdir.x < 0) ? olc::vf2d{ 1, 0 } : olc::vf2d{ -1, 0 } :
		(vInvdir.y < 0) ? olc::vf2d{ 0, 1 } : olc::vf2d{ 0, -1 };

	return Collision{
		true,
		ray.pCaster,
		r.pOwner,
		normal,
		t_hit_near,
		point
	};
}
