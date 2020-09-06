#include "Map.h"
#include "olcPixelGameEngine.h"
#include "Utility.h"
#include "../Math/CollisionRect.h"

int Map::NumTiles() const
{
	return vMapSize.x * vMapSize.y;
}

olc::vf2d Map::ScreenSize() const
{
	return vMapSize * vTileSize;
}

std::vector<int> Map::GetTaggedTiles(std::string name) const
{
	std::vector<int> result;
	for (auto tag : vecTags)
		if (tag.second == name)
			result.push_back(tag.first);
	return result;
}

Map::Map(std::string map_file_name, std::string image_file_name) 
	: vTileSize(16, 16), MapTimer(Timer(200.0f / 1000.0f))
{
	std::ifstream is(map_file_name);
	if (is.is_open()) {
		std::string name;
		char lb, c, rb;

		is >> name;
		if (name == "MapSize") {
			is >> lb >> vMapSize.x >> c >> vMapSize.y >> rb;
			vecTiles.clear();
		}
		else
			std::cout << "error reading MapSize." << std::endl;

		is >> name;
		if (name == "Tiles") {
			is >> nNumLayers;
			vecTiles.resize(nNumLayers + 1);
			for (int layer = 0; layer < nNumLayers + 1; layer++) {
				is >> lb;
				for (int tile = 0; tile < NumTiles(); tile++) {
					int t;
					is >> t >> c;
					vecTiles[layer].push_back(t);
				}	// the last c is the last rb
			}
		}
		else
			std::cout << "Error reading Tiles." << std::endl;

		is >> name;
		if (name == "Animations") {
			int nNumAnimations;
			is >> nNumAnimations;
			vecAnimations.clear();
			vecAnimations.resize(nNumAnimations);
			for (int i = 0; i < nNumAnimations; i++) {
				is >> lb;
				while (true) {
					int cell;
					is >> cell;
					vecAnimations[i].push_back(cell);
					is >> c;
					if (c == '}')
						break;
				}
			}
		}

		is >> name;
		if (name == "Tags") {
			int nNumTags;
			is >> nNumTags;
			vecTags.clear();
			vecTags.resize(nNumTags);
			for (int i = 0; i < nNumTags; i++) {
				is >> lb >> vecTags[i].first >> c >> vecTags[i].second >> rb;
			}
		}
	}

	image.Load(image_file_name);
}

std::vector<Rect*> Map::GetConstraints() const
{
	std::vector<Rect*> result;
	for (int nTile = 0; nTile < NumTiles(); nTile++)
		if (vecTiles[0][nTile] == 1)
			result.push_back(new Rect(IndexToScreenPos(nTile, vMapSize.x, vTileSize), vTileSize));
	return result;
}

void Map::UpdateFrame(float fDelta)
{
	MapTimer.Update(fDelta);
}

void Map::RenderLayer(int layer, olc::vf2d camera_pos, olc::PixelGameEngine* pge) const
{
	auto IsOnScreen = [&](int tile) -> bool
	{
		// translated back to origin
		olc::vf2d vScreenPos = IndexToScreenPos(tile, vMapSize.x, vTileSize) - camera_pos;
		return (vScreenPos.x < 256 && vScreenPos.y < 240);
	};

	for (int nTile = 0; nTile < NumTiles(); nTile++) {
		if (IsOnScreen(nTile)) {
			int nSource = vecTiles[layer][nTile];

			for (const std::vector<int>& vecAnimation : vecAnimations)
				if (IsInVec(nSource, vecAnimation))
					nSource = vecAnimation[MapTimer.nFrame % vecAnimation.size()];

			pge->DrawPartialDecal(
				olc::vi2d(IndexToScreenPos(nTile, vMapSize.x, vTileSize) - camera_pos),
				image.Decal(),
				IndexToScreenPos(nSource, image.Sprite()->width / vTileSize.x, vTileSize),
				vTileSize
			);
		}
	}
}
