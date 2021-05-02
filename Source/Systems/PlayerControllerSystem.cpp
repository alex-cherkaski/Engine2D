#include "PCH.h"
#include "Components\Components.h"
#include "ECS\Registry.h"
#include "PlayerControllerSystem.h"
#include "Tags\Tags.h"
#include "TextureManager\TextureManager.h"

PlayerControllerSystem::PlayerControllerSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<VelocityComponent>();
	RequireComponent<AnimationComponent>();
	RequireComponent<PlayerControllerComponent>();
}

void PlayerControllerSystem::Initialize()
{
	// Get the projectile texture Id.
	const TextureManager& textureManager = TextureManager::GetInstanceRead();
	const size_t textureId = textureManager.GetTextureId("./Assets/Images/Projectile.png");

	// Retrieve player and animation components to calculate projectile offsets.
	assert(m_registry.GetEntitiesWithTag<PlayerTag>().size() == 1);
	const Entity player = m_registry.GetEntitiesWithTag<PlayerTag>()[0];
	const TransformComponent& transformComponent = m_registry.GetComponentRead<TransformComponent>(player);
	const AnimationComponent& animationComponent = m_registry.GetComponentRead<AnimationComponent>(player);

	// Calculate the projectile width and height to find the emission midpoint.
	const float halfProjectileWidth = textureManager.GetTextureWidth(textureId) * transformComponent.xScale / 2.0f;
	const float halfProjectileHeight = textureManager.GetTextureHeight(textureId) * transformComponent.yScale / 2.0f;

	// Calculate the player width and height to find the projectile emission midpoint.
	const float halfSpriteWidth = animationComponent.spriteWidth * transformComponent.xScale / 2.0f;
	const float halfSpriteHeight = animationComponent.spriteHeight * transformComponent.yScale / 2.0f;

	// Calculate and cache the projectile 
	m_xProjectileOffset = halfSpriteWidth - halfProjectileWidth;
	m_yProjectileOffset = halfSpriteHeight - halfProjectileHeight;

	// Cache the projectile entity texture id.
	m_projectileTextureId = TextureManager::GetInstanceRead().GetTextureId("./Assets/Images/Projectile.png");
}

void PlayerControllerSystem::Update(float a_deltaTime)
{
	if (m_registry.GetEntitiesWithTag<PlayerTag>().empty())
		return;

	// Retrieve the player entity.
	const Entity player = *m_entities.begin();

	// Retrieve the relevant components to update the player.
	TransformComponent& transformComponent = m_registry.GetComponentWrite<TransformComponent>(player);
	VelocityComponent& velocityComponent = m_registry.GetComponentWrite<VelocityComponent>(player);
	AnimationComponent& spriteComponent = m_registry.GetComponentWrite<AnimationComponent>(player);
	WeaponComponent& weaponComponent = m_registry.GetComponentWrite<WeaponComponent>(player);
	const PlayerControllerComponent& playerControllerComponent = m_registry.GetComponentRead<PlayerControllerComponent>(player);

	// Update the velocity based on the key pressed.
	if (m_keyboardState[SDL_SCANCODE_W] || m_keyboardState[SDL_SCANCODE_UP])			// Up
	{
		velocityComponent.y = -playerControllerComponent.m_playerSpeed;
		velocityComponent.x = 0;
	}
	else if (m_keyboardState[SDL_SCANCODE_S] || m_keyboardState[SDL_SCANCODE_DOWN])		// Down
	{
		velocityComponent.y = playerControllerComponent.m_playerSpeed;
		velocityComponent.x = 0;
	}
	else if (m_keyboardState[SDL_SCANCODE_D] || m_keyboardState[SDL_SCANCODE_RIGHT])	// Right
	{
		velocityComponent.x = playerControllerComponent.m_playerSpeed;
		velocityComponent.y = 0;
	}
	else if (m_keyboardState[SDL_SCANCODE_A] || m_keyboardState[SDL_SCANCODE_LEFT])		// Left
	{
		velocityComponent.x = -playerControllerComponent.m_playerSpeed;
		velocityComponent.y = 0;
	}

	// Emit a projectile if requested by the player, enough time has past since the previous emission, and the player has started moving.
	const unsigned int currenTime = SDL_GetTicks();
	if (m_keyboardState[SDL_SCANCODE_SPACE] 
		&& (currenTime >= weaponComponent.lastEmissionTime + weaponComponent.emissionCadence)
		&& (velocityComponent.x != 0 || velocityComponent.y != 0))
	{
		EmitProjectile(transformComponent, spriteComponent, velocityComponent);
		weaponComponent.lastEmissionTime = currenTime;
	}

	// Calculate the sprite row index.
	if (velocityComponent.y < 0)
		spriteComponent.currentRow = 0;
	else if (velocityComponent.y > 0)
		spriteComponent.currentRow = 2;
	if (velocityComponent.x > 0)
		spriteComponent.currentRow = 1;
	else if (velocityComponent.x < 0)
		spriteComponent.currentRow = 3;
}

void PlayerControllerSystem::Render()
{
}

void PlayerControllerSystem::EmitProjectile(const TransformComponent& a_playerTransform, const AnimationComponent& a_playerAnimation, const VelocityComponent& a_playerVelocity)
{
	// The speed of the emitted projectile.
	constexpr float PROJECTILE_SPEED = 200.0f;

	// From where the projectile will be emitted.
 	const float x = a_playerTransform.x + m_xProjectileOffset;
	const float y = a_playerTransform.y + m_yProjectileOffset;

	// Calculate the initial projectile direction.
	float xVelocity = 0.0f;
	float yVelocity = 0.0f;
	if (a_playerVelocity.y < 0)
		yVelocity = -1.0f;
	if (a_playerVelocity.y > 0)
		yVelocity = 1.0f;
	if (a_playerVelocity.x > 0)
		xVelocity = 1.0f;
	if (a_playerVelocity.x < 0)
		xVelocity = -1.0f;

	// Create the projectile.
  	const Entity projectile = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(projectile, { x, y, a_playerTransform.rotation, 1.0f, 1.0f  });
		m_registry.AddComponent<VelocityComponent>(projectile, { xVelocity * PROJECTILE_SPEED, yVelocity * PROJECTILE_SPEED });
		m_registry.AddComponent<TextureComponent>(projectile, { m_projectileTextureId, RenderOrder::ProjectileOrder });
		m_registry.AddComponent<ProjectileComponent>(projectile, { 1, ProjectileOwner::PlayerOwner });
		m_registry.AddComponent<CollisionComponent>(projectile, { 4, 4 });
 		m_registry.AddTag<ProjectileTag>(projectile);
	}
}

