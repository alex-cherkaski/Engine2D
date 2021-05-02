#pragma once
#include "Events\Events.h"
#include "Macros.h"
#include "SceneManager\SceneManager.h"
#include "TileManager\TileManager.h"

class Engine final
{
public:
	SINGLETON(Engine);

	Engine();
	~Engine() = default;

	void Initialize();
	void Run();
	void Shutdown();

	void OnQuit(const QuitEvent& a_quitEvent);
	void OnKeyDown(const KeyDownEvent& a_keyDownEvent);
	void OnKeyUp(const KeyUpEvent& a_keyUpEvent);

	int GetWindowWidth() const { return m_windowWidth; }
	int GetWindowHeight() const { return m_windowHeight; }

	SDL_Window* GetEngineWindow() const { return m_window.get(); }
	SDL_Renderer* GetEngineRenderer() const { return m_renderer.get(); }

	const Registry& GetRegistryRead() const { return m_registry; }
	Registry& GetRegistryWrite() { return m_registry; }

	const SceneManager& GetSceneManagerRead() const { return m_sceneManager; }
	SceneManager& GetSceneManagerWrite() { return m_sceneManager; }

	const TileManager& GetTileManagerRead() const { return m_tileManager; }
	TileManager& GetTileManagerWrite() { return m_tileManager; }

	int GetMapWidth() const { return m_tileManager.GetMapWidth(); }
	int GetMapHeight() const { return m_tileManager.GetMapHeight(); }

private:
	void ProcessInput();
	void Update();
	void Render();

	void InitializeWindow();
	void SubscribeToEvents();

private:
	Registry m_registry;
	TileManager m_tileManager;
	SceneManager m_sceneManager;

	size_t m_lastUpdateTime = 0; // Stored in milliseconds.

	std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> m_window{ nullptr, SDL_DestroyWindow };
	std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> m_renderer{ nullptr, SDL_DestroyRenderer };

	const int m_windowWidth = 800;
	const int m_windowHeight = 600;

	bool m_isRunning = false;
};

