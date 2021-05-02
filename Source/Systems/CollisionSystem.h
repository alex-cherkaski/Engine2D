#pragma once
#include "ECS\System.h"
#include "ECS\Registry.h"
#include "Events\Events.h"

class CollisionSystem final : public ISystem
{
public:
	CollisionSystem(Registry& a_registry);
	~CollisionSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;

	void OnCollision(const CollisionEvent& a_collisionEvent);

private:
	bool CollideAABB(const Entity entity1, const Entity entity2) const;
	template<typename TTag1, typename TTag2> bool HaveTags(const Entity entity1, const Entity entity2);
};

template<typename TTag1, typename TTag2>
inline bool CollisionSystem::HaveTags(const Entity entity1, const Entity entity2)
{
	// Check if the pair of entities - among themselves - contain the specified pair of tags.
	return (m_registry.HaveTag<TTag1>(entity1) && m_registry.HaveTag<TTag2>(entity2))
		|| (m_registry.HaveTag<TTag1>(entity2) && m_registry.HaveTag<TTag2>(entity1));
}

