#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#ifdef __EMSCRIPTEN__
#include "entity.h"
#else
#include "include/entity.h"
#endif

void EsetType(Entity *entity, EntityType *type)
{
	for (int a = 0; a < type->type->all_anims; a++)
	{
		for (int b = 0; b < type->type->tpa; b++)
		{
			entity->animations[a][b] = type->type->animations[a][b];
		}
	}

	for (int i = 0; i < type->type->all_astates; i++)
	{
		entity->astate[i] = 0;
	}

	entity->x = type->type->x;
	entity->y = type->type->y;
	entity->w = type->type->w;
	entity->h = type->type->h;

	entity->speed = type->type->speed;
	entity->health = type->type->health;
	entity->damage = type->type->damage;

	entity->next = NULL;
}

void Eappend(Entity **e, EntityType *type, float x, float y)
{
	Entity *n = new Entity;
	Entity *last = *e;
	EsetType(n, type);

	if (e == NULL)
	{
		*e = n;
		return;
	}
	while (last->next != NULL)
		last = last->next;
	last->next = n;
	return;
}