#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_system.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
class Graphics
{
public:
	Graphics(uint32_t w, uint32_t h);
	SDL_Window* window;
	uint32_t w, h;

	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mainRenderTargetView;
	//Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRenderTargetView;
};