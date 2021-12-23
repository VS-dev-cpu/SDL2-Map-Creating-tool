#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

#define max_tiles 100
#define obj_properties 8 //texture_index, x, y, z (rendering order), w, h, c (collision 0/1), d (custom index)
#define max_textypes 20
#define max_texpathlen 32

typedef struct map
{
    int offset_x, offset_y;
    int object[max_tiles][obj_properties];
    int usedObjects;
    int properties = obj_properties;

    char paths[max_textypes][max_texpathlen];
    SDL_Texture *textures[max_textypes];
    int usedTextures;
} map;

bool loadMap(const char *path, map *loadTo);
bool saveMap(const char *path, map toSave);