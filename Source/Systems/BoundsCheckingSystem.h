#pragma once
#include "ECS\System.h"
#include "Constants\Constants.h"
#include "TextureManager\TextureManager.h"

class BoundsCheckingSystem final : public ISystem
{
public:
	BoundsCheckingSystem(Registry& a_registry);
	~BoundsCheckingSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;

private:
	void CheckPlayerBounds();
	void CheckProjectileBounds();
	void CheckNPCBounds();

private:
	const TextureManager& m_textureManager = TextureManager::GetInstanceRead();
	Entity m_playerEntity = INVALID_ENTITY;
	
	int m_mapWidth = 0;
	int m_mapHeight = 0;
	
	float m_playerTextureWidth = 0.0f;
	float m_playerTextureHeight = 0.0f;
};

