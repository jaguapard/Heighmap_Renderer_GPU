#include <vector>
#include <string>
#include <sstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "SDL3.lib")
#pragma comment(lib, "SDL3_image.lib")
#pragma comment(lib, "SDL3_ttf.lib")

int main(int argc, char* argv[])
{
	Graphics gfx(2560, 1440);
	return 0;
}