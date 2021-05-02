#include "PCH.h"
#include "Components\Components.h"
#include "Tags\Tags.h"
#include "TextureRenderSystem.h"
#include "EventManager\EventManager.h"

TextureRenderSystem::TextureRenderSystem(Registry& a_registry)
	: ISystem::ISystem(a_registry)
{
	RequireComponent<TransformComponent>();
	RequireComponent<TextureComponent>();
}

void TextureRenderSystem::Initialize()
{
	// Cache the camera entity to query for culling relevant components.
	assert(m_registry.GetEntitiesWithTag<CameraTag>().size() == 1);
	m_cameraEntity = m_registry.GetEntitiesWithTag<CameraTag>()[0];

	// Subscribe to key up events to toggle render debug mode.
	EventManager& eventManager = EventManager::GetInstanceWrite();
	eventManager.SubscribeToEvent<KeyUpEvent, TextureRenderSystem>(this, &TextureRenderSystem::OnKeyUp);
}

void TextureRenderSystem::Update(float a_deltaTime)
{
}

void TextureRenderSystem::Render()
{
	static SDL_Renderer* renderer = Engine::GetInstanceRead().GetEngineRenderer();
	static const TextureManager& textureManager = TextureManager::GetInstanceRead();

	// Retrieve relevant camera components to perform the culling.
	const TransformComponent& cameraTransform = m_registry.GetComponentRead<TransformComponent>(m_cameraEntity);
	const CameraComponent& cameraComponent = m_registry.GetComponentRead<CameraComponent>(m_cameraEntity);

	// Lambda to cull all textures and sprites that are outside of the visible window area. 
	const auto filter =
	[
		&a_registry = static_cast<const Registry&>(m_registry),
		&a_cameraTransform = static_cast<const TransformComponent&>(cameraTransform),
		&a_cameraComponent = static_cast<const CameraComponent&>(cameraComponent)
	]
	(const Entity entity) -> bool
	{
		// The current camera viewport bounds.		
		const int cameraEndX = static_cast<int>(round(a_cameraTransform.x + a_cameraComponent.cameraWidth));
		const int cameraEndY = static_cast<int>(round(a_cameraTransform.y + a_cameraComponent.cameraHeight));

		bool shouldKeepEntity = false;

		// Retrieve the transform of the entity being considered for rendering.
		const TransformComponent& transformComponent = a_registry.GetComponentRead<TransformComponent>(entity);

		// Handle tile entity culling.
		if (a_registry.HaveComponent<TileComponent>(entity))
		{
			// Retrieve the tile component to perform bounds checking calculations.
			const TileComponent& tileComponent = a_registry.GetComponentRead<TileComponent>(entity);
			
			// Check if the tile texture overlaps with the viewport x bounds.
			const bool xBounds = (transformComponent.x + tileComponent.tileWidth * transformComponent.xScale >= a_cameraTransform.x) &&
				(transformComponent.x < cameraEndX);

			// Check if the tile texture overlaps with the viewport y bounds.
			const bool yBounds = (transformComponent.y + tileComponent.tileHeight * transformComponent.yScale >= a_cameraTransform.y) &&
				(transformComponent.y < cameraEndY);

			// Keep the tile texture for rendering if it overlaps with the viewport in the x and y axis.
			shouldKeepEntity = xBounds && yBounds;
		}

		// Handle non-tile-like textures.
		else if (a_registry.HaveTag<ProjectileTag>(entity)
			|| a_registry.HaveTag<NPCTag>(entity)
			|| a_registry.HaveTag<SceneryTag>(entity))
		{
			// Retrieve the texture component to query for texture parameters.
			const TextureComponent& textureComponent = a_registry.GetComponentRead<TextureComponent>(entity);
			
			// Get the texture width and height to for bounds checking calculations.
			static const TextureManager& textureManager = TextureManager::GetInstanceRead();
			const int textureWidth = textureManager.GetTextureWidth(textureComponent.textureId);
			const int textureHeight = textureManager.GetTextureHeight(textureComponent.textureId);

			// Check if the tile texture overlaps with the viewport x bounds.
			const bool xBounds = (transformComponent.x + textureWidth * transformComponent.xScale >= a_cameraTransform.x) &&
				(transformComponent.x < cameraEndX);

			// Check if the tile texture overlaps with the viewport y bounds.
			const bool yBounds = (transformComponent.y + textureHeight * transformComponent.yScale >= a_cameraTransform.y) &&
				(transformComponent.y < cameraEndY);

			// Keep the tile texture for rendering if it overlaps with the viewport in the x and y axis.
			shouldKeepEntity = xBounds && yBounds;
		}

		// Handle player case.
		else if (a_registry.HaveTag<PlayerTag>(entity))
		{
			// The player should always be on the screen.
			shouldKeepEntity = true;
		}

		return shouldKeepEntity;
	};

	// Lambda to compare the render order of both entities to render the lesser render order texture first.
	const auto sort = [&a_registry = static_cast<const Registry&>(m_registry)](const Entity entity1, const Entity entity2) -> bool
	{
		const TextureComponent& textureComponent1 = a_registry.GetComponentRead<TextureComponent>(entity1);
		const TextureComponent& textureComponent2 = a_registry.GetComponentRead<TextureComponent>(entity2);
		return textureComponent1.renderOrder < textureComponent2.renderOrder;
	};

	// Cull entities not visible in the viewport, and sort entities according to their texture render order before rendering.
	std::vector<Entity> entitiesToRender;
	std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(entitiesToRender), filter);
	std::sort(entitiesToRender.begin(), entitiesToRender.end(), sort);

	// Render each texture and sprite.
	for (const Entity& entity : entitiesToRender)
	{
		if (m_registry.HaveComponent<AnimationComponent>(entity))
			RenderSprite(entity, m_registry, renderer, textureManager);
		else if (m_registry.HaveComponent<TileComponent>(entity))
			RenderTile(entity, m_registry, renderer, textureManager);
		else
			RenderTexture(entity, m_registry, renderer, textureManager);

		// Render health bar for entities with a health component.
		if (m_registry.HaveComponent<HealthComponent>(entity))
			RenderHealthBar(entity, m_registry, renderer, textureManager);

		// If debug mode is toggled, render the AABB boxes of the relevant entities.
		if (m_debugModeEnabled && m_registry.HaveComponent<CollisionComponent>(entity))
			RenderColliders(entity, m_registry, renderer);
	}
}

