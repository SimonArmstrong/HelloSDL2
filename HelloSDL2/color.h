#pragma once
#include <SDL.h>

class Color {
public:
	union {
		struct {
			Uint16 r, g, b, a;
		};
		Uint16 rgba[4];
	};

	operator Uint16*() {
		return rgba;
	}
};

void SetColorRender(SDL_Renderer* r, Uint8* color) {
	SDL_SetRenderDrawColor(r, color[0], color[1], color[2], color[3]);
}