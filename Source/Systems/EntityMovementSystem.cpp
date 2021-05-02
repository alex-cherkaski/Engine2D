#include "PCH.h"
#include "EntityMovementSystem.h"
#include "Components\Components.h"
#include "ECS\Registry.h"

EntityMovementSystem::EntityMovementSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<VelocityComponent>();
}

void EntityMovementSystem::Initialize()
{
}

void EntityMovementSystem::Update(float a_deltaTime)
{
	// Move all entities with a velocity component by their desired velocity for this frame.
	for (const Entity entity : m_entities)
	{
		TransformComponent& transformComponent = m_registry.GetComponentWrite<TransformComponent>(entity);
		const VelocityComponent& velocityComponent = m_registry.GetComponentRead<VelocityComponent>(entity);

		transformComponent.x += velocityComponent.x * a_deltaTime;
		transformComponent.y += velocityComponent.y * a_deltaTime;
	}
}

void EntityMovementSystem::Render()
{
}