void TextureRenderSystem::RenderTile(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager)
{
	// Retrieve the camera transform for render position related offsets and calculations.
	const TransformComponent& cameraTransform = a_registry.GetComponentRead<TransformComponent>(m_cameraEntity);

	// Retrieve the necessary entity components to render the tile.
	const TransformComponent& transformComponent = a_registry.GetComponentRead<TransformComponent>(entity);
	const TextureComponent& textureComponent = a_registry.GetComponentRead<TextureComponent>(entity);
	const TileComponent& tileComponent = a_registry.GetComponentRead<TileComponent>(entity);

	// Retrieve the texture containing the tile map that we will read from.
	SDL_Texture* tilemapTexture = a_textureManager.GetTexture(textureComponent.textureId);

	// Where on the texture to read the tile from.
	const SDL_Rect sourceRectangle = {
		tileComponent.tileX,
		tileComponent.tileY,
		tileComponent.tileWidth,
		tileComponent.tileHeight
	};

	// Where on the renderer to write the texture.
	const SDL_Rect destinationRectangle = {
		static_cast<int>(round(transformComponent.x - cameraTransform.x)),
		static_cast<int>(round(transformComponent.y - cameraTransform.y)),
		static_cast<int>(round(tileComponent.tileWidth * transformComponent.xScale)),
		static_cast<int>(round(tileComponent.tileHeight * transformComponent.yScale))
	};

	// Render the texture onto the back buffer.
	SDL_RenderCopy(a_renderer, tilemapTexture, &sourceRectangle, &destinationRectangle);
}

void TextureRenderSystem::RenderSprite(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager)
{
	// Retrieve the camera transform for render position related offsets and calculations.
	const TransformComponent& cameraTransform = a_registry.GetComponentRead<TransformComponent>(m_cameraEntity);

	// Get the relevant components required to render the player texture.
	const TransformComponent& transformComponent = a_registry.GetComponentRead<TransformComponent>(entity);
	const AnimationComponent& spriteComponent = a_registry.GetComponentRead<AnimationComponent>(entity);
	const TextureComponent& textureComponent = a_registry.GetComponentRead<TextureComponent>(entity);

	// Get the player texture itself.
	SDL_Texture* currentPlayerTexture = a_textureManager.GetTexture(textureComponent.textureId);

	// The position and dimensions of the texture that will be copied.
	const SDL_Rect sourceRect = {
		spriteComponent.currentColumn * spriteComponent.spriteWidth,
		spriteComponent.currentRow * spriteComponent.spriteHeight,
		spriteComponent.spriteWidth,
		spriteComponent.spriteHeight
	};

	// The positions and destination of where the texture will be copied too.
	const SDL_Rect destinationRect = {
		static_cast<int>(round(transformComponent.x - cameraTransform.x)),
		static_cast<int>(round(transformComponent.y - cameraTransform.y)),
		static_cast<int>(round(spriteComponent.spriteWidth * transformComponent.xScale)),
		static_cast<int>(round(spriteComponent.spriteHeight * transformComponent.yScale))
	};

	// Render the sprite onto the back buffer.
	SDL_RenderCopyEx(a_renderer, currentPlayerTexture, &sourceRect, &destinationRect, transformComponent.rotation, nullptr, SDL_FLIP_NONE);
}

