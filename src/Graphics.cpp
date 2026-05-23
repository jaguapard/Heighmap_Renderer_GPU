#include "Graphics.h"
#include "errors.h"

Graphics::Graphics(uint32_t w, uint32_t h)
{
    this->w = w;
    this->h = h;
    window = SDL_CreateWindow("SDL3 + D3D11 Pixel Display", w, h, 0);
    if (!window) RAISE_ERROR("SDL_CreateWindow failed");

    SDL_PropertiesID props = SDL_GetWindowProperties(window);
    if (!props) RAISE_ERROR("SDL_GetWindowProperties returned NULL");

    void* rawHwnd = SDL_GetPointerProperty(props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
    HWND hwnd = reinterpret_cast<HWND>(rawHwnd);
    if (!hwnd) RAISE_ERROR("Failed to obtain HWND from SDL3 window properties");

    TTF_Init();
}
