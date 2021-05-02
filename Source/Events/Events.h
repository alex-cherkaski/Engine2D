#pragma once
#include "Constants\Constants.h"

struct QuitEvent final
{
};

struct KeyDownEvent final
{
	SDL_Keycode m_keyCode = 0;
};

struct KeyUpEvent final
{
	SDL_Keycode m_keyCode = 0;
};

struct CollisionEvent final
{
	Entity entity1 = INVALID_ENTITY;
	Entity entity2 = INVALID_ENTITY;
};

