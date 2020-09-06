#pragma once
#include "olcPixelGameEngine.h"

inline olc::vf2d RandPos(olc::PixelGameEngine* pge) { 
	return olc::vf2d{ (float)(rand() % pge->ScreenWidth()), (float)(rand() % pge->ScreenHeight()) }; 
};

inline olc::vi2d IndexToTilePos(int index, int vec_width)
{
	return olc::vi2d{ index % vec_width, index / vec_width };
}

inline olc::vi2d IndexToScreenPos(int index, int vec_width, const olc::vi2d& tile_size)
{
	return IndexToTilePos(index, vec_width) * tile_size;
}

inline int TilePosToIndex(const olc::vi2d& tile_pos, int vec_width)
{
	return tile_pos.y * vec_width + tile_pos.x;
}

inline int ScreenPosToIndex(const olc::vi2d screen_pos, int vec_width, const olc::vi2d& tile_size)
{
	const olc::vi2d tile_pos = screen_pos / tile_size;
	return TilePosToIndex(tile_pos, vec_width);
}

inline olc::vf2d ClampedVec(const olc::vf2d vec, const olc::vf2d min, const olc::vf2d max) {
	olc::vf2d result = vec;
	if (result.x < min.x) result.x = min.x;
	if (result.y < min.y) result.y = min.y;
	if (result.x > max.x) result.x = max.x;
	if (result.y > max.y) result.y = max.y;
	return result;
}

inline bool IsInVec(int value, const std::vector<int>& vec) {
	for (int i : vec)
		if (i == value)
			return true;
	return false;
}