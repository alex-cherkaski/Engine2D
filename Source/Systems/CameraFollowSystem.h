#pragma once
#include "ECS\System.h"
#include "Constants\Constants.h"

class Registry;

class CameraFollowSystem final : public ISystem
{
public:
	CameraFollowSystem(Registry& a_registry);
	~CameraFollowSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;

private:
	Entity m_cameraEntity = INVALID_ENTITY;
	Entity m_playerEntity = INVALID_ENTITY;

	int m_mapWidth = 0;
	int m_mapHeight = 0;

	float m_xCameraLowerBound = 0.0f;
	float m_yCameraLowerBound = 0.0f;

	float m_xCameraUpperBound = 0.0f;
	float m_yCameraUpperBound = 0.0f;
};

