#pragma once
#include "PCH.h"
#include "ComponentIdGenerator.h"
#include "Types.h"
#include "Macros.h"

class Registry;

class ISystem
{
public:
	NO_COPY(ISystem);
	NO_MOVE(ISystem);

	ISystem(Registry& a_registry) : m_registry(a_registry) {}
	virtual ~ISystem() = default;

	virtual void Initialize() = 0;
	virtual void Update(float a_deltaTime) = 0;
	virtual void Render() = 0;

	void AddEntity(Entity an_entity) { m_entities.insert(an_entity); }
	void RemoveEntity(Entity an_entity) { m_entities.erase(an_entity); }

	const ComponentKey& GetRequiredComponents() const { return m_requiredComponents; }

protected:
	template<typename TComponent> void RequireComponent();

protected:
	std::set<Entity> m_entities;		// The set of entities processed by this system.
	ComponentKey m_requiredComponents;	// The set of all required components to process an entity.
	Registry& m_registry;
};

template<typename TComponent>
void ISystem::RequireComponent()
{
	// Get the component id and set in the component key bit set.
	const ComponentId componentId = ComponentIdGenerator::GetComponentId<TComponent>();
	m_requiredComponents.set(componentId);
}
