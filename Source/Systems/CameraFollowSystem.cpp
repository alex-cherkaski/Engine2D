#include "PCH.h"
#include "CameraFollowSystem.h"
#include "Components\Components.h"
#include "ECS\Registry.h"
#include "Tags\Tags.h"
#include "Engine.h"
#include "TileManager\TileManager.h"

CameraFollowSystem::CameraFollowSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<CameraComponent>();
}

void CameraFollowSystem::Initialize()
{
	// Cache camera width and height parameters.
	const TileManager& tileManager = Engine::GetInstanceRead().GetTileManagerRead();
	m_mapWidth = tileManager.GetMapWidth();
	m_mapHeight = tileManager.GetMapHeight();

	// Cache player entity.
	assert(m_registry.GetEntitiesWithTag<PlayerTag>().size() == 1);
	m_playerEntity = m_registry.GetEntitiesWithTag<PlayerTag>()[0];

	// Retrieve player transform and animation components to calculate and cache camera bounds.
	const TransformComponent& playerTransform = m_registry.GetComponentRead<TransformComponent>(m_playerEntity);
	const AnimationComponent& playerAnimation = m_registry.GetComponentRead<AnimationComponent>(m_playerEntity);

	// Retrieve camera component to calculate camera entity bounds.
	assert(m_entities.size() == 1);
	const Entity camera = *m_entities.begin();
	const CameraComponent& cameraComponent = m_registry.GetComponentRead<CameraComponent>(camera);

	// Cache camera lower bound follow parameters.
	m_xCameraLowerBound = (playerAnimation.spriteWidth * playerTransform.xScale / 2.0f) - (cameraComponent.cameraWidth / 2.0f);
	m_yCameraLowerBound = (playerAnimation.spriteHeight * playerTransform.yScale / 2.0f) - (cameraComponent.cameraHeight / 2.0f);

	// Cache camera upper bound follow parameters.
	m_xCameraUpperBound = static_cast<float>(fabs(m_mapWidth - cameraComponent.cameraWidth));
	m_yCameraUpperBound = static_cast<float>(fabs(m_mapHeight - cameraComponent.cameraHeight));
}

void CameraFollowSystem::Update(float a_deltaTime)
{
	// Only update the camera position if there is a valid player to follow.
	if (m_registry.GetEntitiesWithTag<PlayerTag>().empty())
		return;

	// Retrieve the camera entity and components.
	const Entity camera = *m_entities.begin();
	TransformComponent& cameraTransform = m_registry.GetComponentWrite<TransformComponent>(camera);
	const CameraComponent& cameraComponent = m_registry.GetComponentRead<CameraComponent>(camera);

	// Retrieve player transform component to update camera position.
	const TransformComponent& playerTransform = m_registry.GetComponentRead<TransformComponent>(m_playerEntity);

	// Top and left camera bounding box.
	cameraTransform.x = fmax(0.0f, playerTransform.x + m_xCameraLowerBound);
	cameraTransform.y = fmax(0.0f, playerTransform.y + m_yCameraLowerBound);

	// Bottom and right camera bounding box.
	cameraTransform.x = fmin(cameraTransform.x, m_xCameraUpperBound);
	cameraTransform.y = fmin(cameraTransform.y, m_yCameraUpperBound);
}

void CameraFollowSystem::Render()
{
}

