#pragma once
#include "libs.h"
#include "Graphics.h"
#include <array>

class CubemapTexture
{
public:
	CubemapTexture() = default;
	CubemapTexture(std::array<std::string, 6> paths, Graphics& gfx);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};