#include "Game.h"
#include "Graphics.h"
#include <algorithm>
#include <cmath>
#include "errors.h"
#include "utils.h"

Game::Game(Graphics& gfx) :gfx(gfx)
{
}

/*
static Microsoft::WRL::ComPtr<ID3DBlob> CompileShaderFromFile(std::wstring path)
{
	Microsoft::WRL::ComPtr<ID3DBlob> compiled;
	Microsoft::WRL::ComPtr<ID3DBlob> read;
	DX_RAISE_ON_FAIL(D3DReadFileToBlob(path.c_str(), &read), "Read shader code for comiling");

	HRESULT hr = D3DCompileFromFile(path.c_str(), nullptr, nullptr, "main",  )

	/*
	Microsoft::WRL::ComPtr<ID3DBlob> blob, errors;
    HRESULT hr = D3DCompile(source, strlen(source), nullptr, nullptr, nullptr,
        entry, target, 0, 0, &blob, &errors);
    if (FAILED(hr)) {
        std::string errMsg;
        if (errors) {
            errMsg = static_cast<const char*>(errors->GetBufferPointer());
        }
        else {
            errMsg = "Unknown compilation error";
        }
        RAISE_ERROR(errMsg);
    }
    return blob;
	}
	*/
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

	r = g = b = 0;
	float clear[4] = { r,g,b,1 };
	this->gfx.deviceContext->ClearRenderTargetView(this->gfx.mainRenderTargetView.Get(), clear);


	struct Vertex {
		float x, y, z;
	};
	Vertex verts[] = {
		{0.f,0.5f,1.f},
		{0.5f,-0.5f,1.f},
		{-0.5f,-0.5f,1.f},
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.ByteWidth = sizeof(verts);
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData;
	vertexBufferSubresourceData.pSysMem = verts;



	DX_THROW_ON_FAIL(this->gfx.device->CreateBuffer(&vertexBufferDesc, &vertexBufferSubresourceData, &vertexBuffer), "Create vertex buffer", this->gfx.device.Get());

	UINT vbStrides[] = { sizeof(Vertex) };
	UINT vbOffsets[] = { 0 };
	this->gfx.deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vbStrides, vbOffsets);
	this->gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	std::wstring vsPath = Graphics::SHADERS_FOLDER + L"BasicVS.cso";
	DX_THROW_ON_FAIL(D3DReadFileToBlob(vsPath.c_str(), &vsBlob), "Read basic VS blob");
	DX_THROW_ON_FAIL(this->gfx.device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShader), "Create basic VS");
	this->gfx.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	DX_THROW_ON_FAIL(D3DReadFileToBlob((Graphics::SHADERS_FOLDER + L"BasicPS.cso").c_str(), &psBlob), "Read basic PS blob");
	DX_THROW_ON_FAIL(this->gfx.device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShader), "Create basic PS");
	this->gfx.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	this->gfx.deviceContext->OMSetRenderTargets(1, this->gfx.mainRenderTargetView.GetAddressOf(), nullptr); //TODO: add ZBuffer here!

	this->gfx.deviceContext->Draw(3, 0);
	DX_THROW_ON_FAIL(this->gfx.swapChain->Present(1, 0), "Swapchain present", this->gfx.device.Get()); //TODO: disable VSYNC later
}
