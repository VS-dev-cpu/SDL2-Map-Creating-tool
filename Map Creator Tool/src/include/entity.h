#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define animations_number 9
#define texturesperanimation 4

typedef struct Entity
{
	SDL_Texture *animations[animations_number][texturesperanimation];	//0 idle; 1 forward; 2 backward; 3 right; 4 left;

	int tpa = texturesperanimation;	//textures/animation

	int all_anims = animations_number;

	int astate[animations_number];

	int all_astates = animations_number;

	int x, y, w, h;

	int speed;
	int health;
	int damage;

	Entity *next;
} Entity;

typedef struct EntityType
{
	Entity *type;
} EntityType;

void EsetType(Entity *entity, EntityType *type);

void Eappend(Entity **e, EntityType *type, float x, float y);