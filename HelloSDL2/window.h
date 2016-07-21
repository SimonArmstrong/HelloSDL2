#pragma once

#include <SDL.h>
#include <stdio.h>
#include <Matrix2.h>
#include <SDL_image.h>
#include <glew.h>

#define DEBUG_FRAME_STEPPER 1

class Screen {
private:
public:
	void clear();
	void present();
	SDL_Renderer* renderer;
};

class Window {
private:
	int width, height;
	const char* title;
	SDL_Surface* surface;
	SDL_Window * window;

	Uint32 style;

public:
	Window();
	Window(const int& width, const int& height, const char* title);
	~Window();

	bool running;
	Screen* screen;

	void Draw(SDL_Renderer* renderer, SDL_Event* e);

	bool init(Uint32 style);	//Starts up and creates SDL window
	void update();				//Updates the screen
	void close();

	void create(Uint32 style) {
		this->init(style);
		this->update();
	}
};