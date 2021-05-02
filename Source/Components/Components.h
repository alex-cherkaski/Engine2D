#pragma once
#include "Enums\Enums.h"
#include "TextureManager\TextureManager.h"

struct alignas(32) TransformComponent final
{
	// Position.
	float x = 0.0f;				// 4 bytes.
	float y = 0.0f;				// 4 bytes.

	// Orientation in degrees.
	float rotation = 0.0f;		// 4 bytes.

	// Scale.
	float xScale = 0.0f;		// 4 bytes.
	float yScale = 0.0f;		// 4 bytes.
								// Total = 20 bytes.
};

struct alignas(16) TextureComponent final
{
	// Texture Id required to fetch the texture.
	TextureId textureId = 0;									// 8 bytes.

	// The render priority of this texture. Lower integers get rendered first.
	RenderOrder renderOrder = RenderOrder::UnsetRenderOrder;	// 1 byte.
																// Total = 9 bytes.
};

struct alignas(64) AnimationComponent final
{
	// Sprite change speed.
	size_t updateTime = 0; // In milliseconds.			// 8 bytes.
	size_t lastUpdateTime = 0; // In milliseconds.		// 8 bytes.

	// Single sprite dimensions.
	int spriteWidth = 0;								// 4 bytes.
	int spriteHeight = 0;								// 4 bytes.

	// Current row and column of the sprite sheet.
	int rowCount = 0;									// 4 bytes.
	int columnCount = 0;								// 4 bytes.

	// Current row and column of the sprite sheet.
	int currentRow = 0;									// 4 bytes.
	int currentColumn = 0;								// 4 bytes.
														// Total = 40 bytes.
};

struct alignas(16) WeaponComponent final
{
	unsigned int emissionCadence = 0; // Measured in milliseconds.				// 4 bytes.
	unsigned int lastEmissionTime = 0; // Measured in milliseconds.				// 4 bytes.

	int damage = 0;																// 4 bytes.

	// Who emitted this projectile.
	ProjectileOwner projectileOwner = ProjectileOwner::UnsetProjectileOwner;	// 1 byte.

	// Should a projectile be emitted every emissionCadence milliseconds.
	bool shouldAutoEmit = false;												// 1 byte.
																				// Total = 14 bytes.
};

struct alignas(8) ProjectileComponent final
{
	int damage = 0;																// 4 bytes.

	// Who emitted this projectile.
	ProjectileOwner projectileOwner = ProjectileOwner::UnsetProjectileOwner;	// 1 byte.
																				// Total = 5 bytes.
};

struct VelocityComponent final
{
	// Speed and direction.
	float x = 0.0f;					// 4 bytes.
	float y = 0.0f;					// 4 bytes.
									// Total = 8 bytes.
};

struct TileComponent final
{
	// X and Y positions to start reading the tile.
	int tileX = 0;					// 4 bytes.
	int tileY = 0;					// 4 bytes.

	// Texture dimensions.
	int tileWidth = 0;				// 4 bytes.
	int tileHeight = 0;				// 4 bytes.
									// Total = 16 bytes.
};

struct PlayerControllerComponent final
{
	float m_playerSpeed = 0.0f;		// 4 bytes.
									// Total = 4 bytes.
};

struct CameraComponent final
{
	// Width and height of the viewport.
	int cameraWidth = 0;			// 4 bytes.
	int cameraHeight = 0;			// 4 bytes.
									// Total 8 bytes.
};

struct CollisionComponent final
{
	// Box dimensions for AABB collision detection.
	float colliderWidth = 0;		// 4 bytes.
	float colliderHeight = 0;		// 4 bytes.
};									// Total 8 bytes.

struct HealthComponent final
{
	int totalHealthPoints = 0;		// 4 bytes.
	int currentHealthPoints = 0;	// 4 bytes.
									// Total = 8 bytes.
};

