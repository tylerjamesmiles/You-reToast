#include "Appliance.h"
#include "../Utility/olcPixelGameEngine.h"
#include "../Utility/Utility.h"

Appliance::Appliance(olc::vf2d p, Rect* r) : Entity(APPLIANCE, p, r) {
	image.Load("Resources/Art/Toaster.png");
	vImageOffset = { -18, -10 };
	EntityTimer.nFrame += rand() % 4;
	SleepTimer = Timer(1.0);
}

void Appliance::ApplyForces(std::vector<Entity*>& vec_entities, float fDelta, olc::vf2d camera_pos, olc::PixelGameEngine* pge) {

	if (bAsleep) {
		vVel *= 0.0f;

		SleepTimer.Update(fDelta);
		if (SleepTimer.nFrame > 5)
			bAsleep = false;
	}
	else {
		olc::vf2d vAvgPos;
		olc::vf2d vPlayerPos;
		int nNumAppliances = 0;
		for (Entity* e : vec_entities) {
			if (e->type == Entity::APPLIANCE && !static_cast<Appliance*>(e)->bAsleep) {
				vAvgPos += e->vPos;
				nNumAppliances++;
			}
			if (e->type == Entity::PLAYER)
				vPlayerPos = e->vPos;
		}
		vAvgPos /= nNumAppliances;

		olc::vf2d vToAvg = vAvgPos - vPos;
		olc::vf2d vToPlayer = vPlayerPos - vPos;
		float fToAvgWeight = (vToPlayer.mag() < 100.0f) ? 0.5f : 1.0f;
			
		// exponential falloff
		vAcc += vToAvg.norm() * fSpeed * fToAvgWeight;
		vAcc += vToPlayer.norm() * fSpeed * 3.0f;
		if (vAcc.mag2() > 50.0f * 50.0f)
			vAcc = vAcc.norm() * 50.0f;
		
		//vAcc *= 0.90f;
		if (vVel.mag() > fMaxSpeed)
			vVel = vVel.norm() * fMaxSpeed;
	}

	vVel += vAcc * fDelta;

	if (vVel.mag() < 0.01f)
		vVel *= 0.0f;
}

void Appliance::Render(olc::vf2d camera_pos, olc::PixelGameEngine* pge, bool render_debug) {
	bool bRight = (vVel.x > 0);
	float fSourceDir = (bRight) ? 0.0f : 16.0f;
	vImageOffset = (bRight) ? olc::vf2d{ -18, -10 } : olc::vf2d{ -4, -10 };
	olc::Pixel tint = (bAsleep) ? olc::Pixel(255, 0, 0) : olc::Pixel(255, 255, 255);

	if (image.Decal())
		pge->DrawPartialDecal(
			olc::vi2d(vPos + vImageOffset - olc::vi2d(camera_pos)),
			image.Decal(),
			{ (float)(EntityTimer.nFrame % 6) * 32.0f, fSourceDir },
			{ 32, 16 },
			{ 1, 1 },
			tint
		);

	if (bAsleep)
		pge->DrawStringDecal(vPos - camera_pos + olc::vi2d{ 0, -20 }, std::to_string(5 - SleepTimer.nFrame));


	if (render_debug) {
		pge->DrawCircle(olc::vi2d(vPos - camera_pos), 2, olc::DARK_GREEN);
		pRect->Render(camera_pos, pge);
		pge->DrawCircle(olc::vi2d(pRect->Middle() - camera_pos), fCheckRadius, olc::GREEN);

		for (Entity* e : pRect->vecContacts)
			pge->DrawLine(olc::vi2d(pRect->Middle() - camera_pos), olc::vi2d(e->pRect->Middle() - camera_pos), olc::RED);
	}
}