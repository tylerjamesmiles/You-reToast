#pragma once
#include "Math/CollisionRect.h"
#include "Entities/BaseEntity.h"
#include "Utility/Map.h"
#include "Utility/Timer.h"
#include "Entities/Player.h"

struct PhysicsSystem
{
	std::vector<Entity*> vecEntities;
	std::vector<Rect*> vecConstraints;
	std::vector<Collision> vecCollisions;
	Map* pCurrentMap;

	olc::vf2d vCameraPos;

	Timer PhysicsTimer;
	Timer TutorialTimer;
	int TutorialPhase = 0;
	Timer WaveTimer;

	int nNumWaves = 1;

	Player* pPlayer;

	bool render_debug = true;
	bool bGameOver = false;

	PhysicsSystem() : PhysicsTimer(Timer(16.67f / 1000.0f)) {}
	~PhysicsSystem();

	void AddEntity(Entity* e);
	void AddConstraint(Rect* r);
	void RemoveEntity(Entity* e);
	void Clear();
	void SetCurrentMap(Map* m);
	void SpawnWave(int num_entities, int num_loafs);

	void Update(olc::PixelGameEngine* pge, float fDelta);
	void Render(olc::PixelGameEngine* pge);
};