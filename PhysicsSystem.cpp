#include "PhysicsSystem.h"
#include "Entities/Player.h"
#include "Entities/Loaf.h"
#include <algorithm>
#include <assert.h>

PhysicsSystem::~PhysicsSystem()
{
	Clear();
}

void PhysicsSystem::AddEntity(Entity* e)
{
	vecEntities.push_back(e);
}

void PhysicsSystem::AddConstraint(Rect* r)
{
	vecConstraints.push_back(r);
}

void PhysicsSystem::RemoveEntity(Entity* e)
{
	for (size_t i = 0; i < vecEntities.size(); i++)
		if (vecEntities[i] == e) {
			delete e;
			vecEntities.erase(vecEntities.begin() + i);
			break;
		}
}

void PhysicsSystem::Clear()
{
	for (Entity* e : vecEntities)
		delete e;

	for (Rect* r : vecConstraints)
		delete r;

	if (pCurrentMap)
		delete pCurrentMap;
}

olc::vf2d RandomOffscreenPos(const std::vector<Entity*>& vecEntities, const std::vector<Rect*>& vecConstraints) {
	olc::vf2d vPos{};
	while (true) {
		auto RandFloat = []() { return (float)(rand() % 1600); };
		vPos = olc::vf2d{ RandFloat(), RandFloat() };
		bool bLegal = true;

		for (Rect* r : vecConstraints)
			if (ShapesOverlap(Rect(vPos, { 16, 16 }), *r))
				bLegal = false;

		if (bLegal) break;
	}
	return vPos;
}

void PhysicsSystem::SetCurrentMap(Map* m)
{
	for (Rect* r : vecConstraints)
		delete r;
	vecConstraints.clear();
	for (Entity* e : vecEntities)
		delete e;
	vecEntities.clear();

	for (Rect* r : m->GetConstraints())
		vecConstraints.push_back(r);

	TutorialTimer = Timer(1.0f);
	TutorialPhase = 0;
	WaveTimer.fTotalElapsedTime = 0.0f;
	WaveTimer.nFrame = 0;
	nNumWaves = 0;
	bGameOver = false;

	pCurrentMap = m;

	for (int index_pos : m->GetTaggedTiles("PlayerStart")) {
		pPlayer = new Player(IndexToScreenPos(index_pos, m->vMapSize.x, m->vTileSize), new Rect({ 8, 8 }));
		AddEntity(pPlayer);
	}

	// Spawn Tutorial Toaster. Spawn Tutorial Loaf. (r/BrandNewSentence)
	AddEntity(new Appliance(olc::vf2d{ 20, 10 } *m->vTileSize, new Rect({ 12, 12 })));
	AddEntity(new Loaf(olc::vf2d{ 20, 20 } * m->vTileSize, new Rect({ 32, 16 })));

	SpawnWave(10, 3);
}

void PhysicsSystem::SpawnWave(int num_entities, int num_loafs)
{
	for (int i = 0; i < num_entities; i++) {
		olc::vf2d vRandomPos = RandomOffscreenPos(vecEntities, vecConstraints);
		AddEntity(new Appliance(vRandomPos, new Rect({ 10, 10 })));
	}

	for (int i = 0; i < num_loafs; i++) {
		olc::vf2d vRandomPos = RandomOffscreenPos(vecEntities, vecConstraints);
		AddEntity(new Loaf(vRandomPos, new Rect({ 32, 16 })));
	}
}

void CollisionWith(Bread* b, Appliance* a) {
	b->bKillMe = true;
	a->bAsleep = true;
	a->SleepTimer.fTotalElapsedTime = 0.0f;
	a->SleepTimer.nFrame = 0;
}

void CollisionWith(Player* p, Loaf* l) {
	p->nAmmo += 5;
	p->nAmtAmmoGained = 5;
	p->AmmoTimer.nFrame = 0;
	l->bKillMe = true;
}

void CollisionWith(Player* p, Bread* b) {
	p->nAmmo += 1;
	p->nAmtAmmoGained = 1;
	p->AmmoTimer.nFrame = 0;
	b->bKillMe = true;
}

void CollisionWith(Player* p, Appliance* a) {
	if (a->bAsleep) {
		p->nSoulsHarvested += 1;
		p->nHP += 5 - a->SleepTimer.nFrame;
		p->nAmtToHeal = 5 - a->SleepTimer.nFrame;
		p->HealTimer.nFrame = 0;
		a->bKillMe = true;
	}
	else {
		p->nHP -= 30;
		p->BloodShowTimer.nFrame = 0;
		a->vVel *= -10.0f;
	}
}

