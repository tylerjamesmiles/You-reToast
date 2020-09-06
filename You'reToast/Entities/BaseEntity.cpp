#include "BaseEntity.h"

Entity::Entity(entity_type t, olc::vf2d p, Rect* r) :
	vPos(p),
	vVel({}),
	type(t),
	bKillMe(false),
	fCheckRadius(50.0f),
	EntityTimer(0.1f)
{
	SetCollisionRect(r);
}

Entity::~Entity()
{
	delete pRect;
}

Ray Entity::GetRay(float fFrameRate) const 
{ 
	return Ray{ pRect->Middle(), vVel * fFrameRate, pRect->pOwner }; 
}

olc::vf2d Entity::GetRectSize() const 
{ 
	return pRect->vSize; 
}

void Entity::SetCollisionRect(Rect* r) 
{ 
	pRect = r; 
	r->pOwner = this; 
}

void Entity::LoadImage(const std::string& file_name) 
{ 
	image.Load(file_name); 
}

void Entity::Move(olc::vf2d pos_change) 
{ 
	vPos += pos_change; 
	pRect->vPos = vPos + vRectOffset; 
}

void Entity::AddContact(Entity* e) 
{ 
	pRect->vecContacts.push_back(e); 
}

void Entity::ApplyImpulse(olc::vf2d normal, float time) 
{ 
	vVel += normal * olc::vf2d(std::abs(vVel.x), std::abs(vVel.y)) * (1 - time); 
}

void Entity::ClearContacts() 
{ 
	pRect->vecContacts.clear(); 
}

void Entity::UpdateKinematics(float fFrameRate)
{
	Move(vVel * fFrameRate);
	EntityTimer.Update(fFrameRate);
}

void Entity::Update(float fFrameRate) 
{ 
	UpdateKinematics(fFrameRate); 
}

void Entity::Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug)
{
	bool bRight = (vVel.x > 0);
	float fSourceDir = (bRight) ? 0.0f : 16.0f;
	vImageOffset = (bRight) ? olc::vf2d{ -18, -10 } : olc::vf2d{ -4, -10 };

	if (image.Decal())
		pge->DrawPartialDecal(
			vPos + vImageOffset - camera_pos,
			image.Decal(),
			{ (float)(EntityTimer.nFrame % 6) * 32.0f, fSourceDir },
			{ 32, 16 }
	);

	if (render_debug) {
		pge->DrawCircle(vPos - camera_pos, 2, olc::DARK_GREEN);
		pRect->Render(camera_pos, pge);
		pge->DrawCircle(pRect->Middle(), fCheckRadius, olc::GREEN);

		//for (Entity* e : pRect->vecContacts)
		//	pge->DrawLine(pRect->Middle() - camera_pos, e->pRect->Middle() - camera_pos, olc::RED);
	}
}