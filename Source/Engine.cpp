#include "PCH.h"
#include "Engine.h"
#include "EventManager\EventManager.h"
#include "Events\Events.h"
#include "Constants\Constants.h"
#include "TextureManager\TextureManager.h"
#include "Components\Components.h"

Engine::Engine()
	: m_sceneManager(m_registry)
{
}

void Engine::Initialize()
{
	InitializeWindow();
	SubscribeToEvents();
	m_sceneManager.Initialize();
	m_isRunning = true;
}

void Engine::Run()
{
	while (m_isRunning)
	{
		ProcessInput();
		Update();
		Render();
	}
}

void Engine::Shutdown()
{
	// Release all allocated resources and shutdown SDL.
	m_renderer.reset();
	m_window.reset();
	SDL_Quit();
}

void Engine::OnQuit(const QuitEvent& a_quitEvent)
{
	m_isRunning = false;
}

void Engine::OnKeyDown(const KeyDownEvent& a_keyDownEvent)
{
}

void Engine::OnKeyUp(const KeyUpEvent& a_keyUpEvent)
{
	// Escape key shuts down the engine.
	if (a_keyUpEvent.m_keyCode == SDLK_ESCAPE)
	{
		m_isRunning = false;
	}
}

void Engine::ProcessInput()
{
	// Event manager to notify all event subscribers
	static EventManager& eventManager = EventManager::GetInstanceWrite();

	SDL_Event sdlEvent = { 0 };
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
		{
		case SDL_QUIT:
		{
			eventManager.EmitEvent<QuitEvent>({}, EventPriority::Immediate);
			break;
		}
		case SDL_KEYDOWN:
		{
			eventManager.EmitEvent<KeyDownEvent>({ sdlEvent.key.keysym.sym }, EventPriority::Immediate);
			break;
		}
		case SDL_KEYUP:
		{
			eventManager.EmitEvent<KeyUpEvent>({ sdlEvent.key.keysym.sym }, EventPriority::Immediate);
			break;
		}
		default:
			break;
		}
	}
}

void Engine::Update()
{
#ifdef _DEBUG
	const float deltaTime = static_cast<float>(fmin(0.05f, (SDL_GetTicks() - m_lastUpdateTime) / 1000.0f));
#else
	const float deltaTime = static_cast<float>(SDL_GetTicks() - m_lastUpdateTime) / 1000.0f;
#endif // _DEBUG

	m_sceneManager.Update(deltaTime);

	m_lastUpdateTime = SDL_GetTicks();
}

void Engine::Render()
{
	// Set the render window draw color.
	SDL_SetRenderDrawColor(m_renderer.get(), 0, 0, 255, 255);

	// Clear the render window.
	SDL_RenderClear(m_renderer.get());

	m_sceneManager.Render();
	/*
	static const auto once = [&m_registry = const_cast<Registry&>(m_registry)]() -> bool
		{
			// Retrieve the projectile texture.
			TextureManager::GetInstanceWrite().LoadTexture("./Assets/Images/Projectile.png");
			const size_t textureId = TextureManager::GetInstanceRead().GetTextureId("./Assets/Images/Projectile.png");
			// Create the projectile.
			const Entity projectile = m_registry.CreateEntity();
			{
				m_registry.AddComponent<TransformComponent>(projectile, { 50, 50, 0.0f, 1.0f, 1.0f });
				m_registry.AddComponent<VelocityComponent>(projectile, { 0, 0 });
				m_registry.AddComponent<TextureComponent>(projectile, { textureId, 0 });
			}
			return true;
		};


	const size_t textureId = TextureManager::GetInstanceRead().GetTextureId("./Assets/Images/Projectile.png");
	SDL_Texture* texture = TextureManager::GetInstanceRead().GetTexture(textureId);

	SDL_Rect rect = { 50, 50, 4, 4, };
	SDL_RenderCopy(m_renderer.get(), texture, nullptr, &rect);
	*/
	// Swap the SDL front and back buffers in the window.
	SDL_RenderPresent(m_renderer.get());
}

void Engine::InitializeWindow()
{
	// Initialize all SDL subsystems.
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		assert(false);
		exit(EXIT_FAILURE);
	}
	
	// Create the main engine window.
	m_window.reset(SDL_CreateWindow(
		"Engine",					// Window title.
		SDL_WINDOWPOS_CENTERED,		// Window position on screen.
		SDL_WINDOWPOS_CENTERED,		// Window position on screen.
		m_windowWidth,				// Window width.
		m_windowHeight,				// Window height.
		0							// Window creation flags.
	));

	if (!m_window)
	{
		fprintf(stderr, "Error creating window.\n");
		assert(false);
		exit(EXIT_FAILURE);
	}

	// Create the main engine window renderer.
	m_renderer.reset(SDL_CreateRenderer(
		m_window.get(),			// Window the renderer belongs to.
		-1,						// Default renderer driver type.
		0						// Additional render flags.
	));

	if (!m_renderer)
	{
		fprintf(stderr, "Error creating renderer.\n");
		assert(false);
		exit(EXIT_FAILURE);
	}
}

void Engine::SubscribeToEvents()
{
	// Subscribe callbacks to the desired events.
	EventManager& eventManager = EventManager::GetInstanceWrite();
	eventManager.SubscribeToEvent<QuitEvent, Engine>(this, &Engine::OnQuit);
	eventManager.SubscribeToEvent<KeyDownEvent, Engine>(this, &Engine::OnKeyDown);
	eventManager.SubscribeToEvent<KeyUpEvent, Engine>(this, &Engine::OnKeyUp);
}