void CollisionWith(Entity* e1, Entity* e2) {
	switch (e1->type) {
	case Entity::PLAYER:
		if (e2->type == Entity::LOAF) CollisionWith(static_cast<Player*>(e1), static_cast<Loaf*>(e2)); break;
		if (e2->type == Entity::APPLIANCE) CollisionWith(static_cast<Player*>(e1), static_cast<Appliance*>(e2)); break;

	case Entity::BREAD:
		if (e2->type == Entity::APPLIANCE) CollisionWith(static_cast<Bread*>(e1), static_cast<Appliance*>(e2)); break;

	case Entity::APPLIANCE:
		if (e2->type == Entity::PLAYER) CollisionWith(static_cast<Player*>(e2), static_cast<Appliance*>(e1)); break;
		if (e2->type == Entity::BREAD) CollisionWith(static_cast<Bread*>(e2), static_cast<Appliance*>(e1)); break;

	case Entity::LOAF:
		if (e2->type == Entity::PLAYER) CollisionWith(static_cast<Player*>(e2), static_cast<Loaf*>(e1)); break;
	}
}

void PhysicsSystem::Update(olc::PixelGameEngine* pge, float fDelta)
{
	// Fixed framerate
	PhysicsTimer.Update(fDelta);

	pCurrentMap->UpdateFrame(fDelta);

	// Tutorial Updates
	if (TutorialPhase == 0 && pPlayer->nAmmo > 0) {
		TutorialTimer.nFrame = 0;
		TutorialPhase = 1;
	}
	if (TutorialPhase == 1) {
		for (Entity* e : vecEntities) {
			if (e->type == Entity::APPLIANCE && static_cast<Appliance*>(e)->bAsleep) {
				TutorialTimer.nFrame = 0;
				TutorialPhase = 2;
				break;
			}
		}
	}
	if (TutorialPhase == 2) {
		if (pPlayer->nSoulsHarvested > 0)
			TutorialPhase = 3;
	}
	if (TutorialPhase == 3) {
		WaveTimer.Update(fDelta);
	}

	
	// Physics Loop
	for (int i = 0; i < PhysicsTimer.nTics; i++) {

		// Spawn new wave
		if (WaveTimer.nFrame > 20) {
			WaveTimer.nFrame = 0;
			WaveTimer.fTotalElapsedTime = 0.0f;
			nNumWaves += 1;
			SpawnWave(nNumWaves * 3, 5);
		}

		// Clear Collisions/Contacts
		vecCollisions.clear();
		for (Entity* e : vecEntities) {
			e->ClearContacts();
			if (e->type == Entity::PLAYER)
				pPlayer = static_cast<Player*>(e);
		}

		// User Input + AI + Natural Forces
		for (Entity* e : vecEntities) {
			assert(e && "entity didn't die?");
			e->ApplyForces(vecEntities, PhysicsTimer.fFrameRate, vCameraPos, pge);
		}

		// Collect vecCollisions
		auto AddCollision = [](Entity& e, const Rect& rect, std::vector<Collision>& vecCollisions, float fFrameRate) -> void {
			if ((e.vPos - rect.vPos).mag2() < e.fCheckRadius * e.fCheckRadius) {
				Collision c = Raycast(e, rect, fFrameRate);
				if (c.bExists) {
					vecCollisions.push_back(c);
					if (rect.pOwner) // environment constraints have no owner
						e.AddContact(rect.pOwner);
				}
			}
		};
		for (Entity* e1 : vecEntities) {
			for (Rect* r : vecConstraints)
				AddCollision(*e1, *r, vecCollisions, PhysicsTimer.fFrameRate);
			for (Entity* e2 : vecEntities)
				AddCollision(*e1, *e2->pRect, vecCollisions, PhysicsTimer.fFrameRate);
		}

		// fix for player touching bug
		for (Entity* e : vecEntities) {
			if (e->type == Entity::APPLIANCE) {
				Appliance* a = static_cast<Appliance*>(e);
				if (a->bAsleep && (a->vPos - pPlayer->vPos).mag2() < 20.0f * 20.0f) {
					CollisionWith(pPlayer, a);
				}
			}
			else if (e->type == Entity::BREAD) {
				if ((e->vPos - pPlayer->vPos).mag2() < 20.0f * 20.0f)
					CollisionWith(pPlayer, static_cast<Bread*>(e));
			}
		}

		// Sort vecCollisions by time
		std::sort(vecCollisions.begin(), vecCollisions.end(),
			[](const Collision& c1, const Collision& c2) {
				return c1.fTime < c2.fTime;
			});

		// Apply Impulses (change entity vel on collisions)
		for (const Collision& c : vecCollisions) {
			c.pEntity->ApplyImpulse(c.vNormal, c.fTime);
			if (c.pWith)
				CollisionWith(c.pEntity, c.pWith);
		}

		// Update entity kinematics
		for (Entity* e : vecEntities)
			e->Update(PhysicsTimer.fFrameRate);

		// Correct overlaps between entities
		for (Entity* e1 : vecEntities)
			for (Entity* e2 : vecEntities)
				if (ShapesOverlap(*e1->pRect, *e2->pRect)) {
					olc::vf2d fOverlap = AmountOverlap(*e1->pRect, *e2->pRect);
					e1->Move(fOverlap);
					e2->Move(fOverlap * -1.0f);

					// clamp w/in bounds
					e1->vPos = ClampedVec(e1->vPos, { 0, 0 }, pCurrentMap->ScreenSize() - e1->GetRectSize());
				}

		// Set Camera Position
		for (Entity* e : vecEntities)
			if (e->type == Entity::PLAYER)
				vCameraPos = e->vPos;

		olc::vf2d vScreenSize = olc::vf2d{ 256, 240 };
		olc::vf2d vHalfScreenSize = vScreenSize / 2.0f;
		vCameraPos -= vHalfScreenSize;
		vCameraPos = ClampedVec(vCameraPos, { 0, 0 }, pCurrentMap->ScreenSize() - vScreenSize);

		if (pPlayer->nHP <= 0)
			bGameOver = true;

		// Kill them. Kill them all.
		for (Entity* e : vecEntities)
			if (e->bKillMe)
				RemoveEntity(e);
	}
}

