#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

#include <unistd.h>

#include "RenderWindow.hpp"
#include "Canvas.hpp"

#if __EMSCRIPTEN__
#include <emscripten.h>
#endif

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

struct app_ctx
{
    RenderWindow *window;
    Canvas *canvas;
    bool gameRunning = false;
    uint frame;
};

void next_iter(void *arg)
{
    struct app_ctx *ctx = (struct app_ctx *)arg;

    ctx->window->display(*(ctx->canvas));
    ctx->frame++;

    if ((ctx->frame % 100) == 0)
    {
        ctx->canvas->newColor();
    }
}

int main(int argc, char *args[])
{
    if (SDL_Init(SDL_INIT_VIDEO) > 0)
    {
        std::cout << "SDL_Init has failed: " << SDL_GetError() << std::endl;
    }

    RenderWindow window("Game", 800, 600);

    SDL_Rect r = {.x = 0, .y = 0, .w = 800, .h = 600};
    Canvas canvas(r);

    struct app_ctx ctx;
    ctx.canvas = &canvas;
    ctx.window = &window;
    ctx.gameRunning = true;
    ctx.frame = 0;

    SDL_Event event;

    int st = SDL_GetTicks();

    int fc = 0;

#if !__EMSCRIPTEN__
    while (ctx.gameRunning)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE))
            {
                ctx.gameRunning = false;
            }
        }

        next_iter(&ctx);
        wasteTimeMs(20, st);
    }
#else
    emscripten_set_main_loop_arg(next_iter, &ctx, -1, true);
#endif
    window.cleanUp();

    SDL_Quit();

    return 0;
}
