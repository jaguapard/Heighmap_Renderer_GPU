#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_ttf.h>
#include <SDL3/SDL_system.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include "errors.h"
#include "DirectXMath.h"
class Graphics;
struct ShaderCreationDesc;

template<typename T>
struct Shader
{
	Microsoft::WRL::ComPtr<T> shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3DBlob> bytecode;

	Shader() = default;
	Shader(Graphics& gfx, const ShaderCreationDesc& desc);
};

struct ShaderCreationDesc
{
	std::string path;
	//Input layout elements. If empty, this argument will be ignored, and no layout will be created.
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout = {};
	//If set to false, then path provided will be appended to default shader path. If not, will pass the path directly to Direct3D shader creation
	bool pathIsAbsolute = false;
	ID3D11ClassLinkage* classLinkange = nullptr; //passed directly to D3D11
};

struct Vertex3D {
	float x, y, z, u, v;
};

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
	static const std::string SHADERS_FOLDER;

	//Generates a rectangle cuboid centered at target point with side lengths size, and returns it in a vector
	//All vertices are used exactly in one triangle, resulting in 36 total vertices.
	//W component of input vectors is ignored. Setting all sizes to the same value will generate a proper cube
	//Returned shape's vertex winding is undefined.
	static std::vector<Vertex3D> generateRectangularCuboidNoDedup(DirectX::XMVECTOR center = DirectX::XMVectorZero(), DirectX::XMVECTOR size = DirectX::XMVectorSet(1, 1, 1, 0));
};

static std::wstring ascii_string_to_wstring(const std::string s)
{
	std::wstring ws;
	for (auto& c : s)
		if (c >= 0 && c <= 127)
			ws.push_back(c);
		else RAISE_ERROR("Naive extension of non-ASCII string to wstring requested");
	return ws;
}

template<typename T>
inline Shader<T>::Shader(Graphics& gfx, const ShaderCreationDesc& desc)
{
	if (desc.path.empty()) RAISE_ERROR("Attempted to create shader with empty path.");
	std::string asciiPath;
	if (desc.pathIsAbsolute) asciiPath = desc.path;
	else asciiPath = gfx.SHADERS_FOLDER + desc.path;
	std::string baseMessage = "While creating shader from file " + asciiPath + ": ";

	std::wstring fullPath = ascii_string_to_wstring(asciiPath);
	DX_THROW_ON_FAIL(D3DReadFileToBlob(fullPath.c_str(), &this->bytecode), baseMessage + "D3DReadFileToBlob");
	if (!desc.inputLayout.empty())
	{
		DX_THROW_ON_FAIL(gfx.device->CreateInputLayout(desc.inputLayout.data(), desc.inputLayout.size(), this->bytecode->GetBufferPointer(), this->bytecode->GetBufferSize(), &this->inputLayout), baseMessage + "CreateInputLayout");
	}

	if constexpr (std::is_same_v<T, ID3D11VertexShader>)
	{
		DX_THROW_ON_FAIL(gfx.device->CreateVertexShader(this->bytecode->GetBufferPointer(), this->bytecode->GetBufferSize(), desc.classLinkange, &this->shader), baseMessage + "CreateVertexShader");
	}
	else if constexpr (std::is_same_v<T, ID3D11PixelShader>)
	{
		DX_THROW_ON_FAIL(gfx.device->CreatePixelShader(this->bytecode->GetBufferPointer(), this->bytecode->GetBufferSize(), desc.classLinkange, &this->shader), baseMessage + "CreatePixelShader");
	}
	else static_assert(false, "Unsupported type for Shader");
}
