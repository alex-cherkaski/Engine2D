#include "PCH.h"
#include "TileManager.h"
#include "TextureManager\TextureManager.h"
#include "ECS\Registry.h"
#include "Components\Components.h"
#include "Engine.h"

void TileManager::CreateMap(const char* a_tilemapPath, const char* a_spriteSheetPath, int a_rowCount, int a_columnCount, int a_tileWidth, int a_tileHeight)
{
	Registry& registry = Engine::GetInstanceWrite().GetRegistryWrite();
	m_mapWidth = static_cast<int>(round(a_columnCount * a_tileWidth * m_tileScale));
	m_mapHeight = static_cast<int>(round(a_rowCount * a_tileHeight * m_tileScale));

	// Load the sprite sheet for the map.
	TextureManager& textureManager = TextureManager::GetInstanceWrite();
	const size_t textureId = textureManager.LoadTexture(a_spriteSheetPath);

#pragma warning(disable : 4996) // fopen unsafe warning.
	// Attempt to open the tile map file.
	FILE* file = fopen(a_tilemapPath, "r");
	assert(file != nullptr);
#pragma warning(default : 4996)

	// The number of entries in single column, in the form "row tile" "column tile" "comma character".
	constexpr int ENTRIES_PER_COLUMN = 3;

	// Read in the tile map one line at a time.
	constexpr int BUFFER_SIZE = 256;
	char buffer[BUFFER_SIZE] = { '\0' };
	for (int rowIndex = 0; fgets(buffer, sizeof(char) * BUFFER_SIZE, file) && rowIndex < a_rowCount; ++rowIndex)
	{
		// For every column in the tile map...
		for (int columnIndex = 0; columnIndex < a_columnCount * ENTRIES_PER_COLUMN; columnIndex += ENTRIES_PER_COLUMN)
		{
			// Convert the row and column to integers.
			const int row = buffer[columnIndex] - '0';
			const int column = buffer[columnIndex + 1] - '0';

			// Create the tile entity.
			const Entity tile = registry.CreateEntity();
			{
				// Add the transform component.
				registry.AddComponent<TransformComponent>(tile, {
					static_cast<float>((columnIndex / ENTRIES_PER_COLUMN) * a_tileWidth * m_tileScale),
					static_cast<float>(rowIndex * a_tileHeight * m_tileScale),
					0.0f,
					m_tileScale,
					m_tileScale
				},
				RequestPriority::Deferred);

				// Add the texture component.
				registry.AddComponent<TextureComponent>(tile, {
					textureId,
					RenderOrder::BaseTileOrder
				},
				RequestPriority::Deferred);

				// Add the tile component.
				registry.AddComponent<TileComponent>(tile, {
					column * a_tileWidth,
					row * a_tileHeight,
					a_tileWidth,
					a_tileHeight
				},
				RequestPriority::Deferred);
			}
		}
	}
}

