#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "physics.h"
#include "entity.h"
#include "mixer.h"
#include "map.h"

#define max_buttons 20

typedef struct Device
{
    const char *type;

    int x;
    int y;
    bool r;
    bool l;

    const Uint8 *state;
    SDL_Keycode sym;
} Device;

class Renderer
{
public:
    Mixer mix;

    Renderer(const char *title, int w, int h, Uint32 flags);
    void cleanUp();
    void clear();
    void update();

    SDL_Texture *loadTexture(const char *path);
    void loadMapTextures(map *loadTo);
    void renderTexture(SDL_Texture *gfx, int x, int y, int w, int h);
    void renderBackground(SDL_Texture *gfx);
    void fill();

    void renderAnimatedTexture(SDL_Texture *gfx[], int textures, int speed, int x, int y, int w, int h);
    void renderAnimatedBackground(SDL_Texture *gfx[], int textures, int speed);

    void renderCursor(SDL_Texture *cursor[], int size);
    bool renderButton(SDL_Texture *gfx[], Mix_Chunk *sfx[], int x, int y, int w, int h, int index);
    bool renderButton(SDL_Texture *gfx, Mix_Chunk *sfx[], int x, int y, int w, int h, int index);
    void renderText(const char *text, int size, bool outline, int x, int y, int w, int h);

    void renderEntity(Entity *entityToRender, int animation_state, int speed);
    void renderEntity(Entity *entityToRender, int animation_state);
    void renderMap(map *mapToRender, int layer);

    void setColor(Uint8 red, Uint8 green, Uint8 blue);

    int getWindowW();
    int getWindowH();
    Uint32 getTick();
    SDL_Renderer *getRenderer();
    SDL_Window *getWindow();
    const Uint8 *getKeyboardState();
    SDL_Keycode getKeyboardSym();
    int getMouseX();
    int getMouseY();
    bool getMouseR();
    bool getMouseL();
    bool getQuit();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event windowEvent;

    Device mouse;
    Device keyboard;

    int button_tmp[max_buttons];

    int win_w;
    int win_h;
    int dp_w;
    int dp_h;

    bool quitting = false;

    Uint32 tick;
    Uint32 lastTick;

    Uint32 refreshTick;

    Uint32 refresh_rate = 25;

    TTF_Font *font32;
    TTF_Font *font32_outline;
    TTF_Font *font24;
    TTF_Font *font24_outline;
    TTF_Font *font16;
    TTF_Font *font16_outline;

    Uint8 r, g, b;
};