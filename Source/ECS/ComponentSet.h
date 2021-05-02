#pragma once
#include "PCH.h"
#include "Types.h"
#include "Macros.h"

class IComponentSet
{
public:
	NO_COPY(IComponentSet);
	NO_MOVE(IComponentSet);

	IComponentSet() = default;
	virtual ~IComponentSet() = default;

	virtual bool HaveComponent(Entity an_entity) const = 0;
	virtual void RemoveComponent(Entity an_entity) = 0;
};

template<typename TComponent>
class ComponentSet final : public IComponentSet
{
public:
	ComponentSet() = default;
	~ComponentSet() = default;

	void AddComponent(Entity an_entity, const TComponent& a_component);
	bool HaveComponent(Entity an_entity) const override;
	const TComponent& GetComponentRead(Entity an_entity) const;
	TComponent& GetComponentWrite(Entity an_entity);
	void RemoveComponent(Entity an_entity) override;

	bool IsEmpty() const { return m_packedComponentData.empty(); }

private:
	std::unordered_map<Entity, size_t> m_entityIdToIndexMap;	// Book keeping map to help with lookup and erasing.
	std::unordered_map<Entity, size_t> m_indexToEntityIdMap;	// Book keeping map to help with lookup and erasing.
	std::vector<TComponent> m_packedComponentData;				// The contiguous vector of components.
};

template<typename TComponent>
inline void ComponentSet<TComponent>::AddComponent(Entity an_entity, const TComponent& a_component)
{
	// Check if the component already exists for this entity.
	const auto iterator = m_entityIdToIndexMap.find(an_entity);

	// If the entity does not have this component...
	if (iterator == m_entityIdToIndexMap.end())
	{
		// Add the component and update the book keeping maps.
		m_packedComponentData.push_back(a_component);
		m_entityIdToIndexMap[an_entity] = m_packedComponentData.size() - 1;
		m_indexToEntityIdMap[m_packedComponentData.size() - 1] = an_entity;
	}

	// Else overwrite this component.
	else
	{
		const size_t index = iterator->second;
		m_packedComponentData[index] = std::move(a_component);
	}
}

template<typename TComponent>
bool ComponentSet<TComponent>::HaveComponent(Entity an_entity) const
{
	return m_entityIdToIndexMap.find(an_entity) != m_entityIdToIndexMap.end();
}

template<typename TComponent>
const TComponent& ComponentSet<TComponent>::GetComponentRead(Entity an_entity) const
{
	// Ensure the component is present, and return it.
	const auto iterator = m_entityIdToIndexMap.find(an_entity);
	assert(iterator != m_entityIdToIndexMap.end());
	const size_t index = iterator->second;
	return m_packedComponentData[index];
}


template<typename TComponent>
TComponent& ComponentSet<TComponent>::GetComponentWrite(Entity an_entity)
{
	// Ensure the component is present, and return it.
	const auto iterator = m_entityIdToIndexMap.find(an_entity);
	assert(iterator != m_entityIdToIndexMap.end());
	const size_t index = iterator->second;
	return m_packedComponentData[index];
}

template<typename TComponent>
void ComponentSet<TComponent>::RemoveComponent(Entity an_entity)
{
	// If the component is present for this entity...
	const auto iterator = m_entityIdToIndexMap.find(an_entity);
	if (iterator != m_entityIdToIndexMap.end())
	{
		// Get the index and component to remove. Also get the component's owning entity.
		const size_t toEraseComponentIndex = iterator->second;
		TComponent& toEraseComponent = m_packedComponentData[toEraseComponentIndex];
		const Entity toEraseComponentEntity = iterator->first;

		// Get the last component and its index. Also get the component's owning entity.
		const size_t toKeepComponentIndex = m_packedComponentData.size() - 1;
		TComponent& toKeepComponent = m_packedComponentData[toKeepComponentIndex];
		const Entity toKeepComponentEntity = m_indexToEntityIdMap[toKeepComponentIndex];

		// Swap the component to be erased with the last component.
		std::swap(toEraseComponent, toKeepComponent);

		// Swap the entity-index map contents.
		std::swap(m_entityIdToIndexMap[toEraseComponentEntity], m_entityIdToIndexMap[toKeepComponentEntity]);

		// Swap the index-entity map contents.
		std::swap(m_indexToEntityIdMap[toEraseComponentIndex], m_indexToEntityIdMap[toKeepComponentIndex]);

		// Erase the last component from the packed vector.
		m_packedComponentData.erase(m_packedComponentData.end() - 1);

		// Erase the book keeping of data of the erased component.
		m_entityIdToIndexMap.erase(toEraseComponentEntity); // Component for this entity no longer exists after the erase.
		m_indexToEntityIdMap.erase(toKeepComponentIndex); // toKeepComponentIndex is no longer valid after erase.
	}
}

