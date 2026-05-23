#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_system.h>
#include <d3d11.h>
#include <d3dcompiler.h>
class Graphics
{
public:
	Graphics(uint32_t w, uint32_t h);
private:
	SDL_Window* window;
	uint32_t w, h;
};