#pragma once
#include "ECS\System.h"
#include "ECS\Registry.h"
#include "Engine.h"
#include "TextureManager\TextureManager.h"
#include "Constants\Constants.h"

class TextureRenderSystem final : public ISystem
{
public:
	TextureRenderSystem(Registry& a_registry);
	~TextureRenderSystem() = default;

	// Inherited via ISystem.
	void Initialize() override;
	void Update(float a_deltaTime) override;
	void Render() override;

private:
	void RenderTile(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager);
	void RenderSprite(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager);
	void RenderTexture(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager);

	void RenderHealthBar(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer, const TextureManager& a_textureManager);
	void RenderColliders(const Entity entity, Registry& a_registry, SDL_Renderer* a_renderer);

	void OnKeyUp(const KeyUpEvent& a_keyUpEvent);

private:
	Entity m_cameraEntity = INVALID_ENTITY;
	bool m_debugModeEnabled = false;
};

