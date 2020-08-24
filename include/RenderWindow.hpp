#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Canvas.hpp"

class RenderWindow
{
public:
    RenderWindow(const char* p_title, int p_h, int p_w);

    SDL_Texture* loadTexture(const char* p_filePath);

    void clear();
    void render(SDL_Texture* p_tex);
    void display();
    void display(Canvas &p_canvas);

    void cleanUp();
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};