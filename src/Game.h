#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "CubemapTexture.h"

class Graphics;
class Game
{
public:
	Game(Graphics& gfx);
	void beginNewFrame();
	void handleEvent(SDL_Event& event);
	void update(const std::vector<SDL_Event>& events);
	void draw();
	void present();
private:
	Graphics& gfx;
	uint64_t prevTicks = 0, startTicks = 0;
	double gameTime = 0;
	double globalTime = 0;

	bool vsyncEnabled = true;
	float flySpeed = 2500;
	float fieldSize;
	UINT vertexCount, skyCubeVertexCount;
	DirectX::XMVECTOR camPos, camAng, lightDir;
	Shader<ID3D11VertexShader> mainVS, skyboxVS;
	Shader<ID3D11PixelShader> mainPS, skyboxPS;

	Microsoft::WRL::ComPtr<ID3D11Buffer> heightmapVB, mainConstantBuffer, skyboxVB;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

	CubemapTexture skyboxCubemap;
};