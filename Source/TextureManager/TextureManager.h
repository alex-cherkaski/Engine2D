#pragma once
#include "PCH.h"
#include "Macros.h"

using TextureId = size_t;

class TextureManager final
{
public:
	SINGLETON(TextureManager);

	TextureManager();
	~TextureManager();

	void Initialize();
	void Shutdown();

	TextureId LoadTexture(const char* a_texturePath);
	TextureId GetTextureId(const char* a_texturePath) const;

	bool HaveTexture(TextureId a_textureId) const;
	SDL_Texture* GetTexture(TextureId a_textureId) const;
	void RemoveTexture(TextureId a_textureId);

	int GetTextureWidth(TextureId a_textureId) const;
	int GetTextureHeight(TextureId a_textureId) const;

	void Clear();

private:
	std::unordered_map<TextureId, SDL_Texture*> m_textureMap;
	const std::hash<const char*> m_stringHasher;
};

