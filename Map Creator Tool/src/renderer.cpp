#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include "renderer.h"
#include "physics.h"
#include "entity.h"
#include "mixer.h"
#include "map.h"
#else
#include "include/renderer.h"
#include "include/physics.h"
#include "include/entity.h"
#include "include/mixer.h"
#include "include/map.h"
#endif

Renderer::Renderer(const char *title, int w, int h, Uint32 flags)
    : window(NULL), renderer(NULL)
{
    win_w = w;
    win_h = h;

    mouse.type = "mouse";
    keyboard.type = "keyboard";

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    mix.init(4);

#ifdef __EMSCRIPTEN__
    font32 = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
    font32_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 32);
    font24 = TTF_OpenFont("res/fonts/cocogoose.ttf", 24);
    font24_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 24);
    font16 = TTF_OpenFont("res/fonts/cocogoose.ttf", 16);
    font16_outline = TTF_OpenFont("res/fonts/cocogoose.ttf", 16);
    TTF_SetFontOutline(font32_outline, 3);
    TTF_SetFontOutline(font24_outline, 3);
    TTF_SetFontOutline(font16_outline, 3);
#endif
}

void Renderer::cleanUp()
{
    SDL_DestroyWindow(window);
}

void Renderer::clear()
{
    SDL_RenderClear(renderer);
}

