#include <fstream>

#include "Utility/olcPixelGameEngine.h"
#include "PhysicsSystem.h"
#include "Utility/Map.h"
#include "Utility/Timer.h"

#include "soloud.h"
#include "soloud_wav.h"


class Game : public olc::PixelGameEngine
{
private:
	PhysicsSystem system;
	olc::ResourcePack* resources;

	olc::Renderable title_screen;
	enum {
		PLAY,
		QUIT
	} title_option;
	bool bTitle = true;

	SoLoud::Soloud gSoloud; // SoLoud engine
	SoLoud::Wav gWav;

public:
	Game() { sAppName = "Game"; }
	bool OnUserCreate() override;
	bool OnUserUpdate(float fDelta) override;
	bool OnUserDestroy() override;
};

bool Game::OnUserCreate()
{
	srand(time(NULL));

	system.SetCurrentMap(new Map("Resources/Maps/MiddlePark.map", "Resources/Art/FallNeighborhoodTiles.png"));
	system.render_debug = false;

	for (int i = 0; i < 5; i++)
		CreateLayer();

	title_screen.Load("Resources/Art/TitlePlay.png");
	title_option = PLAY;

	gSoloud.init();
	gWav.load("Resources/Music/HeartOfTheToaster.wav");
	gWav.setLooping(true);
	gSoloud.play(gWav);

	return true;
}

bool Game::OnUserUpdate(float fDelta)
{

	if (bTitle) {
		if (GetKey(olc::UP).bPressed || GetKey(olc::DOWN).bPressed || GetKey(olc::W).bPressed || GetKey(olc::S).bPressed) {
			if (title_option == PLAY) {
				title_screen.Load("Resources/Art/TitleQuit.png");
				title_option = QUIT;
			}
			else if (title_option == QUIT) {
				title_screen.Load("Resources/Art/TitlePlay.png");
				title_option = PLAY;
			}
		}

		if (GetKey(olc::ENTER).bPressed || GetKey(olc::SPACE).bPressed) {
			if (title_option == PLAY)
				bTitle = false;
			else if (title_option == QUIT)
				return false;
		}

		DrawDecal({ 0, 0 }, title_screen.Decal());
	}
	else {
		if (!system.bGameOver) {
			system.Update(this, fDelta);
			system.Render(this);

			if (GetKey(olc::ESCAPE).bPressed)
				bTitle = true;
		}
		else {

			int nCurrentScore = system.pPlayer->nSoulsHarvested;
			int nHighScore = nCurrentScore;
			std::ifstream stream("highscore.score");
			while (stream >> nHighScore) {
				if (nHighScore < nCurrentScore) {
					nHighScore = nCurrentScore;
				}
			}
			std::ofstream os("highscore.score");
			os << " " << nHighScore;

			DrawString({ 80, 76 }, "YOU'RE TOAST.");
			DrawString({ 96, 88 }, "SCORE: " + std::to_string(system.pPlayer->nSoulsHarvested));
			DrawString({ 84, 100 }, "HIGH SCORE: " + std::to_string(nHighScore));
			DrawString({ 60, 112 }, "TRY AGAIN? (y/n)");

			if (GetKey(olc::Y).bPressed) {
				system.bGameOver = false;
				system.SetCurrentMap(new Map("Resources/Maps/MiddlePark.map", "Resources/Art/FallNeighborhoodTiles.png"));
			}
			else if (GetKey(olc::N).bPressed) {
				bTitle = true;
			}
		}
	}

	return true;
}

bool Game::OnUserDestroy()
{
	gSoloud.deinit();
	return true;
}

int main()
{
	Game g;
	if (g.Construct(256, 240, 4, 4, false, false, true))
		g.Start();
}