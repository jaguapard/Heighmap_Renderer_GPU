#include "Game.h"
#include "Graphics.h"
#include <algorithm>
#include <cmath>
#include "errors.h"
#include "utils.h"
#include "C_Input.h"
#include <iostream>
using namespace DirectX;
struct Vertex {
	float x, y;
};

//Note to self: don't use members of sizes != integer multiple of 16. That introduces silent disagreement between CPU and GPU side.
//Yes, the memory is wasted, but whatever. If you really want to, you can pack many smaller values into XMVECTOR.
struct alignas(16) ConstantBuffer
{
	XMMATRIX transformation;
	XMVECTOR time, fieldSize;
	XMVECTOR camPos, lightDir;
};


Game::Game(Graphics& gfx) :gfx(gfx)
{
	this->camAng = XMVectorZero();
	this->camPos = XMVectorSet(0, 100, 0, 0);
	this->lightDir = XMVectorSet(0, -1, 0, 0);
	Microsoft::WRL::ComPtr<ID3DBlob> vsBlob;
	std::wstring vsPath = Graphics::SHADERS_FOLDER + L"BasicVS.cso";
	DX_THROW_ON_FAIL(D3DReadFileToBlob(vsPath.c_str(), &vsBlob), "Read basic VS blob");
	DX_THROW_ON_FAIL(this->gfx.device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &this->basicVS), "Create basic VS");
	this->gfx.deviceContext->VSSetShader(this->basicVS.Get(), nullptr, 0);

	Microsoft::WRL::ComPtr<ID3DBlob> psBlob;
	DX_THROW_ON_FAIL(D3DReadFileToBlob((Graphics::SHADERS_FOLDER + L"BasicPS.cso").c_str(), &psBlob), "Read basic PS blob");
	DX_THROW_ON_FAIL(this->gfx.device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &this->basicPS), "Create basic PS");
	this->gfx.deviceContext->PSSetShader(this->basicPS.Get(), nullptr, 0);

	Microsoft::WRL::ComPtr<ID3D11InputLayout> vsInputLayout;
	const D3D11_INPUT_ELEMENT_DESC vsInputLayoutElemets[] = {
		{"Pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	DX_THROW_ON_FAIL(this->gfx.device->CreateInputLayout(vsInputLayoutElemets, std::size(vsInputLayoutElemets), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &vsInputLayout), "Create input layout for basic VS");
	this->gfx.deviceContext->IASetInputLayout(vsInputLayout.Get());

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	D3D11_RASTERIZER_DESC rdsc = {};
	rdsc.FillMode = D3D11_FILL_SOLID;
	rdsc.CullMode = D3D11_CULL_NONE;
	DX_THROW_ON_FAIL(this->gfx.device->CreateRasterizerState(&rdsc, &rasterizerState), "Create rasterizer state");
	this->gfx.deviceContext->RSSetState(rasterizerState.Get());

	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = this->gfx.w;
	descDepth.Height = this->gfx.h;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DX_THROW_ON_FAIL(this->gfx.device->CreateTexture2D(&descDepth, nullptr, &this->depthStencil), "Create depth stencil texture");

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	DX_THROW_ON_FAIL(this->gfx.device->CreateDepthStencilView(this->depthStencil.Get(), &descDSV, &this->depthStencilView), "Create depth stencil view");

	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.StencilEnable = false;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER; //using reverse depth, greater comparison is needed
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> dsState;
	DX_THROW_ON_FAIL(this->gfx.device->CreateDepthStencilState(&dsDesc, &dsState), "Create depth stencil state");
	this->gfx.deviceContext->OMSetDepthStencilState(dsState.Get(), 0);

	//This is 2D mathematical vertices, i.e x,y. In 3D, the y is put into Z coordinate, since Y is height that will be calculated from a function
	//TODO: can probably generate this on GPU?
	this->fieldSize = 20000;
	int subdivisions = 400;
	std::vector<Vertex> verts;
	for (int stepIndexY = 0; stepIndexY < subdivisions; ++stepIndexY)
	{
		float sy = fieldSize / subdivisions * stepIndexY - fieldSize / 2;
		float sny = fieldSize / subdivisions * (stepIndexY+1) - fieldSize / 2;
		for (int stepIndexX = 0; stepIndexX < subdivisions; ++stepIndexX)
		{
			float sx = fieldSize / subdivisions * stepIndexX - fieldSize / 2;
			float snx = fieldSize / subdivisions * (stepIndexX+1) - fieldSize / 2;
			Vertex v;
			v.x = sx;
			v.y = sy;
			verts.emplace_back(v);
			v.x = snx;
			verts.emplace_back(v);
			v.y = sny;
			verts.emplace_back(v);
			verts.emplace_back(v); //yes, twice. It is shared by 2 triangles in a heightmap block
			v.x = sx;
			verts.emplace_back(v);
			v.y = sy;
			verts.emplace_back(v);
		}
	}
	
	this->vertexCount = verts.size();
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.ByteWidth = verts.size()*sizeof(Vertex);
	vertexBufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA vertexBufferSubresourceData;
	vertexBufferSubresourceData.pSysMem = verts.data();
	DX_THROW_ON_FAIL(this->gfx.device->CreateBuffer(&vertexBufferDesc, &vertexBufferSubresourceData, &this->vertexBuffer), "Create vertex buffer", this->gfx.device.Get());

	UINT vbStrides[] = { sizeof(Vertex) };
	UINT vbOffsets[] = { 0 };
	this->gfx.deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), vbStrides, vbOffsets);
	this->gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ConstantBuffer cb;
	memset(&cb, 0, sizeof(cb));
	D3D11_BUFFER_DESC cbd;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd;
	csd.pSysMem = &cb;
	DX_THROW_ON_FAIL(this->gfx.device->CreateBuffer(&cbd, &csd, &this->constantBuffer), "Create constant buffer");
	this->gfx.deviceContext->VSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());
	this->gfx.deviceContext->PSSetConstantBuffers(0, 1, this->constantBuffer.GetAddressOf());
}

