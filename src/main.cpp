#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

#include <unistd.h>

#include "RenderWindow.hpp"
#include "Canvas.hpp"

void wasteTimeMs(int p_time, int &p_dtime)
{
    int dt = SDL_GetTicks() - p_dtime;

    std::cout << "Render Time : " << dt << "ms" << std::endl;

    if ((p_time - dt) > 0)
    {
        usleep((p_time - dt) * 1000);
    }

    p_dtime = SDL_GetTicks();
}

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        std::cout << "SDL_Init has failed: " << SDL_GetError() << std::endl;
    }

    if (!IMG_Init(IMG_INIT_PNG))
    {
        std::cout << "IMG_Init has failed: " << SDL_GetError() << std::endl;
    }

// 3440x1440

    RenderWindow window("Game", 3440, 1440);

    // SDL_Rect r = {.x = 0, .y = 0, .w = 1280, .h = 720};
    SDL_Rect r = {.x = 0, .y = 0, .w = 3440, .h = 1440};    
    Canvas canvas(r);

    bool gameRunning = true;

    SDL_Event event;

    int st = SDL_GetTicks();

    int fc = 0;

    while (gameRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT)
            {
                gameRunning = false;
            }
        }

        window.display(canvas);
        fc++;
        wasteTimeMs(20, st);

        if ((fc % 100) == 0) {
            canvas.newColor();
        }
    }

    window.cleanUp();

    IMG_Quit();
    SDL_Quit();

    return 0;
}
