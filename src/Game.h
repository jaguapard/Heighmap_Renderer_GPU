#pragma once
#include <SDL3/SDL.h>
#include <d3d11.h>
#include <wrl/client.h>
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

	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
};