void Game::beginNewFrame()
{
	
}

void Game::handleEvent(SDL_Event& event)
{

}

static std::ostream& operator<<(std::ostream& os, const XMVECTOR& v)
{
	for (int i = 0; i < 4; ++i)
	{
		os << v.vector4_f32[i];
		if (i != 3) os << ", ";
	}
	return os;
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
void Game::update(const std::vector<SDL_Event>& events)
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

	C_Input& inp = C_Input::getInstance();
	if (inp.wasCharPressedOnThisFrame('V')) this->vsyncEnabled ^= 1;

	bool mouseRelativeMode = SDL_GetWindowRelativeMouseMode(this->gfx.window);
	for (auto& event : events)
	{
		if (mouseRelativeMode && event.type == SDL_EVENT_MOUSE_MOTION)
		{
			float angAddX = event.motion.xrel * 1e-3;
			float angAddY = event.motion.yrel * 1e-3;
			this->camAng += XMVectorSet(angAddY, angAddX, 0, 0); //TODO: wrap angles around multiples of PI
			
			//clamp angles. Vertical to range +-PI/2 with small dead zone. Horizontal to 0..2*PI 
			float ang1 = XMVectorGetX(this->camAng);
			float ang2 = XMVectorGetY(this->camAng);
			float lim = XM_PIDIV2 - 0.01;
			ang1 = std::clamp<float>(ang1, -lim, lim);
			ang2 = std::fmodf(ang2, XM_2PI);
			if (ang2 < 0) ang2 += XM_2PI;
			this->camAng = XMVectorSet(ang1, ang2, 0, 0);
		}
	}
	
	if (inp.wasButtonPressedOnThisFrame(SDL_SCANCODE_LCTRL))
	{
		SDL_SetWindowRelativeMouseMode(this->gfx.window, !mouseRelativeMode);
	}

	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(this->camAng);
	XMVECTOR camAdd = XMVectorZero();
	XMVECTOR right = XMVectorSet(rotation.m[0][0], rotation.m[0][1], rotation.m[0][2], 0.f);
	XMVECTOR forward = XMVectorSet(rotation.m[2][0], rotation.m[2][1], rotation.m[2][2], 0.f);
	if (inp.isButtonHeld(SDL_SCANCODE_W)) camAdd += forward;
	if (inp.isButtonHeld(SDL_SCANCODE_S)) camAdd -= forward;
	if (inp.isButtonHeld(SDL_SCANCODE_A)) camAdd -= right;
	if (inp.isButtonHeld(SDL_SCANCODE_D)) camAdd += right;
	if (inp.isButtonHeld(SDL_SCANCODE_Z)) camAdd -= XMVectorSet(0.f, 1.f, 0.f, 0.f);
	if (inp.isButtonHeld(SDL_SCANCODE_X)) camAdd += XMVectorSet(0.f, 1.f, 0.f, 0.f);
	if (XMVector3NotEqual(camAdd, XMVectorZero()))
	{
		this->camPos += XMVector3Normalize(camAdd) * this->flySpeed * clampedDt;
		std::cout << this->camPos << "\n";
	}
	
	XMMATRIX translation = XMMatrixTranslation(-this->camPos.vector4_f32[0], -this->camPos.vector4_f32[1], -this->camPos.vector4_f32[2]);
	XMMATRIX view = translation * XMMatrixTranspose(rotation);
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, float(this->gfx.w) / float(this->gfx.h), 100000.f, 0.1f);
	XMMATRIX transform = view * projection;

	D3D11_MAPPED_SUBRESOURCE mappedCb = {};
	DX_THROW_ON_FAIL(this->gfx.deviceContext->Map(this->constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedCb), "Constant buffer map");
	ConstantBuffer* cb = (ConstantBuffer*)mappedCb.pData;
	cb->transformation = XMMatrixTranspose(transform);
	cb->time = XMVectorSet(this->gameTime,0,0,0);
	cb->fieldSize = XMVectorSet(this->fieldSize, 0, 0, 0);
	cb->camPos = this->camPos;
	cb->lightDir = XMVector3Normalize(this->lightDir);
	this->gfx.deviceContext->Unmap(this->constantBuffer.Get(), 0);

	this->gfx.deviceContext->OMSetRenderTargets(1, this->gfx.mainRenderTargetView.GetAddressOf(), this->depthStencilView.Get());
	float r = 0;
	float g = 0;
	float b = 0;
	float clear[4] = { r,g,b,1 };
	this->gfx.deviceContext->ClearRenderTargetView(this->gfx.mainRenderTargetView.Get(), clear);
	this->gfx.deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 0.f, 0);

	this->gfx.deviceContext->Draw(this->vertexCount, 0);
	DX_THROW_ON_FAIL(this->gfx.swapChain->Present(this->vsyncEnabled ? 1 : 0, 0), "Swapchain present", this->gfx.device.Get());
}
