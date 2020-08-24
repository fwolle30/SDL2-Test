#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <iostream>
#include <algorithm>
#include <cstdio>

#include "Canvas.hpp"

CircleDef::CircleDef() : x(0), y(0), r(0) {}
CircleDef::CircleDef(uint x, uint y, uint r, int dir_x, int dir_y)
    : x(x), y(y), r(r), dir_x(dir_x), dir_y(dir_y) {}

Color::Color() : r(0), g(0), b(0){};
Color::Color(uint r, uint g, uint b)
    : r(r), g(g), b(b){};

void Color::operator=(Color c)
{
    r = c.r;
    g = c.g;
    b = c.b;
}

Canvas::Canvas(SDL_Rect p_rect)
    : canvasRect(p_rect), resolution(10), grid(nullptr), circles(nullptr), circleCount(30), step(0.0)
{
    int size_x = canvasRect.w;
    int size_y = canvasRect.h;

    srand((unsigned int)time(NULL));

    start = Color(rand() % 255, rand() % 255, rand() % 255);
    color = start;
    end = Color(rand() % 255, rand() % 255, rand() % 255);

    circles = new CircleDef[circleCount];

    for (int i = 0; i < circleCount; i++)
    {
        int x = (rand() % (size_x - 100)) + 100;
        int y = (rand() % (size_y - 100)) + 100;
        int r = (((rand() % 100) + 100) / 2) * 2;

        int dx = 0, dy = 0;
        int d = ((rand() % 8)) / 2;

        switch (d)
        {
        case 0:
            dx = -2;
            dy = -2;
            break;
        case 1:
            dx = 2;
            dy = -2;
            break;
        case 2:
            dx = 2;
            dy = 2;
            break;
        case 3:
            dx = -2;
            dy = 2;
            break;
        }

        dx += ((rand() % 8)) / 4;
        dy += ((rand() % 8)) / 4;

        circles[i] = CircleDef(x, y, r, dx, dy);
    }

    populateGrid();
}

Canvas::~Canvas()
{
    int size_y = canvasRect.h / resolution + 1;

    if (grid != nullptr)
    {
        for (int y = 0; y < size_y; y++)
        {
            delete[] grid[y];
        }

        delete[] grid;
    }

    delete[] circles;
}

void Canvas::newColor()
{
    start = end;
    color = start;
    end = Color(rand() % 255, rand() % 255, rand() % 255);

    step = 0;
}

void Canvas::lerpColor()
{
    color.r = (1 - step) * start.r + step * end.r;
    color.g = (1 - step) * start.g + step * end.g;
    color.b = (1 - step) * start.b + step * end.b;

    step += 0.01;
}

