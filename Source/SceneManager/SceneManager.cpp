#include "PCH.h"
#include "Components\Components.h"
#include "ECS\Registry.h"
#include "Engine.h"
#include "Enums\Enums.h"
#include "Macros.h"
#include "SceneManager.h"
#include "Systems\BoundsCheckingSystem.h"
#include "Systems\CameraFollowSystem.h"
#include "Systems\CollisionSystem.h"
#include "Systems\EntityMovementSystem.h"
#include "Systems\PlayerControllerSystem.h"
#include "Systems\SpriteUpdateSystem.h"
#include "Systems\TextureRenderSystem.h"
#include "Tags\Tags.h"
#include "TextureManager\TextureManager.h"
#include "TileManager\TileManager.h"

SceneManager::SceneManager(Registry& a_registry)
	: m_registry(a_registry)
{
}

void SceneManager::Initialize()
{
	InitializeRequiredManagers();
	CreateRequiredSystems();
	LoadRequiredAssets();
	CreateRequiredEntities();

	// Initialize all systems, and handle any pending entity, component, and tag  requests.
	m_registry.RunSystemsInitialize();
}

void SceneManager::Update(float a_deltaTime)
{
	// Process all pending entity additions and removals, and rune the update routine of each system.
	m_registry.RunSystemsUpdate(a_deltaTime);
}

void SceneManager::Render()
{
	// Run the render routine of all registered systems.
	m_registry.RunSystemsRender();
}

void SceneManager::Shutdown()
{
}

void SceneManager::InitializeRequiredManagers()
{
	TextureManager::GetInstanceWrite().Initialize();
}

void SceneManager::CreateRequiredSystems()
{
	// The order at which the systems sequentially update and render.
	m_registry.AddSystem<PlayerControllerSystem>();
	m_registry.AddSystem<EntityMovementSystem>();
	m_registry.AddSystem<BoundsCheckingSystem>();
	m_registry.AddSystem<CameraFollowSystem>();
	m_registry.AddSystem<CollisionSystem>();
	m_registry.AddSystem<SpriteUpdateSystem>();
	m_registry.AddSystem<TextureRenderSystem>();
}

void SceneManager::LoadRequiredAssets()
{
	Engine::GetInstanceWrite().GetTileManagerWrite().CreateMap("./Assets/Maps/Jungle.map", "./Assets/Maps/Jungle.png", 20, 25, 32, 32);
}

