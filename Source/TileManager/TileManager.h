#pragma once
#include "Macros.h"

class TileManager final
{
public:
	TileManager() = default;
	~TileManager() = default;

	void CreateMap(const char* a_tilemapPath, const char* a_spriteSheetPath, int a_rowCount, int a_columnCount, int a_tileWidth, int a_tileHeight);

	float GetTileScale() const { return m_tileScale; }
	int GetMapWidth() const { return m_mapWidth; }
	int GetMapHeight() const { return m_mapHeight; }

private:
	float m_tileScale = 2.0f;
	int m_mapWidth = 0;
	int m_mapHeight = 0;
};

