#pragma once
#include "../Utility/olcPixelGameEngine.h"
#include "../Math/CollisionRect.h"
#include "../Utility/Utility.h"
#include "../Utility/Timer.h"

struct Entity
{
	olc::vf2d vPos;
	olc::vf2d vVel;
	olc::vf2d vAcc;

	Rect* pRect;
	olc::vf2d vRectOffset;

	olc::Renderable image;
	olc::vf2d vImageOffset;

	Timer EntityTimer;

	float fCheckRadius;

	bool bKillMe;

	enum entity_type {
		PLAYER,
		APPLIANCE,
		BREAD,
		LOAF
	} type;

	// Constructor
	Entity(entity_type t, olc::vf2d p, Rect* r);
	~Entity();

	// Getters
	Ray GetRay(float fFrameRate) const;
	olc::vf2d GetRectSize() const;

	// Setters
	void SetCollisionRect(Rect* r);
	void LoadImage(const std::string& file_name);
	void Move(olc::vf2d pos_change);
	void AddContact(Entity* e);
	void ApplyImpulse(olc::vf2d normal, float time);

	// Physics Loop
	void ClearContacts();
	virtual void ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge) {}
	void UpdateKinematics(float fFrameRate);
	virtual void Update(float fFrameRate);
	virtual void Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug);
};







