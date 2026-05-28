#include <vector>
#include <string>
#include <sstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_system.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Graphics.h"
#include "Game.h"
#include "C_Input.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "SDL3.lib")
#pragma comment(lib, "SDL3_image.lib")
#pragma comment(lib, "SDL3_ttf.lib")

int main(int argc, char* argv[])
{
	try
	{
		std::vector<SDL_Event> sdlEvents;
		Graphics gfx(2560, 1440);
		C_Input& input = C_Input::getInstance();
		Game game(gfx);
		while (true)
		{
			SDL_Event ev;
			input.beginNewFrame();
			game.beginNewFrame();
			while (SDL_PollEvent(&ev))
			{
				input.handleEvent(ev);
				//game.handleEvent(ev);
				sdlEvents.emplace_back(ev);
				if (ev.type == SDL_EVENT_QUIT)
				{
					SDL_Quit();
					return 0;
				}
			}
			game.update(sdlEvents);
			sdlEvents.clear();
			game.draw();
			game.present();
		}
	}
	catch (const std::exception& e)
	{
		std::stringstream ss;
		char strerror_out[8192] = { 0 };
		strerror_s(strerror_out, sizeof(strerror_out), errno);
		ss << e.what() << "\nSDL error: " << SDL_GetError() << "\n" << "strerror: " << strerror_out << "\n";
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", ss.str().c_str(), nullptr);
	}
	return 0;
}