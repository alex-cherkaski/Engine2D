#pragma once
#include "ECS\Registry.h"
#include "Macros.h"

class SceneManager final
{
public:
	SceneManager(Registry& a_registry);
	~SceneManager() = default;

	void Initialize();
	void Update(float a_deltaTime);
	void Render();
	void Shutdown();

private:
	void InitializeRequiredManagers();
	void CreateRequiredSystems();
	void LoadRequiredAssets();
	void CreateRequiredEntities();

private:
	Registry& m_registry;
};

