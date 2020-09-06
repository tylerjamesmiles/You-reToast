#pragma once
#include "../Utility/olcPixelGameEngine.h"
#include "../Utility/Timer.h"
#include <fstream>

struct Rect;

struct Map
{
	olc::Renderable image;
	olc::vi2d vMapSize;
	olc::vi2d vTileSize;
	int nNumLayers;
	std::vector<std::vector<int>> vecTiles;
	std::vector<std::vector<int>> vecAnimations;
	std::vector<std::pair<int, std::string>> vecTags;
	
	Timer MapTimer;

	int NumTiles() const;
	olc::vf2d ScreenSize() const;
	std::vector<int> GetTaggedTiles(std::string tag) const;

	Map(std::string map_file_name, std::string image_file_name);

	std::vector<Rect*> GetConstraints() const;
	
	void UpdateFrame(float fDelta);
	void RenderLayer(int layer, olc::vf2d camera_pos, olc::PixelGameEngine* pge) const;
};