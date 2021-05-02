#include "PCH.h"
#include "TextureManager.h"
#include "Engine.h"
#include "SDL\SDL_render.h"

TextureManager::TextureManager()
	: m_stringHasher()
{
}

TextureManager::~TextureManager()
{
	Shutdown();
}

void TextureManager::Initialize()
{
	LoadTexture("./Assets/Images/Chopper-Spritesheet.png");
	LoadTexture("./Assets/Images/Projectile.png");
	LoadTexture("./Assets/Images/Tank.png");
	LoadTexture("./Assets/Images/DestroyedTank.png");
	LoadTexture("./Assets/Images/Truck.png");
	LoadTexture("./Assets/Images/Airplane-Spritesheet.png");
	LoadTexture("./Assets/Images/Tank.png");
	LoadTexture("./Assets/Images/LandingBase.png");
	LoadTexture("./Assets/Images/Boat.png");
	LoadTexture("./Assets/Images/Carrier.png");
	LoadTexture("./Assets/Images/Jet-Spritesheet.png");
}

void TextureManager::Shutdown()
{
	Clear();
}

TextureId TextureManager::LoadTexture(const char* a_texturePath)
{
	// Get the engine and renderer to construct the texture.
	static const Engine& engine = Engine::GetInstanceRead();
	static SDL_Renderer* renderer = engine.GetEngineRenderer();

	// Hash the path to the file. This is the key in the map.
	const TextureId hash = m_stringHasher(a_texturePath);

	// Load the texture.
	SDL_Texture* texture = IMG_LoadTexture(renderer, a_texturePath);
	if (!texture)
	{
		fprintf(stderr, "Failed to load texture %s.\n", a_texturePath);
		assert(false);
	}

	// Add the texture to our map and return the result.
	m_textureMap[hash] = texture;
	return hash;
}

TextureId TextureManager::GetTextureId(const char* a_texturePath) const
{
	const size_t textureHash = m_stringHasher(a_texturePath);;
	assert(HaveTexture(textureHash));
	return textureHash;
}

bool TextureManager::HaveTexture(TextureId a_textureId) const
{
	return m_textureMap.find(a_textureId) != m_textureMap.end();
}

SDL_Texture* TextureManager::GetTexture(TextureId a_textureId) const
{
	return m_textureMap.at(a_textureId);
}

void TextureManager::RemoveTexture(TextureId a_textureId)
{
	// Free the texture and erase the entry for this texture.
	SDL_Texture* texture = m_textureMap.at(a_textureId);
	SDL_DestroyTexture(texture);
	m_textureMap.erase(a_textureId);
}

int TextureManager::GetTextureWidth(TextureId a_textureId) const
{
	// Query the SDL texture for its width.
	int textureWidth = 0;
	SDL_Texture* texture = m_textureMap.at(a_textureId);
	SDL_QueryTexture(texture, nullptr, nullptr, &textureWidth, nullptr);
	return textureWidth;
}

int TextureManager::GetTextureHeight(TextureId a_textureId) const
{
	// Query the SDL texture for its height.
	int textureHeight = 0;
	SDL_Texture* texture = m_textureMap.at(a_textureId);
	SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &textureHeight);
	return textureHeight;
}

void TextureManager::Clear()
{
	// Erase all textures and clear the map.
	for (const auto& texturePair : m_textureMap)
	{
		SDL_DestroyTexture(texturePair.second);
	}
	m_textureMap.clear();
}

