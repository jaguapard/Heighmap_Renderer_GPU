#pragma once
#include <SDL3/SDL.h>
class Graphics;
class Game
{
public:
	Game(Graphics& gfx);
	void update();
private:
	Graphics& gfx;
	uint64_t prevTicks = 0, startTicks = 0;
	double gameTime = 0;
	double globalTime = 0;
};