#pragma once
#include "ECS\System.h"
#include "Components\Components.h"

class PlayerControllerSystem final : public ISystem
{
public:
	PlayerControllerSystem(Registry& a_registry);
	~PlayerControllerSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;

private:
	void EmitProjectile(const TransformComponent& a_playerTransform, const AnimationComponent& a_playerAnimation, const VelocityComponent& a_playerVelocity);

private:
	const Uint8* m_keyboardState = SDL_GetKeyboardState(nullptr);
	size_t m_projectileTextureId = 0;

	float m_halfProjectileWidth = 0.0f;
	float m_halfProjectileHeight = 0.0f;

	float m_halfSpriteWidth = 0.0f;
	float m_halfSpriteHeight = 0.0f;

	float m_xProjectileOffset = 0;
	float m_yProjectileOffset = 0;
};