void Canvas::squareMarch(SDL_Renderer *renderer)
{
    int size_x = canvasRect.w / resolution + 1;
    int size_y = canvasRect.h / resolution + 1;

    float blobThreshold = 2.4;

    for (int y = 0; y < size_y; y++)
    {
        for (int x = 0; x < size_x; x++)
        {
            int px = x * resolution;
            int py = y * resolution;

            int qx, qy;

            if (y > 0 && x > 0)
            {
                float a = std::abs(grid[y - 1][x - 1] - blobThreshold);
                float b = std::abs(grid[y - 1][x] - blobThreshold);
                float c = std::abs(grid[y][x - 1] - blobThreshold);
                float d = std::abs(grid[y][x] - blobThreshold);

                int p0 = (a >= 1.0) ? 1 : 0;
                int p1 = (b >= 1.0) ? 1 : 0;
                int p2 = (c >= 1.0) ? 1 : 0;
                int p3 = (d >= 1.0) ? 1 : 0;

                uint8_t iso = p3 << 3 | p2 << 2 | p1 << 1 | p0;

                int ax = (x - 1) * resolution;
                int ay = (y - 1) * resolution;

                int bx = x * resolution;
                int by = (y - 1) * resolution;

                int cx = (x - 1) * resolution;
                int cy = (y)*resolution;

                int dx = (x)*resolution;
                int dy = (y)*resolution;

                float cp = 0.5 * a + 0.5 * b;
                float cq = 0.5 * c + 0.5 * d;
                float cm = 0.5 * cp + 0.5 * cq;

                float list[4] = {a, b, c, d};
                float minV = *std::min_element(list, list + 4);
                float maxV = *std::max_element(list, list + 4);

                if (minV > 0)
                {
                    maxV -= minV;
                    cm -= minV;
                }

                float cv = std::min(cm / maxV, (float)1.0);
                float alpha = std::max(1.0 - cv, 0.0);

                // alpha = 1;

                // int alpha = 0xff;

                switch (iso)
                {
                case 1:
                    px = bx + (ax - bx) * (1 - b) / (a - b);
                    py = ay;

                    qx = ax;
                    qy = cy + (ay - cy) * (1 - c) / (a - c);

                    filledTrigonRGBA(renderer, px, py, qx, qy, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);

                    break;
                case 2:
                    px = ax + (bx - ax) * (1 - a) / (b - a);
                    py = by;

                    qx = bx;
                    qy = dy + (by - dy) * (1 - d) / (b - d);

                    filledTrigonRGBA(renderer, px, py, qx, qy, bx, by, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);

                    break;
                case 3:
                    px = ax;
                    py = cy + (ay - cy) * (1 - c) / (a - c);

                    qx = bx;
                    qy = dy + (by - dy) * (1 - d) / (b - d);

                    filledTrigonRGBA(renderer, px, py, ax, ay, bx, by, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, px, py, qx, qy, bx, by, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);

                    break;
                case 4:
                    px = ax;
                    py = cy + (ay - cy) * (1 - c) / (a - c);

                    qx = dx + (cx - dx) * (1 - d) / (c - d);
                    qy = cy;

                    filledTrigonRGBA(renderer, px, py, qx, qy, cx, cy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 5:
                    px = bx + (ax - bx) * (1 - b) / (a - b);
                    py = ay;

                    qx = dx + (cx - dx) * (1 - d) / (c - d);
                    qy = cy;

                    filledTrigonRGBA(renderer, ax, ay, px, py, qx, qy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, qx, qy, cx, cy, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 6:
                    px = ax + (bx - ax) * (1 - a) / (b - a);
                    py = by;

                    qx = bx;
                    qy = dy + (by - dy) * (1 - d) / (b - d);

                    filledTrigonRGBA(renderer, px, py, qx, qy, bx, by, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);

                    px = cx;
                    py = ay + (cy - ay) * (1 - a) / (c - a);

                    qx = dx + (cx - dx) * (1 - d) / (c - d);
                    qy = cy;

                    filledTrigonRGBA(renderer, px, py, qx, qy, cx, cy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);

                    break;
                case 7:
                    px = bx;
                    py = dy + (by - dy) * (1 - d) / (b - d);

                    qx = dx + (cx - dx) * (1 - d) / (c - d);
                    qy = cy;

                    filledTrigonRGBA(renderer, px, py, bx, by, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, qx, qy, cx, cy, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, qx, qy, px, py, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 8:
                    px = dx;
                    py = by + (dy - by) * (1 - b) / (d - b);

                    qx = cx + (dx - cx) * (1 - c) / (d - c);
                    qy = dy;

                    filledTrigonRGBA(renderer, px, py, qx, qy, dx, dy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 9:
                    px = bx + (ax - bx) * (1 - b) / (a - b);
                    py = ay;

                    qx = ax;
                    qy = cy + (ay - cy) * (1 - c) / (a - c);

                    filledTrigonRGBA(renderer, px, py, qx, qy, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);

                    px = dx;
                    py = by + (dy - by) * (1 - b) / (d - b);

                    qx = cx + (dx - cx) * (1 - c) / (d - c);
                    qy = dy;

                    filledTrigonRGBA(renderer, px, py, qx, qy, dx, dy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 10:
                    px = ax + (bx - ax) * (1 - a) / (b - a);
                    py = by;

                    qx = cx + (dx - cx) * (1 - c) / (d - c);
                    qy = dy;

                    filledTrigonRGBA(renderer, px, py, bx, by, qx, qy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, bx, by, dx, dy, qx, qy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 11:
                    px = ax;
                    py = cy + (ay - cy) * (1 - c) / (a - c);

                    qx = cx + (dx - cx) * (1 - c) / (d - c);
                    qy = dy;

                    filledTrigonRGBA(renderer, px, py, ax, ay, bx, by, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, bx, by, dx, dy, px, py, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, dx, dy, qx, qy, px, py, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 12:
                    px = cx;
                    py = ay + (cy - ay) * (1 - a) / (c - a);

                    qx = dx;
                    qy = by + (dy - by) * (1 - b) / (d - b);

                    filledTrigonRGBA(renderer, cx, cy, px, py, qx, qy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, qx, qy, dx, dy, cx, cy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 13:
                    px = bx + (ax - bx) * (1 - b) / (a - b);
                    py = ay;

                    qx = dx;
                    qy = by + (dy - by) * (1 - b) / (d - b);

                    filledTrigonRGBA(renderer, ax, ay, px, py, qx, qy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, qx, qy, dx, dy, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, dx, dy, cx, cy, ax, ay, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 14:
                    px = ax + (bx - ax) * (1 - a) / (b - a);
                    py = by;

                    qx = cx;
                    qy = ay + (cy - ay) * (1 - a) / (c - a);

                    filledTrigonRGBA(renderer, px, py, bx, by, dx, dy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, px, py, dx, dy, cx, cy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    filledTrigonRGBA(renderer, px, py, cx, cy, qx, qy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);

                    lineRGBA(renderer, px, py, qx, qy, color.r, color.g, color.b, 0xff);
                    break;
                case 15:

                    boxRGBA(renderer, ax, ay, dx, dy, color.r * alpha, color.g * alpha, color.b * alpha, 0xff);
                    break;
                }
            }
        }
    }
}

void Canvas::populateGrid()
{
    int size_x = canvasRect.w / resolution + 1;
    int size_y = canvasRect.h / resolution + 1;

    if (grid == nullptr)
    {
        grid = new float *[size_y];
        std::fill(grid, grid + size_y, nullptr);
    }

    for (int y = 0; y < size_y; y++)
    {
        if (grid[y] == nullptr)
        {
            grid[y] = new float[size_x];
        }

        for (int x = 0; x < size_x; x++)
        {
            grid[y][x] = calcContribution(x * resolution, y * resolution);
        }
    }
}

float Canvas::calcContribution(int x, int y)
{
    float contrib = 0;

    for (int i = 0; i < circleCount; i++)
    {
        float rs = float(circles[i].r * circles[i].r);
        float xs = float((x - circles[i].x) * (x - circles[i].x));
        float ys = float((y - circles[i].y) * (y - circles[i].y));

        contrib += rs / (xs + ys);
    }

    return contrib;
}

void Canvas::applyBounds(CircleDef &circle)
{
    int nx = circle.x, ny = circle.y;

    nx += circle.dir_x;
    ny += circle.dir_y;

    if (nx < circle.r)
    {
        nx = circle.r;
        circle.dir_x *= -1;
    }

    if (nx > canvasRect.w - circle.r)
    {
        nx = canvasRect.w - circle.r;
        circle.dir_x *= -1;
    }

    if (ny < circle.r)
    {
        ny = circle.r;
        circle.dir_y *= -1;
    }

    if (ny > canvasRect.h - circle.r)
    {
        ny = canvasRect.h - circle.r;
        circle.dir_y *= -1;
    }

    circle.x = nx;
    circle.y = ny;
}

void Canvas::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    squareMarch(renderer);

    for (int i = 0; i < circleCount; i++)
    {
        // filledCircleRGBA(renderer, circles[i].x, circles[i].y, resolution, color.r, color.g, color.b, 200);
        int bx = (circles[i].x / resolution - 1) * resolution;
        int by = (circles[i].y / resolution - 1) * resolution;

        int r = (int)(color.r * 0.7);
        int g = (int)(color.g * 0.7);
        int b = (int)(color.b * 0.7);

        boxRGBA(renderer, bx, by, bx + 3 * resolution, by + 3 * resolution, r, g, b, 255);

        applyBounds(circles[i]);
    }

    SDL_RenderPresent(renderer);

    lerpColor();

    populateGrid();
} 