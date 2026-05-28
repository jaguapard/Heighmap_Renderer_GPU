#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include "CubemapTexture.h"

class Graphics;
//Note to self: don't use members of sizes != integer multiple of 16. That introduces silent disagreement between CPU and GPU side.
//Yes, the memory is wasted, but whatever. If you really want to, you can pack many smaller values into XMVECTOR.
struct alignas(16) ConstantBuffer
{
	DirectX::XMMATRIX view, projection, viewProjection;
	DirectX::XMVECTOR time, fieldSize;
	DirectX::XMVECTOR camPos, lightDir;
};

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
	ConstantBuffer mainCB_CPU;
	DirectX::XMVECTOR camPos, camAng, lightDir;
	Shader<ID3D11VertexShader> mainVS, skyboxVS;
	Shader<ID3D11PixelShader> mainPS, skyboxPS;

	Microsoft::WRL::ComPtr<ID3D11Buffer> heightmapVB, mainConstantBuffer, skyboxVB;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> skyboxSamplerState;
	CubemapTexture skyboxCubemap;
};