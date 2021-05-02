#pragma once
#include "ECS\System.h"

class EntityMovementSystem final : public ISystem
{
public:
	EntityMovementSystem(Registry& a_registry);
	~EntityMovementSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;
};

