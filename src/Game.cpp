#include "Game.h"
#include "Graphics.h"
#include <algorithm>
#include <cmath>

Game::Game(Graphics& gfx) :gfx(gfx)
{
}

void Game::update()
{
	uint64_t currTicks = SDL_GetTicksNS();
	if (!this->prevTicks) {
		this->prevTicks = this->startTicks = currTicks;
		return;
	}

	double realDt = (currTicks - prevTicks) / 1e9;
	double clampedDt = std::clamp(realDt, 0.0, 0.1);
	this->globalTime = (currTicks - startTicks) / 1e9;
	this->gameTime += clampedDt;
	this->prevTicks = currTicks;

	this->gfx.deviceContext->OMSetRenderTargets(1, this->gfx.mainRenderTargetView.GetAddressOf(), nullptr);
	float r = std::fmod(this->gameTime, 10.0) / 10.0;
	float g = std::fmod(this->gameTime, 20.0) / 20.0;
	float b = std::fmod(this->gameTime, 30.0) / 30.0;
	float clear[4] = { r,g,b,1 };
	this->gfx.deviceContext->ClearRenderTargetView(this->gfx.mainRenderTargetView.Get(), clear);

	this->gfx.swapChain->Present(1, 0); //TODO: disable VSYNC later
}
