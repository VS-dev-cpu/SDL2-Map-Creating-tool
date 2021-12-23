#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#ifdef __EMSCRIPTEN__
#include "renderer.h"
#else
#include "include/renderer.h"
#include "include/physics.h"
#include "include/entity.h"
#include "include/mixer.h"
#include "include/map.h"
#endif

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

SDL_Texture *logo;
Mix_Chunk *music;

map *level = new map;

SDL_Texture *button_load_gfx[2];
SDL_Texture *button_save_gfx[2];
SDL_Texture *button_open_gfx[2];
Mix_Chunk *button_sfx[2];

SDL_Texture *alert_gfx;
SDL_Texture *background;

SDL_Texture *cursor_gfx[2];

int selectedImage = -1;
int currentX, currentY, currentW, currentH;
int currentState = 0;

Renderer renderer("SDL2", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALWAYS_ON_TOP);

void alert(bool state)
{
	if (state == true)
	{
		renderer.renderTexture(alert_gfx, 0, 980, 100, 100);
	}
	renderer.update();
}

bool init()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	// ----- Loading
	button_load_gfx[0] = renderer.loadTexture("res/gfx/button_load_a.png");
	button_load_gfx[1] = renderer.loadTexture("res/gfx/button_load_b.png");

	button_save_gfx[0] = renderer.loadTexture("res/gfx/button_save_a.png");
	button_save_gfx[1] = renderer.loadTexture("res/gfx/button_save_b.png");

	button_open_gfx[0] = renderer.loadTexture("res/gfx/button_open_a.png");
	button_open_gfx[1] = renderer.loadTexture("res/gfx/button_open_b.png");

	button_sfx[0] = renderer.mix.load("res/sfx/button_play_a.wav");
	button_sfx[1] = renderer.mix.load("res/sfx/button_play_b.wav");

	alert_gfx = renderer.loadTexture("res/gfx/alert.png");
	background = renderer.loadTexture("res/gfx/background.png");

	// ----- Before loop
	//SDL_ShowCursor(0);
	renderer.update();
	renderer.clear();

	return true;
}

bool load = init();

bool gameLoop()
{
	renderer.setColor(0, 68, 80);
	renderer.clear();
	renderer.renderBackground(background);

	if (renderer.getQuit())
	{
#ifdef __EMSCRIPTEN__
		printf("exit\n");
#else
		return true;
#endif
	}

	if (renderer.renderButton(button_load_gfx, button_sfx, 0, 0, 100, 100, 0))
	{
		cout << "Map to Open: maps/";
		string in;
		alert(true);
		cin >> in;
		alert(false);
		char open[max_textypes];
		strcpy(open, "maps/");
		strcat(open, in.c_str());
		cout << "Searching... ";

		if (loadMap(open, level))
		{
			renderer.loadMapTextures(level);
			cout << "Found & Loaded!\n";
		}
		else
		{
			cout << "Not Found!\n";
		}
	}

	if (renderer.renderButton(button_save_gfx, button_sfx, 0, 150, 100, 100, 1))
	{
		cout << "Name to Save: maps/";
		string in;
		alert(true);
		cin >> in;
		alert(false);
		char save[max_textypes];
		strcpy(save, "maps/");
		strcat(save, in.c_str());
		saveMap(save, *level);
		cout << "Saved Map!\n";
	}

	if (renderer.renderButton(button_open_gfx, button_sfx, 0, 300, 100, 100, 2))
	{
		cout << "File to open: res/gfx/";
		string in;
		alert(true);
		cin >> in;
		alert(false);
		char open[max_texpathlen];
		strcpy(open, "res/gfx/");
		strcat(open, in.c_str());
		cout << "Searching... ";
		if (renderer.loadTexture(open) != NULL)
		{
			for (int i = 0; i < max_texpathlen; i++)
				level->paths[level->usedTextures][i] = open[i];

			level->textures[level->usedTextures] = renderer.loadTexture(level->paths[level->usedTextures]);
			level->usedTextures++;
			cout << "Found & Loaded!\n";
		}
		else
		{
			cout << "Not Found!\n";
		}
	}

	for (int i = 0; i < level->usedTextures; i++)
	{
		if (i < 7)
		{
			if (renderer.renderButton(level->textures[i], button_sfx, 1080, i * 100, 100, 100, i + 5))
			{
				selectedImage = i;
			}
		}
		else if (i < 14)
		{
			if (renderer.renderButton(level->textures[i], button_sfx, 1180, (i - 7) * 100, 100, 100, i + 5))
			{
				selectedImage = i;
			}
		}
	}

	if (selectedImage != -1)
	{
		if (currentState == 0)
		{
			if (renderer.getKeyboardState()[SDL_SCANCODE_LCTRL])
			{
				currentX = roundTo(100, 1, renderer.getMouseX());
				currentY = roundTo(100, 1, renderer.getMouseY());
			}
			else
			{
				currentX = renderer.getMouseX();
				currentY = renderer.getMouseY();
			}

			if (renderer.getMouseL())
				currentState = 1;
		}
		else if (currentState == 1)
		{
			if (renderer.getKeyboardState()[SDL_SCANCODE_LCTRL])
			{
				currentW = roundTo(100, 1, renderer.getMouseX() - currentX);
				currentH = roundTo(100, 1, renderer.getMouseY() - currentY);
			}
			else
			{
				currentW = renderer.getMouseX() - currentX;
				currentH = renderer.getMouseY() - currentY;
			}

			if (!renderer.getMouseL())
			{
				int in;
				level->object[level->usedObjects][0] = selectedImage;
				level->object[level->usedObjects][1] = currentX;
				level->object[level->usedObjects][2] = currentY;
				cout << "Rendering Order: ";
				cin >> in;
				level->object[level->usedObjects][3] = in;
				level->object[level->usedObjects][4] = currentW;
				level->object[level->usedObjects][5] = currentH;
				cout << "Has Collision (0/1): ";
				cin >> in;
				level->object[level->usedObjects][6] = in;
				cout << "Custom Index (default: 0): ";
				cin >> in;
				level->object[level->usedObjects][7] = in;
				cout << "\n\n";

				level->usedObjects++;

				currentState = 0;
				selectedImage = -1;
			}
		}

		renderer.renderTexture(level->textures[selectedImage], currentX, currentY, currentW, currentH);
	}

	int max_rendering = 3;
	for (int rendering = 0; rendering < max_rendering; rendering++)
	{
		for (int i = 0; i < level->usedObjects; i++)
		{
			if (level->object[i][3] == rendering)
				renderer.renderTexture(level->textures[level->object[i][0]], level->object[i][1], level->object[i][2], level->object[i][4], level->object[i][5]);
		}
	}

	renderer.update();
	return false;
}

#ifdef __EMSCRIPTEN__
void emscriptenLoop()
{
	gameLoop();
}
#endif

int main(int argc, char *argv[])
{
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(emscriptenLoop, 0, 1);
#else
	while (gameLoop() != true)
		;
#endif

	renderer.cleanUp();
	SDL_Quit();
	return 0;
}