#include "CubemapTexture.h"
#include "smart.h"

CubemapTexture::CubemapTexture(std::array<std::string, 6> paths, Graphics& gfx)
{
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //TODO: sure about it?
	texDesc.CPUAccessFlags = 0;
	texDesc.MipLevels = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	//texDesc.Width = 
	D3D11_SUBRESOURCE_DATA data[6];
	Smart_Surface surfaces[6];
	for (int i = 0; i < 6; ++i)
	{
		std::string baseErrMsg = "Error while loading cubemap texture " + paths[i] + ": ";
		auto s = Smart_Surface(IMG_Load(paths[i].c_str()));
		if (!s) RAISE_ERROR(baseErrMsg + "IMG_Load");

		surfaces[i] = Smart_Surface(SDL_ConvertSurface(s.get(), SDL_PIXELFORMAT_RGBA8888)); //is this the same as above?
		if (!surfaces[i]) RAISE_ERROR(baseErrMsg + "SDL_ConverSurface");
		if (i == 0)
		{
			texDesc.Width = surfaces[i]->w;
			texDesc.Height = surfaces[i]->h;
		}
		else if(surfaces[i]->w != texDesc.Width || surfaces[i]->h != texDesc.Height)
		{
			std::ostringstream oss;
			oss << "Mismatched cubemap texture sizes: initial surface " << paths[0] << " was " << texDesc.Width << "x" << texDesc.Height << ", " << paths[i] << " is " << surfaces[i]->w << "x" << surfaces[i]->h;
			RAISE_ERROR(oss.str());
		}
		data[i].pSysMem = surfaces[i]->pixels;
		data[i].SysMemPitch = surfaces[i]->pitch;
		data[i].SysMemSlicePitch = 0;
	}
	DX_THROW_ON_FAIL(gfx.device->CreateTexture2D(&texDesc, data, &this->texture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	DX_THROW_ON_FAIL(gfx.device->CreateShaderResourceView(this->texture.Get(), &srvDesc, &this->srv));

}
