#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>

class Graphics;
class Game
{
public:
	Game(Graphics& gfx);
	void beginNewFrame();
	void handleEvent(SDL_Event& event);
	void update(const std::vector<SDL_Event>& events);
private:
	Graphics& gfx;
	uint64_t prevTicks = 0, startTicks = 0;
	double gameTime = 0;
	double globalTime = 0;

	bool vsyncEnabled = true;
	float flySpeed = 2500;
	UINT vertexCount;
	DirectX::XMVECTOR camPos, camAng;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> basicVS;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> basicPS;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
};