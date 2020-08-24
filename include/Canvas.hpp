#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

struct CircleDef
{
    uint x;
    uint y;
    uint r;

    int dir_x;
    int dir_y;

    CircleDef();
    CircleDef(uint x, uint y, uint r, int dir_x, int dir_y);
};

struct Color
{
    uint r, g, b;

    Color();
    Color(uint r, uint g, uint b);
    void operator=(Color c);
};

class Canvas
{
public:
    Canvas(SDL_Rect p_rect);
    ~Canvas();

    void draw(SDL_Renderer *renderer);

    void newColor();

private:
    float **grid;
    int resolution;

    Color start;
    Color color;
    Color end;

    float step;

    int circleCount;
    CircleDef *circles;

    void applyBounds(CircleDef &circle);
    float calcContribution(int x, int y);

    void squareMarch(SDL_Renderer *renderer);
    void populateGrid();

    void lerpColor();

    SDL_Rect canvasRect;
};