void TextureRenderSystem::RenderTexture(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager)
{
	// Retrieve the camera transform for render position related offsets and calculations.
	const TransformComponent& cameraTransform = a_registry.GetComponentRead<TransformComponent>(m_cameraEntity);

	// Retrieve the necessary entity components to render the tile.
	const TransformComponent& transformComponent = a_registry.GetComponentRead<TransformComponent>(entity);
	const TextureComponent& textureComponent = a_registry.GetComponentRead<TextureComponent>(entity);

	const int textureWidth = a_textureManager.GetTextureWidth(textureComponent.textureId);
	const int textureHeight = a_textureManager.GetTextureHeight(textureComponent.textureId);

	// Retrieve the texture containing the tile map that we will read from.
	SDL_Texture* texture = a_textureManager.GetTexture(textureComponent.textureId);

	// Where on the texture to read the tile from.
	const SDL_Rect sourceRectangle = {
		0,
		0,
		textureWidth,
		textureHeight
	};

	// Where on the renderer to write the texture.
	const SDL_Rect destinationRectangle = {
		static_cast<int>(round(transformComponent.x - cameraTransform.x)),
		static_cast<int>(round(transformComponent.y - cameraTransform.y)),
		static_cast<int>(round(textureWidth * transformComponent.xScale)),
		static_cast<int>(round(textureHeight * transformComponent.yScale))
	};

	// Render the texture onto the back buffer.
	SDL_RenderCopyEx(a_renderer, texture, &sourceRectangle, &destinationRectangle, transformComponent.rotation, nullptr, SDL_FLIP_NONE);
}

void TextureRenderSystem::RenderHealthBar(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager)
{
	// Retrieve the camera transform for render position related offsets and calculations.
	const TransformComponent& cameraTransform = a_registry.GetComponentRead<TransformComponent>(m_cameraEntity);

	// Retrieve the relevant entity components required to render the health bar.
	const TransformComponent& transformComponent = a_registry.GetComponentRead<TransformComponent>(entity);
	const TextureComponent& textureComponent = a_registry.GetComponentRead<TextureComponent>(entity);
	const HealthComponent& healthComponent = a_registry.GetComponentRead<HealthComponent>(entity);

	// Get the texture width and height to for bounds checking calculations.
	int textureWidth = 0;
	int textureHeight = 0;

	// Use sprite dimensions for texture width and height if an animation component is present on the entity.
	if (a_registry.HaveComponent<AnimationComponent>(entity))
	{
		const AnimationComponent& animationComponent = a_registry.GetComponentRead<AnimationComponent>(entity);
		textureWidth = static_cast<int>(round(animationComponent.spriteWidth * transformComponent.xScale));
		textureHeight = static_cast<int>(round(animationComponent.spriteHeight * transformComponent.yScale));
	}

	// Else use the regular texture.
	else
	{
		textureWidth = a_textureManager.GetTextureWidth(textureComponent.textureId);
		textureHeight = a_textureManager.GetTextureHeight(textureComponent.textureId);
	}

	// Calculate the current health of the player to determine the health bar desired length.
	const float healthPercentage = static_cast<float>(healthComponent.currentHealthPoints) / static_cast<float>(healthComponent.totalHealthPoints);

	// Calculate health bar rectangle with and height.
	const int rectangleWidth = static_cast<int>(round(healthPercentage * textureWidth * transformComponent.xScale));
	const int rectangleHeight = static_cast<int>(round(0.1f * textureHeight * transformComponent.yScale));

	SDL_Rect healthBar = {
		static_cast<int>(round(transformComponent.x - cameraTransform.x)),
		static_cast<int>(round(transformComponent.y - cameraTransform.y)),
		static_cast<int>(round(rectangleWidth)),
		static_cast<int>(round(rectangleHeight))
	};

	// Render the health bar in green.
	SDL_SetRenderDrawColor(a_renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(a_renderer, &healthBar);
}

void TextureRenderSystem::RenderColliders(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer)
{
	// Get the relevant components required to render the AABB box.
	const TransformComponent& transformComponent = m_registry.GetComponentRead<TransformComponent>(entity);
	const CollisionComponent& collisionComponent = m_registry.GetComponentRead<CollisionComponent>(entity);

	// Get the camera transform to offset the rendering of the AABB box.
	const TransformComponent& cameraTransform = m_registry.GetComponentRead<TransformComponent>(m_cameraEntity);

	// Construct a rectangle representing the position dimensions of the box.
	SDL_Rect collisionBox = {
		static_cast<int>(round(transformComponent.x - cameraTransform.x)),
		static_cast<int>(round(transformComponent.y - cameraTransform.y)),
		static_cast<int>(round(collisionComponent.colliderWidth * transformComponent.xScale)),
		static_cast<int>(round(collisionComponent.colliderHeight * transformComponent.yScale))
	};

	// Draw the boxes in red.
	SDL_SetRenderDrawColor(a_renderer, 255, 0, 0, 255);
	SDL_RenderDrawRect(a_renderer, &collisionBox);
}

void TextureRenderSystem::OnKeyUp(const KeyUpEvent& a_keyUpEvent)
{
	// Debug mode is toggled by pressing the 'B' key on the keyboard.
	if (a_keyUpEvent.m_keyCode == SDLK_b)
		m_debugModeEnabled = !m_debugModeEnabled;
}

