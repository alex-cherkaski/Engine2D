#include "PCH.h"
#include "Registry.h"

void Registry::RunSystemsInitialize()
{
	// Process all initial entity, component, and tag creation requests.
	ProcessPendingComponents();
	ProcessPendingEntities();
	ProcessPendingTags();

	// Run the initialize routine of all existing systems.
	for (const std::unique_ptr<ISystem>& system : m_systems)
	{
		system->Initialize();
	}
}

void Registry::RunSystemsUpdate(float a_delatTime)
{
	// Run the update routine of a system, then process all pending events and requests it may have emitted.
	for (const std::unique_ptr<ISystem>& system : m_systems)
	{
		m_isInSystemUpdate = true;
		system->Update(a_delatTime);
		m_isInSystemUpdate = false;

		m_eventManager.Update();
		ProcessPendingComponents();
		ProcessPendingEntities();
		ProcessPendingTags();
	}
}

void Registry::RunSystemsRender()
{
	// Run the render routine of a system, then process all pending events and requests it may have emitted.
	for (const std::unique_ptr<ISystem>& system : m_systems)
	{
		m_isInSystemRender = true;
		system->Render();
		m_isInSystemRender = false;

		m_eventManager.Update();
		ProcessPendingComponents();
		ProcessPendingEntities();
		ProcessPendingTags();
	}
}

void Registry::ProcessPendingEntities()
{
	ProcessEntityRemovals();
	ProcessEntityAdditions();
}

void Registry::ProcessPendingComponents()
{
	ProcessComponentAdditions();
	ProcessComponnetRemovals();
}

void Registry::ProcessPendingTags()
{
	ProcessTagAdditions();
	ProcessTagRemovals();
}

void Registry::Shutdown()
{
	// Reset the next available entity Id.
	m_nextEntity = 0;

	// Clear our queue of recyclable entity Ids.
	while (!m_deletedEntities.empty())
	{
		m_deletedEntities.pop();
	}

	// Clear all component sets, entity component key sets, and systems.
	m_componentSets.clear();
	m_entityComponentKeys.clear();
	m_systems.clear();
}

Entity Registry::CreateEntity()
{
	Entity newEntity;

	// Create a new entity if we have non to recycle.
	if (m_deletedEntities.empty())
	{
		newEntity = m_nextEntity++;
	}

	// Otherwise reuse a previously created entity.
	else
	{
		newEntity = m_deletedEntities.front();
		m_deletedEntities.pop();
	}

	// Queue the entity for potential addition into all current systems.
	m_addedEntities.push_back(newEntity);

	return newEntity;
}

void Registry::RemoveEntity(Entity an_entity)
{
	// Queue the entity for removal for complete removal.
	m_removedEntities.push_back(an_entity);
}

void Registry::ProcessEntityAdditions()
{
	for (const Entity entity : m_addedEntities)
	{
		// Consider the entity for addition into all systems in the registry.
		for (std::unique_ptr<ISystem>& genericSystem : m_systems)
		{
			if (genericSystem != nullptr)
			{
				// Get the required component key of the system.
				const ComponentKey& requiredComponents = genericSystem->GetRequiredComponents();

				// If the entity component key has all the bits for system component set, add the entity to the system.
				if ((m_entityComponentKeys[entity] & requiredComponents) == requiredComponents)
				{
					genericSystem->AddEntity(entity);
				}
			}
		}
	}
	
	// Clear the list of pending entities.
	m_addedEntities.clear();
}

void Registry::ProcessEntityRemovals()
{
	for (const Entity entity : m_removedEntities)
	{
		// Remove all components belonging to the entity from every non empty component set.
		for (std::unique_ptr<IComponentSet>& genericComponentSet : m_componentSets)
		{
			if (genericComponentSet)
			{
				genericComponentSet->RemoveComponent(entity);
			}
		}

		// Remove all entity tags.
		std::vector<TagId>& entityTags = m_entityToTagMap[entity];
		for (const TagId tagId : entityTags)
		{
			std::vector<TagId>& entitiesWithTag = m_tagToEntityMap[tagId];
			m_tagToEntityMap[tagId].erase(std::find(entitiesWithTag.begin(), entitiesWithTag.end(), entity));
		}
		entityTags.clear();

		// Remove the entity from all systems.
		for (std::unique_ptr<ISystem>& genericSystem : m_systems)
		{
			genericSystem->RemoveEntity(entity);
		}

		// Reset the entity component key set.
		m_entityComponentKeys[entity].reset();

		// Add the entityId to the queue of recyclable entities.
		m_deletedEntities.push(entity);
	}

	// Clear the set of removed entities.
	m_removedEntities.clear();
}

void Registry::ProcessTagAdditions()
{
	// Execute each pending entity tag add request.
	for (std::unique_ptr<IRegistryRequestTag>& addTagRequest : m_addTagRequests)
	{
		addTagRequest->Execute();
	}

	// Clear the list of pending tag requests.
	m_addTagRequests.clear();
}

void Registry::ProcessTagRemovals()
{
	// Execute each pending entity tag remove request.
	for (std::unique_ptr<IRegistryRequestTag>& removeTagRequest : m_removeTagRequests)
	{
		removeTagRequest->Execute();
	}

	// Clear the list of pending tag removal requests.
	m_removeTagRequests.clear();
}

void Registry::ProcessComponentAdditions()
{
	// Execute each pending component addition request.
	for (std::unique_ptr<IRegistryRequestAddComponent>& addComponentRequest : m_addComponentRequests)
	{
		addComponentRequest->Execute();
	}

	// Clear the set of pending component addition requests.
	m_addComponentRequests.clear();
}

void Registry::ProcessComponnetRemovals()
{
	// Execute each pending component removal request.
	for (std::unique_ptr<IRegistryRequestRemoveComponent>& removeComponentRequest : m_removeComponentRequests)
	{
		removeComponentRequest->Execute();
	}

	// Clear the set of pending component removal requests.
	m_removeComponentRequests.clear();
}

