#pragma once
#include "ECS\System.h"

class SpriteUpdateSystem final : public ISystem
{
public:
	SpriteUpdateSystem(Registry& a_registry);
	~SpriteUpdateSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;
};

