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

int load_spritesheet(SpriteSheet *, SDL_Renderer *, char *, int);
void delete_spritesheet(SpriteSheet *);

typedef struct
{
    SDL_Texture *texture;
    SDL_Rect rect;
} Sprite;

void load_sprite(Sprite *, SpriteSheet *, int, int, int);


typedef struct
{
    int n;
    Sprite *sprites;
} TileSet;

void load_tileset(TileSet *, SpriteSheet *);
void delete_tileset(TileSet *);

typedef struct
{
    SDL_Rect size;
    TileSet *tiles;
    int **indexes;
} Map;

void map_render(SDL_Renderer *, Map *, int);

typedef struct
{
    int n;
    int speed;
    Sprite *sprites;
} Animation;


#endif