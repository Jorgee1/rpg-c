#include <SDL.h>
#include <SDL_image.h>

#ifndef SPRITE_H
#define SPRITE_H

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect rect;
    char *path;
    int cell_size;
} SpriteSheet;

typedef struct
{
    SpriteSheet *sheet; //Might be better to give a texture pointer
    SDL_Rect rect;
} Sprite;

typedef struct
{
    int n;
    int speed;
    Sprite *sprites;
} Animation;

int load_sprite_sheet(
    SDL_Renderer *,
    SpriteSheet *,
    char *,
    int
);

#endif