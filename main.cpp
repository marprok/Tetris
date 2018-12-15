#include "SDL.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "tetrominos.h"

static constexpr int WIDTH = 10;
static constexpr int HEIGHT = 22;
static constexpr int block_size = 10;

static bool field[WIDTH*HEIGHT];

bool isValidMove(int x, int y, int cid, int angle)
{
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
        {
            char c = tetrominos[cid][angle][j*4 + i];
            if (c == '1' && (x + i >= WIDTH || x + i < 0 || y + j >= HEIGHT || y + j < 0 || !field[(y+j)*WIDTH + x + i]))
                return false;
        }
    return true;
}

void cap_fps(float deltaTime, uint32_t fps)
{
    uint32_t dt = static_cast<uint32_t>(deltaTime);
    if (1000/fps > dt)
        SDL_Delay((1000/fps) - dt);
}

int main()
{
    srand(time(NULL));
    //Prepare the data
    int cid, cx, cy;
    const uint32_t FPS = 60;
    bool keys[] = {false, false, false, false};
	
	INIT_TETROMINOS();
    for (auto &p : field)
        p = true;

    SDL_Window *window;
    SDL_Renderer *renderer;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << SDL_GetError() << std::endl;
        return 1;
    }

    if (SDL_CreateWindowAndRenderer(WIDTH*block_size, HEIGHT*block_size, SDL_WINDOW_SHOWN, &window, &renderer) < 0)
    {
        std::cerr << SDL_GetError() << std::endl;
        return 1;
    }

    uint32_t tic = 0, counter = 0;
    bool gameOver = false;
    cid = rand() % tetrominos.size();
    cx = WIDTH/2;
    cy = 0;
    int angle = 0;

    while (!gameOver)
    {
        tic = SDL_GetTicks();
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                gameOver = true;
                break;
            case SDL_KEYDOWN:
                switch (e.key.keysym.sym)
                {
                case SDLK_a:
                    keys[0] = true;
                    break;
                case SDLK_d:
                    keys[1] = true;
                    break;
                case SDLK_s:
                    keys[2] = true;
                    break;
                case SDLK_SPACE:
                    keys[3] = true;
                    break;
                }
                break;
            }
        }
        // Key presses
        if (keys[0])
        {
            if (isValidMove(cx - 1, cy, cid, angle))
                cx--;
            keys[0] = false;
        }

        if (keys[1])
        {
            if (isValidMove(cx + 1, cy, cid, angle))
                cx++;
            keys[1] = false;
        }

        if (keys[2])
        {
            if (isValidMove(cx, cy + 1, cid, angle))
                cy++;
            keys[2] = false;
        }

        if (keys[3])
        {
            if (isValidMove(cx, cy, cid, (angle + 1) % 4))
                angle = (angle + 1) % 4;
            keys[3] = false;
        }

        // Game logic
        if (counter >= FPS)
        {
            // Move the current piece one block down every 1 sec
            counter = counter - FPS;
            if (cy + 1 < HEIGHT && isValidMove(cx, cy + 1, cid, angle))
                cy += 1;
            else
            {
                for (int i = 0; i < 4; ++i)
                    for (int j = 0; j < 4; ++j)
                    {
                        char c = tetrominos[cid][angle][j*4 + i];
                        if (c == '1')
                            field[(cy + j)*WIDTH + cx + i] = false;
                    }
                // Check for lines
                for (int j = 0; j < 4; ++j)
                {
                    if (cy + j < HEIGHT)
                    {
                        bool line = true;
                        for (int i = 0; i < WIDTH; ++i)
                            if (field[(cy + j)*WIDTH + i])
                            {
                                line = false;
                                break;
                            }
                        if (line)
                        {
                            // Make the current line available and then move all the previous
                            // lines move one place down.
                            for (int i = 0; i < WIDTH; ++i)
                                field[(cy + j)*WIDTH + i] = true;
                            for (int l = j + cy; l > 0; --l)
                            {
                                for (int i = 0; i < WIDTH; ++i)
                                {
                                    field[(l)*WIDTH + i] = field[(l - 1)*WIDTH + i];
                                    field[(l - 1)*WIDTH + i] = true;
                                }
                            }
                        }
                    }
                }
                // New Tetromino
                cid = rand() % tetrominos.size();
                cy = 0;
                cx = WIDTH/2;

                if (!isValidMove(cx, cy, cid, angle))
                    gameOver = true;
            }

        }
        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
        SDL_RenderClear(renderer);
        // Update the graphics
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);

        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
            {
                char c = tetrominos[cid][angle][j*4 + i];
                // Translate the coordinates
                int tx = cx*block_size, ty = cy*block_size;

                if (c == '1' && cx + i < WIDTH && cy + j < HEIGHT && cx + i >= 0 && cy + j >= 0)
                {
                    SDL_Rect r{tx + block_size*i, ty + j*block_size, block_size, block_size};
                    SDL_RenderFillRect(renderer, &r);
                    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 0x00);
                    SDL_RenderDrawRect(renderer, &r);
                    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                }
            }

        for (int i = 0; i < WIDTH; ++i)
            for (int j = 0; j < HEIGHT; ++j)
            {
                if (!field[j*WIDTH + i])
                {
                    // Translated coordinates
                    SDL_Rect r{i*block_size, j*block_size, block_size, block_size};
                    SDL_RenderFillRect(renderer, &r);
                    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
                    SDL_RenderDrawRect(renderer, &r);
                    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
                }
            }

        SDL_RenderPresent(renderer);
        counter++;
        cap_fps(SDL_GetTicks() - tic, FPS);

    }
    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
