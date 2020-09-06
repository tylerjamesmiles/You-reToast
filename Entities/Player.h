#pragma once
#include "BaseEntity.h"
#include "Bread.h"

struct Player : public Entity
{
	float fSpeed = 100.0f;
	int nAmmo = 0;
	int nSoulsHarvested = 0;
	int nHP = 100;
	int nAmtToHeal = 0;
	int nAmtAmmoGained = 0;

	olc::Renderable cursor;
	olc::Renderable hud;

	Timer ShootyTimer;
	Timer BloodShowTimer;
	Timer HealTimer;
	Timer AmmoTimer;

	Player(olc::vf2d p, Rect* r);
	~Player();

	void ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge);
	void Update(float fFrameRate);
	void Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug);
};