#include "PCH.h"
#include "CollisionSystem.h"
#include "Components\Components.h"
#include "Tags\Tags.h"
#include "EventManager\EventManager.h"
#include "Events\Events.h"

CollisionSystem::CollisionSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<CollisionComponent>();
}

void CollisionSystem::Initialize()
{
	EventManager& eventManager = EventManager::GetInstanceWrite();
	eventManager.SubscribeToEvent<CollisionEvent, CollisionSystem>(this, &CollisionSystem::OnCollision);
}

void CollisionSystem::Update(float a_deltaTime)
{
	// Cache the event manager 
	static EventManager& eventManager = EventManager::GetInstanceWrite();

	// Check all currently active entities for collisions against each other.
	for (auto entityIndex1 = m_entities.cbegin(); entityIndex1 != m_entities.cend(); ++entityIndex1)
	{
		for (auto entityIndex2 = std::next(entityIndex1); entityIndex2 != m_entities.cend(); ++entityIndex2)
		{
			// If a collision occurred, emit a collision event.
			if (CollideAABB(*entityIndex1, *entityIndex2))
			{
				eventManager.EmitEvent<CollisionEvent>({ *entityIndex1, *entityIndex2 }, EventPriority::Deferred);
			}
		}
	}
}

void CollisionSystem::Render()
{
}

void CollisionSystem::OnCollision(const CollisionEvent& a_collisionEvent)
{
	const Entity entity1 = a_collisionEvent.entity1;
	const Entity entity2 = a_collisionEvent.entity2;

	// Handle Player and NPC collisions.
	if (HaveTags<PlayerTag, NPCTag>(entity1, entity2))
	{
		m_registry.RemoveEntity(entity1);
		m_registry.RemoveEntity(entity2);
	}

	// Handle Player and NPC collisions.
	else if (HaveTags<ProjectileTag, NPCTag>(entity1, entity2))
	{
		// Figure out which entity is which.
		const Entity projectileEntity = m_registry.HaveTag<ProjectileTag>(entity1) ? entity1 : entity2;
		const Entity npcEntity = m_registry.HaveTag<NPCTag>(entity1) ? entity1 : entity2;

		// Get the health and projectile components, and reduce the health by the hit amount.
		const ProjectileComponent& projectileComponent = m_registry.GetComponentRead<ProjectileComponent>(projectileEntity);
		HealthComponent& healthComponent = m_registry.GetComponentWrite<HealthComponent>(npcEntity);
		healthComponent.currentHealthPoints -= projectileComponent.damage;

		// Destroy the projectile.
		m_registry.RemoveEntity(projectileEntity);

		// If the health of the hit entity is less than or equal zero, destroy the entity as well.
		if (healthComponent.currentHealthPoints <= 0)
			m_registry.RemoveEntity(npcEntity);
	}
}

bool CollisionSystem::CollideAABB(const Entity entity1, const Entity entity2) const
{
	const TransformComponent& transformComponent1 = m_registry.GetComponentRead<TransformComponent>(entity1);
	const CollisionComponent& collisionComponent1 = m_registry.GetComponentRead<CollisionComponent>(entity1);

	const TransformComponent& transformComponent2 = m_registry.GetComponentRead<TransformComponent>(entity2);
	const CollisionComponent& collisionComponent2 = m_registry.GetComponentRead<CollisionComponent>(entity2);

	const SDL_Rect entity1Box = {
		static_cast<int>(round(transformComponent1.x)),
		static_cast<int>(round(transformComponent1.y)),
		static_cast<int>(round(collisionComponent1.colliderWidth * transformComponent1.xScale)),
		static_cast<int>(round(collisionComponent1.colliderWidth * transformComponent1.yScale))
	};

	const SDL_Rect entity2Box = {
		static_cast<int>(round(transformComponent2.x)),
		static_cast<int>(round(transformComponent2.y)),
		static_cast<int>(round(collisionComponent2.colliderWidth * transformComponent2.xScale)),
		static_cast<int>(round(collisionComponent2.colliderWidth * transformComponent2.yScale))
	};

	return SDL_HasIntersection(&entity1Box, &entity2Box) == SDL_TRUE;
}

