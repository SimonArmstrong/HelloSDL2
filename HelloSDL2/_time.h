#pragma once
#include <SDL.h>
#include <SDL_timer.h>

namespace Time {
	Uint64 now = SDL_GetTicks();
	Uint64 last = 0;

	float deltaTime = 0.f;
}