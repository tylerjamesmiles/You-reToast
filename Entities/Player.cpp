#include "Player.h"

Player::Player(olc::vf2d p, Rect* r) : Entity(PLAYER, p, r)
{
	image.Load("Resources/Art/Toaster.png");
	cursor.Load("Resources/Art/Bread.png");
	hud.Load("Resources/Art/HUD.png");

	vImageOffset = { -18, -10 };

	EntityTimer.nFrame += rand() % 4;
	ShootyTimer = Timer(0.5f);
	BloodShowTimer = Timer(0.1f);
	BloodShowTimer.nFrame = 5;
	HealTimer = Timer(0.1f);
	HealTimer.nFrame = 5;
	AmmoTimer = Timer(0.1f);
	AmmoTimer.nFrame = 5;
}

Player::~Player() 
{ 
	delete pRect; 
}

void Player::ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge) {
	vVel *= 0.0f;
	if (pge->GetKey(olc::W).bHeld) vVel.y -= fSpeed;
	if (pge->GetKey(olc::A).bHeld) vVel.x -= fSpeed;
	if (pge->GetKey(olc::S).bHeld) vVel.y += fSpeed;
	if (pge->GetKey(olc::D).bHeld) vVel.x += fSpeed;

	// Make dat Bread
	if (pge->GetMouse(0).bHeld && ShootyTimer.nFrame >= 1 && nAmmo > 0) {
		ShootyTimer.nFrame = 0;
		nAmmo -= 1;

		Entity* b = new Bread(vPos, new Rect({ 16, 16 }));
		olc::vf2d vMousePos = pge->GetMousePos() + camera_pos;
		b->vAcc = vVel + (vMousePos - pRect->Middle()).norm() * 400.0f;
		b->vPos += b->vAcc * 0.07f;
		vec_entities.push_back(b);
	}
}

void Player::Update(float fFrameRate) {
	ShootyTimer.Update(fFrameRate);
	BloodShowTimer.Update(fFrameRate);
	HealTimer.Update(fFrameRate);
	AmmoTimer.Update(fFrameRate);
	UpdateKinematics(fFrameRate);
}

void Player::Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug) {
	bool bRight = (vVel.x > 0);
	float fSourceDir = (bRight) ? 0.0f : 16.0f;
	vImageOffset = (bRight) ? olc::vf2d{ -18, -10 } : olc::vf2d{ -4, -10 };
	int nHPTint = (int)((float)(255.0f * ((float)nHP / 100.0f)));
	if (nHPTint >= 100) nHPTint = 255;


	if (image.Decal())
		pge->DrawPartialDecal(
			olc::vi2d(vPos + vImageOffset - camera_pos),
			image.Decal(),
			{ (float)(EntityTimer.nFrame % 6) * 32.0f, fSourceDir },
			{ 32, 16 },
			{ 1, 1 },
			olc::Pixel(255, nHPTint, nHPTint)
	);



	if (render_debug) {
		pge->DrawCircle(vPos - camera_pos, 2, olc::DARK_GREEN);
		pRect->Render(camera_pos, pge);
	}

	pge->SetDrawTarget(uint8_t(0));

	// Hit Animation
	if (BloodShowTimer.nFrame < 5) {
		pge->FillRectDecal({ 0, 0 }, { 256, 240 }, olc::Pixel(255, 0, 0, 255 - BloodShowTimer.nFrame * 50));
		pge->DrawStringDecal(olc::vi2d(vPos - camera_pos + olc::vf2d{-16, -20 + (float)BloodShowTimer.nFrame * -1}), "-30 HP");
	}

	if (HealTimer.nFrame < 5) {
		pge->DrawStringDecal(olc::vi2d(vPos - camera_pos + olc::vf2d{ -16, -20 + (float)HealTimer.nFrame * -1 }), "+" + std::to_string(nAmtToHeal) + " HP");
	}

	if (AmmoTimer.nFrame < 5) {
		pge->DrawStringDecal(olc::vi2d(vPos - camera_pos + olc::vf2d{ -16, -20 + (float)AmmoTimer.nFrame * -1 }), "+" + std::to_string(nAmtAmmoGained) + " AMMO");
	}

	// UI
	pge->DrawDecal(olc::vi2d{ 0, 224 }, hud.Decal());
	pge->DrawStringDecal(olc::vi2d{ 10, 230 }, "HP: " + std::to_string(nHP), olc::Pixel(255, nHPTint, nHPTint));
	pge->DrawStringDecal(olc::vi2d{ 96, 230 }, "AMMO: " + std::to_string(nAmmo), olc::WHITE);
	pge->DrawStringDecal(olc::vi2d{ 180, 230 }, "SCORE: " + std::to_string(nSoulsHarvested), olc::WHITE);

	if (nAmmo > 0) {
		olc::vf2d vMousePos = pge->GetMousePos() + camera_pos;
		olc::vf2d vToMouse = (vMousePos - pRect->Middle()).norm() * 50.0f;
		olc::vf2d vCursorPos = (pRect->Middle() - camera_pos) + vToMouse;
		pge->DrawPartialDecal(olc::vi2d(vCursorPos + olc::vf2d{-8, -8}), { 16, 16 }, cursor.Decal(), { 0, 0 }, { 16, 16 }, olc::Pixel(255, 0, 0, 100));
	}
}