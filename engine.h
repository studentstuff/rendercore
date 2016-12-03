#pragma once

#include <SDL.h>

class Engine
{
public:
	Engine(const char* title, int width, int height);
	~Engine();

	bool Run();
	void SwapBuffers();
private:
	SDL_Window* _window = nullptr;
	SDL_GLContext _context = nullptr;
};
