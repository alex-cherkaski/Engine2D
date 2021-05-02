#include "PCH.h"
#include "BoundsCheckingSystem.h"
#include "Components\Components.h"
#include "ECS\Registry.h"
#include "Engine.h"
#include "Tags\Tags.h"
#include "TextureManager\TextureManager.h"

BoundsCheckingSystem::BoundsCheckingSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<VelocityComponent>();
	RequireComponent<TextureComponent>();
}

void BoundsCheckingSystem::Initialize()
{
	// Retrieve the player entity and relevant components required for the bounds checking.
	assert(m_registry.GetEntitiesWithTag<PlayerTag>().size() == 1);
	m_playerEntity = m_registry.GetEntitiesWithTag<PlayerTag>()[0];

	// Get map dimensions to check player position against.
	const Engine& engine = Engine::GetInstanceRead();
	m_mapWidth = engine.GetMapWidth();
	m_mapHeight = engine.GetMapHeight();

	// Retrieve relevant components for player texture dimensions calculations.
	TransformComponent& playerTransform = m_registry.GetComponentWrite<TransformComponent>(m_playerEntity);
	const AnimationComponent& playerAnimation = m_registry.GetComponentRead<AnimationComponent>(m_playerEntity);

	// Cache player texture width and height.
	m_playerTextureWidth = playerAnimation.spriteWidth * playerTransform.xScale;
	m_playerTextureHeight = playerAnimation.spriteHeight * playerTransform.yScale;
}

void BoundsCheckingSystem::Update(float a_deltaTime)
{
	// Check and adjust the player's position if necessary.
	CheckPlayerBounds();
	
	// Check all other moving entities with texture and transform component.
	for (const Entity entity : m_entities)
	{
		// // Retrieve the relevant entity components required to do the bounds checking.
		const TransformComponent& transformComponent = m_registry.GetComponentRead<TransformComponent>(entity);
		const TextureComponent& textureComponent = m_registry.GetComponentRead<TextureComponent>(entity);

		// Get the texture width and height to for bounds checking calculations.
		int textureWidth = 0;
		int textureHeight = 0;

		// Use sprite dimensions for texture width and height if an animation component is present on the entity.
		if (m_registry.HaveComponent<AnimationComponent>(entity))
		{
			const AnimationComponent& animationComponent = m_registry.GetComponentRead<AnimationComponent>(entity);
			textureWidth = static_cast<int>(round(animationComponent.spriteWidth * transformComponent.xScale));
			textureHeight = static_cast<int>(round(animationComponent.spriteHeight * transformComponent.yScale));
		}

		// Else use the regular texture.
		else
		{
			textureWidth = m_textureManager.GetTextureWidth(textureComponent.textureId);
			textureHeight = m_textureManager.GetTextureHeight(textureComponent.textureId);
		}

		// Check if the projectile is still within the map bounds along the x-axis.
		const bool xOutOfBounds = (transformComponent.x + textureWidth * transformComponent.xScale <= 0.0f)
			|| (transformComponent.x > m_mapWidth);

		// Check if the projectile is still within the map bounds along the y-axis.
		const bool yOutOfBounds = (transformComponent.y + textureHeight * transformComponent.yScale <= 0.0f)
			|| (transformComponent.y > m_mapHeight);

		static int value = 0;
		if (entity == value)
		{
			int breakpoint = 0;
		}

		if (xOutOfBounds || yOutOfBounds)
			m_registry.RemoveEntity(entity);
	}
}

void BoundsCheckingSystem::Render()
{
}

void BoundsCheckingSystem::CheckPlayerBounds()
{
	// Only check player bounds if we have a living player.
	if (m_registry.GetEntitiesWithTag<PlayerTag>().empty())
		return;

	// Get player transform to update player position.
	TransformComponent& playerTransform = m_registry.GetComponentWrite<TransformComponent>(m_playerEntity);

	// Calculate player dimensions offset by sprite width, height, and scale.
	const int xPlayerEnd = static_cast<int>(round(playerTransform.x + m_playerTextureWidth));
	const int yPlayerEnd = static_cast<int>(round(playerTransform.y + m_playerTextureHeight));

	// Check if the player is within the x-axis bounds of the map, and adjust the player's transform if necessary.
	if (playerTransform.x < 0.0f)
		playerTransform.x += -playerTransform.x;
	else if (xPlayerEnd > m_mapWidth)
		playerTransform.x -= static_cast<float>(xPlayerEnd - m_mapWidth);

	// Check if the player is within the y-axis bounds of the map, and adjust the player's transform if necessary.
	if (playerTransform.y < 0.0f)
		playerTransform.y += -playerTransform.y;
	else if (yPlayerEnd > m_mapHeight)
		playerTransform.y -= static_cast<float>(yPlayerEnd - m_mapHeight);
}