void Renderer::update()
{
    quitting = false;
    while (SDL_PollEvent(&windowEvent))
    {
        switch (windowEvent.type)
        {
        case SDL_QUIT:
            quitting = true;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (windowEvent.button.button == SDL_BUTTON_LEFT)
            {
                mouse.l = true;
            }
            if (windowEvent.button.button == SDL_BUTTON_RIGHT)
            {
                mouse.r = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (windowEvent.button.button == SDL_BUTTON_LEFT)
            {
                mouse.l = false;
            }
            if (windowEvent.button.button == SDL_BUTTON_RIGHT)
            {
                mouse.r = false;
            }
            break;
        case SDL_KEYDOWN:
            switch (windowEvent.key.keysym.sym)
            {
            default:
                keyboard.sym = windowEvent.key.keysym.sym;
            }
        default:
            break;
        }
    }

    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x = mapper(mouse.x, 0, dp_w, 0, win_w);
    mouse.y = mapper(mouse.y, 0, dp_h, 0, win_h);
    keyboard.state = SDL_GetKeyboardState(NULL);

    SDL_RenderPresent(renderer);
    SDL_GetWindowSize(window, &dp_w, &dp_h);

    lastTick = tick;
    tick = SDL_GetTicks();

    Uint32 difference = tick - lastTick;

    if (difference < refresh_rate)
    {
        SDL_Delay(refresh_rate - difference);
    }
}

SDL_Texture *Renderer::loadTexture(const char *path)
{
    return IMG_LoadTexture(renderer, path);
}

void Renderer::loadMapTextures(map *loadTo)
{
    for (int i = 0; i < loadTo->usedTextures; i++)
    {
        //char path[max_texpathlen];
        //strcpy(path, "res/gfx/");
        //strcpy(path, loadTo->paths[i]);
        loadTo->textures[i] = loadTexture(loadTo->paths[i]);
    }
}

void Renderer::renderTexture(SDL_Texture *gfx, int x, int y, int w, int h)
{
    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = 0;
    src.h = 0;
    SDL_QueryTexture(gfx, NULL, NULL, &src.w, &src.h);

    SDL_Rect dst;
    dst.x = mapper(x, 0, win_w, 0, dp_w);
    dst.y = mapper(y, 0, win_h, 0, dp_h);
    dst.w = mapper(w, 0, win_w, 0, dp_w);
    dst.h = mapper(h, 0, win_h, 0, dp_h);

    SDL_RenderCopy(renderer, gfx, &src, &dst);
}

void Renderer::renderBackground(SDL_Texture *gfx)
{
    renderTexture(gfx, 0, 0, win_w, win_h);
}

void Renderer::renderAnimatedTexture(SDL_Texture *gfx[], int textures, int speed, int x, int y, int w, int h)
{
    renderTexture(gfx[tick % textures], x, y, w, h);
}

void Renderer::renderAnimatedBackground(SDL_Texture *gfx[], int textures, int speed)
{
    renderAnimatedTexture(gfx, textures, speed, 0, 0, win_w, win_h);
}

void Renderer::renderCursor(SDL_Texture *cursor[], int size)
{
    if (mouse.l || mouse.r)
    {
        renderTexture(cursor[1], mouse.x - size / 2, mouse.y - size / 2, size, size);
    }
    else
    {
        renderTexture(cursor[0], mouse.x - size / 2, mouse.y - size / 2, size, size);
    }
}

bool Renderer::renderButton(SDL_Texture *gfx[], Mix_Chunk *sfx[], int x, int y, int w, int h, int index)
{
    if (index >= max_buttons)
        return false;

    bool focused = isCollide(mouse.x, mouse.y, x, y, w, h);
    renderTexture(gfx[button_tmp[index]], x, y, w, h);
    if (focused && mouse.l && button_tmp[index] == 0)
    {
        Mix_PlayChannel(1, sfx[0], 0);
        button_tmp[index] = 1;
    }
    else if (focused && not mouse.l && button_tmp[index] == 1)
    {
        Mix_PlayChannel(1, sfx[1], 0);
        button_tmp[index] = 0;
        return true;
    }
    else if (!focused)
    {
        button_tmp[index] = 0;
    }

    return false;
}

bool Renderer::renderButton(SDL_Texture *gfx, Mix_Chunk *sfx[], int x, int y, int w, int h, int index)
{
    if (index >= max_buttons)
        return false;

    bool focused = isCollide(mouse.x, mouse.y, x, y, w, h);
    renderTexture(gfx, x, y, w, h);
    if (focused && mouse.l && button_tmp[index] == 0)
    {
        Mix_PlayChannel(1, sfx[0], 0);
        button_tmp[index] = 1;
    }
    else if (focused && not mouse.l && button_tmp[index] == 1)
    {
        Mix_PlayChannel(1, sfx[1], 0);
        button_tmp[index] = 0;
        return true;
    }
    else if (!focused)
    {
        button_tmp[index] = 0;
    }

    return false;
}

void Renderer::renderText(const char *text, int size, bool outline, int x, int y, int w, int h)
{
#ifdef __EMSCRIPTEN__
    TTF_Font *font;
    switch (size)
    {
    case 16:
        if (outline)
        {
            font = font16_outline;
        }
        else
        {
            font = font16;
        }
        break;

    case 24:
        if (outline)
        {
            font = font24_outline;
        }
        else
        {
            font = font24;
        }
        break;

    case 32:
        if (outline)
        {
            font = font32_outline;
        }
        else
        {
            font = font32;
        }
        break;

    default:
        font = font24;
        break;
    }

    SDL_Color color = {r, g, b};
    SDL_Surface *surfaceMessage = TTF_RenderText_Blended(font, text, color);
    SDL_Texture *message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect src;
    src.x = 0;
    src.y = 0;
    src.w = surfaceMessage->w;
    src.h = surfaceMessage->h;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(renderer, message, &src, &dst);
    SDL_FreeSurface(surfaceMessage);
#endif
}

void Renderer::renderEntity(Entity *entityToRender, int animation_state, int speed)
{
    renderAnimatedTexture(entityToRender->animations[animation_state], 4, speed, entityToRender->x, entityToRender->y, entityToRender->w, entityToRender->h);
}

void Renderer::renderEntity(Entity *entityToRender, int animation_state)
{
    renderEntity(entityToRender, animation_state, 2);
}

void Renderer::renderMap(map *mapToRender, int layer)
{
    for (int i = 0; i < mapToRender->usedObjects; i++)
    {
        if (mapToRender->object[i][3] == layer)
            renderTexture(mapToRender->textures[mapToRender->object[i][0]], mapToRender->object[i][1] + mapToRender->offset_x, mapToRender->object[i][2] + mapToRender->offset_y, mapToRender->object[i][4], mapToRender->object[i][5]);
    }
}

void Renderer::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
    r = red;
    g = green;
    b = blue;
}

int Renderer::getWindowW()
{
    return win_w;
}

int Renderer::getWindowH()
{
    return win_h;
}

Uint32 Renderer::getTick()
{
    return tick;
}

SDL_Renderer *Renderer::getRenderer()
{
    return renderer;
}

SDL_Window *Renderer::getWindow()
{
    return window;
}

const Uint8 *Renderer::getKeyboardState()
{
    return keyboard.state;
}

SDL_Keycode Renderer::getKeyboardSym()
{
    return keyboard.sym;
}

int Renderer::getMouseX()
{
    return mouse.x;
}

int Renderer::getMouseY()
{
    return mouse.y;
}

bool Renderer::getMouseR()
{
    return mouse.r;
}

bool Renderer::getMouseL()
{
    return mouse.l;
}

bool Renderer::getQuit()
{
    return quitting;
}