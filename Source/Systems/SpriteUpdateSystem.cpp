#include "PCH.h"
#include "SpriteUpdateSystem.h"
#include "ECS\Registry.h"
#include "Components\Components.h"
#include "Tags\Tags.h"
#include "Engine.h"
#include "TextureManager\TextureManager.h"

SpriteUpdateSystem::SpriteUpdateSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<AnimationComponent>();
	RequireComponent<TextureComponent>();
}

void SpriteUpdateSystem::Initialize()
{

}

void SpriteUpdateSystem::Update(float a_deltaTime)
{
	for (const Entity entity : m_entities)
	{
		// Retrieve any required components to update the current entity belonging to this system.
		AnimationComponent& spriteComponent = m_registry.GetComponentWrite<AnimationComponent>(entity);

		// Calculate the sprite column index.
		const size_t currentTimeInMs = spriteComponent.lastUpdateTime + spriteComponent.updateTime;
		if (SDL_GetTicks() > currentTimeInMs)
		{
			// Toggle to the next column on the sprite sheet in a circular manner.
			spriteComponent.currentColumn = (spriteComponent.currentColumn + 1) % spriteComponent.columnCount;

			// Update the time required to pass before the next toggle.
			spriteComponent.lastUpdateTime = SDL_GetTicks();
		}
	}
}

void SpriteUpdateSystem::Render()
{
}