void PhysicsSystem::Render(olc::PixelGameEngine* pge)
{
	// Y Sort Entities
	std::sort(vecEntities.begin(), vecEntities.end(),
		[](const Entity* e1, const Entity* e2) {
			return e1->vPos.y < e2->vPos.y;
		});

	// Render Map
	for (int layer = 1; layer <= 4; layer++) {
		pge->SetDrawTarget(layer); 
		pge->Clear(olc::BLANK);
		pCurrentMap->RenderLayer(layer, vCameraPos, pge);
	}

	// Render Entities
	for (Entity* e : vecEntities) {
		pge->SetDrawTarget(3);
		e->Render(vCameraPos, pge, render_debug);
	}

	// Render Constraints
	pge->SetDrawTarget(uint8_t(0));
	pge->Clear(olc::BLANK);
	if (render_debug) 
		for (const Rect* r : vecConstraints)
			r->Render(vCameraPos, pge);
	
	// Enable Layers
	for (int i = 0; i < 5; i++)
		pge->EnableLayer(i, true);

	// Tutorial Text
	if (TutorialPhase == 0) {
		if (TutorialTimer.nFrame < 5) {
			pge->DrawStringDecal(pPlayer->vPos + olc::vf2d{ -10, -30 } - vCameraPos, "WASD");
		}

		for (Entity* e : vecEntities) {
			if (e->type == Entity::APPLIANCE) {
				pge->DrawStringDecal(e->vPos + olc::vf2d{ -10, -30 } - vCameraPos, "AVOID");
			}
			if (e->type == Entity::LOAF) {
				pge->DrawStringDecal(e->vPos + olc::vf2d{ -10, -30 } - vCameraPos, "COLLECT");
			}
		}

	}
	if (TutorialPhase == 1) {
		pge->DrawStringDecal(pPlayer->vPos + olc::vf2d{ -60, -36 } - vCameraPos, "LEFT MOUSE BUTTON");
		for (Entity* e : vecEntities) {
			if (e->type == Entity::APPLIANCE) {
				pge->DrawStringDecal(e->vPos + olc::vf2d{ -10, -30 } - vCameraPos, "SHOOT");
			}
		}
	}
	if (TutorialPhase == 2) {
		for (Entity* e : vecEntities) {
			if (e->type == Entity::APPLIANCE && static_cast<Appliance*>(e)->bAsleep) {
				pge->DrawStringDecal(e->vPos + olc::vf2d{ -20, -30 } - vCameraPos, "COLLECT");
			}
		}
	}
	if (TutorialPhase == 3) {
		// Wave
		pge->DrawStringDecal({ 0, 0 }, "WAVE " + std::to_string(nNumWaves));
		pge->DrawStringDecal({ 88, 0 }, "NEXT WAVE: " + std::to_string(20 - WaveTimer.nFrame));
	}
}
