#include "window.h"

Uint32 rmask, gmask, bmask, amask;

void Screen::clear() {	
	SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
	SDL_RenderClear(this->renderer);
}

void Screen::present() {
	SDL_RenderPresent(this->renderer);
}

Window::Window(const int & width, const int & height, const char * title)
{
	this->width = width;
	this->height = height;
	this->title = title;
	this->screen = new Screen();
}

Window::~Window()
{
	if (this->window != nullptr) {
		if(this->surface != nullptr) SDL_FreeSurface(this->surface);
		this->surface = NULL;
		delete[] this->screen;
		SDL_DestroyWindow(this->window);
		this->window = nullptr;

	}
}

Window::Window()
{
	this->width = 640;
	this->height = 480;
	this->title = "empty";
	this->screen = new Screen();
}


bool Window::init(Uint32 style)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("SDL Failed to initialize. Error: %s\n", SDL_GetError());
		return false;
	}

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	// Settings
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	///glEnable(GL_MULTISAMPLE);

	this->style = style;
	//Create a window
	this->window = SDL_CreateWindow(this->title, 50, 40, this->width, this->height, this->style);

	//Check if the window created
	if (this->window == nullptr) return false;

	//Create a main surface
	this->surface = SDL_CreateRGBSurface(0, this->width, this->height, 32, rmask, gmask, bmask, amask);

	//Create main screen renderer
	this->screen[0].renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

	this->running = true;

	return true;
}

void Window::update()
{
	SDL_Event e;

	while (running) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				running = false;
			}
		}
		this->screen[0].clear();
		this->Draw(this->screen[0].renderer, &e);			//Drawing happens here
		this->screen[0].present();
	}
}

void Window::close()
{
	SDL_FreeSurface(this->surface);
	if (this->window != nullptr) {
		SDL_DestroyRenderer(this->screen->renderer);
		this->surface = NULL;
		delete[] this->screen;
		SDL_DestroyWindow(this->window);
		this->window = nullptr;
	}
}