void SceneManager::CreateRequiredEntities()
{
	const Engine& engine = Engine::GetInstanceRead();

	TextureManager& textureManager = TextureManager::GetInstanceWrite();
	const TextureId chopperSpritesheet = textureManager.GetTextureId("./Assets/Images/Chopper-Spritesheet.png");
	const TextureId destroyedTankTexture = textureManager.GetTextureId("./Assets/Images/DestroyedTank.png");
	const TextureId truckTexture = textureManager.GetTextureId("./Assets/Images/Truck.png");
	const TextureId airplaneSpritesheet = textureManager.GetTextureId("./Assets/Images/Airplane-Spritesheet.png");
	const TextureId tankTexture = textureManager.GetTextureId("./Assets/Images/Tank.png");
	const TextureId landingBaseTexture = textureManager.GetTextureId("./Assets/Images/LandingBase.png");
	const TextureId boatTexture = textureManager.GetTextureId("./Assets/Images/Boat.png");
	const TextureId carrierTexture = textureManager.GetTextureId("./Assets/Images/Carrier.png");
	const TextureId jetSpritesheet = textureManager.GetTextureId("./Assets/Images/Jet-Spritesheet.png");

	// Player.
	const Entity player = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(player, { 239.0f, 115.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<VelocityComponent>(player, { 0.0f, 0.0f });
		m_registry.AddComponent<AnimationComponent>(player, { 125, 0, 32, 32, 4, 2, 0, 0 });
		m_registry.AddComponent<PlayerControllerComponent>(player, { 100.0f });
		m_registry.AddComponent<TextureComponent>(player, { chopperSpritesheet, RenderOrder::PlayerOrder });
		m_registry.AddComponent<WeaponComponent>(player, { 1000, 0, 5, ProjectileOwner::PlayerOwner, false });
		m_registry.AddComponent<CollisionComponent>(player, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(player, { 5, 5 });
		m_registry.AddTag<PlayerTag>(player);
	}

	// Camera.
	const Entity camera = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(camera, { 0.0f, 0.0f, 0.0f });
		m_registry.AddComponent<CameraComponent>(camera, { engine.GetWindowWidth(), engine.GetWindowHeight() });
		m_registry.AddTag<CameraTag>(camera);
	}

	// Destroyed Tank.
	const Entity destroyedTank = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(destroyedTank, { 670.0f, 530.0f, 270.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(destroyedTank, { destroyedTankTexture, RenderOrder::TopTileOrder });
		m_registry.AddTag<SceneryTag>(destroyedTank);
	}

	// Truck.
	const Entity truck = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(truck, { 660.0f, 420.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(truck, { truckTexture, RenderOrder::NPCOrder });
		m_registry.AddComponent<CollisionComponent>(truck, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(truck, { 5, 5 });
		m_registry.AddTag<NPCTag>(truck);
	}

	// Airplane1.
	const Entity airplane1 = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(airplane1, { 160.0f, 497.0f, 90.0f, 1.0f, 1.0f });
		m_registry.AddComponent<VelocityComponent>(airplane1, { 100.0f, 0.0f });
		m_registry.AddComponent<TextureComponent>(airplane1, { airplaneSpritesheet , RenderOrder::NPCOrder });
		m_registry.AddComponent<AnimationComponent>(airplane1, { 125, 0, 32, 32, 1, 3, 0, 0 });
		m_registry.AddComponent<CollisionComponent>(airplane1, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(airplane1, { 5, 5 });
		m_registry.AddTag<NPCTag>(airplane1);
	}

	// Airplane2.
	const Entity airplane2 = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(airplane2, { 440.0f, 380.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(airplane2, { airplaneSpritesheet , RenderOrder::NPCOrder });
		m_registry.AddComponent<AnimationComponent>(airplane2, { 125, 0, 32, 32, 1, 3, 0, 0 });
		m_registry.AddComponent<CollisionComponent>(airplane2, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(airplane2, { 5, 5 });
		m_registry.AddTag<NPCTag>(airplane2);
	}

	// Airplane3.
	const Entity airplane3 = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(airplane3, { 440.0f, 420.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(airplane3, { airplaneSpritesheet , RenderOrder::NPCOrder });
		m_registry.AddComponent<AnimationComponent>(airplane3, { 125, 0, 32, 32, 1, 3, 0, 0 });
		m_registry.AddComponent<CollisionComponent>(airplane3, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(airplane3, { 5, 5 });
		m_registry.AddTag<NPCTag>(airplane3);
	}

	// Airplane4.
	const Entity airplane4 = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(airplane4, { 490.0f, 420.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(airplane4, { airplaneSpritesheet , RenderOrder::NPCOrder });
		m_registry.AddComponent<AnimationComponent>(airplane4, { 125, 0, 32, 32, 1, 3, 0, 0 });
		m_registry.AddComponent<CollisionComponent>(airplane4, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(airplane4, { 5, 5 });
		m_registry.AddTag<NPCTag>(airplane4);
	}

	// Airplane5.
	const Entity airplane5 = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(airplane5, { 490.0f, 380.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(airplane5, { airplaneSpritesheet , RenderOrder::NPCOrder });
		m_registry.AddComponent<AnimationComponent>(airplane5, { 125, 0, 32, 32, 1, 3, 0, 0 });
		m_registry.AddComponent<CollisionComponent>(airplane5, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(airplane5, { 5, 5 });
		m_registry.AddTag<NPCTag>(airplane5);
	}

	// Tank.
	const Entity tank = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(tank, { 500.0f, 530.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(tank, { tankTexture, RenderOrder::NPCOrder });
		m_registry.AddComponent<CollisionComponent>(tank, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(tank, { 5, 5 });
		m_registry.AddTag<NPCTag>(tank);
	}

	// Landing Base.
	const Entity landingBase = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(landingBase, { 239.0f, 115.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(landingBase, { landingBaseTexture, RenderOrder::TopTileOrder });
		m_registry.AddTag<SceneryTag>(landingBase);
	}

	// Boat.
	const Entity boat = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(boat, { 720.0f, 230.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(boat, { boatTexture, RenderOrder::TopTileOrder });
		m_registry.AddTag<SceneryTag>(boat);
	}

	// Carrier.
	const Entity carrier = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(carrier, { 550.0f, 295.0f, 0.0f, 1.0f, 1.0f });
		m_registry.AddComponent<TextureComponent>(carrier, { carrierTexture, RenderOrder::TopTileOrder });
		m_registry.AddTag<SceneryTag>(carrier);
	}

	// Jet.
	const Entity jet = m_registry.CreateEntity();
	{
		m_registry.AddComponent<TransformComponent>(jet, { 570.0f, 450.0f, 350.0f, 1.0f, 1.0f });
		m_registry.AddComponent<VelocityComponent>(jet, { 100 * (float)cos(259.0 * (float)M_PI / 180), 100 * (float)sin(259.0 * (float)M_PI / 180) });
		m_registry.AddComponent<TextureComponent>(jet, { jetSpritesheet, RenderOrder::NPCOrder });
		m_registry.AddComponent<AnimationComponent>(jet, { 125, 0, 32, 32, 1, 2, 0, 0 });
		m_registry.AddComponent<CollisionComponent>(jet, { 32.0f, 32.0f });
		m_registry.AddComponent<HealthComponent>(jet, { 5, 5 });
		m_registry.AddTag<NPCTag>(jet);
	}
}

