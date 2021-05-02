#pragma once

// Tells us who emitted this projectile.
enum class ProjectileOwner : unsigned char
{
	PlayerOwner = 0,
	NPCOwner,
	UnsetProjectileOwner
};

// Render priority for sprites and textures. Lower values get rendered first.
enum class RenderOrder : unsigned char
{
	BaseTileOrder = 0,
	TopTileOrder,
	NPCOrder,
	PlayerOrder,
	ProjectileOrder,
	UnsetRenderOrder
};

