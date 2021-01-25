#include "sprite.h"

// SpriteSheet
int load_spritesheet(SpriteSheet *sheet, SDL_Renderer *renderer, char *path, int cell_size)
{
    int img_flags = IMG_INIT_PNG;
    if((IMG_Init(img_flags) & img_flags) != img_flags) return 1;

    sheet->path = path;

    SDL_Surface *surface = NULL;
    surface = IMG_Load(path);
    if (surface == NULL) return 1;

    sheet->rect.x = 0;
    sheet->rect.y = 0;
    sheet->texture = SDL_CreateTextureFromSurface(renderer, surface);
    sheet->cell_size = cell_size;

    SDL_QueryTexture(sheet->texture, NULL, NULL, &sheet->rect.w, &sheet->rect.h);
    SDL_FreeSurface(surface);

    IMG_Quit();
    return 0;
}

void delete_spritesheet(SpriteSheet *sheet)
{
    SDL_DestroyTexture(sheet->texture);
    sheet->texture = NULL;
}

// Sprites
void load_sprite(Sprite *sprite, SpriteSheet *sheet, int x, int y, int size)
{
    sprite->texture = sheet->texture;
    
    sprite->rect.x = x;
    sprite->rect.y = y;
    sprite->rect.w = size;
    sprite->rect.h = size;
}

// Tiles
void load_tileset(TileSet *tileset, SpriteSheet *sheet)
{
    int w = sheet->rect.w / sheet->cell_size;
    int h = sheet->rect.h / sheet->cell_size;
    int n = w * h;

    tileset->n = n;
    
    tileset->sprites = malloc(sizeof(Sprite) * n);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            int i = x + (y * w);
            int size = sheet->cell_size;

            Sprite *sprite = &(tileset->sprites[i]);
            load_sprite(sprite, sheet, x * size, y * size, size);
        }
    }
}

void delete_tileset(TileSet *tileset)
{
    free(tileset->sprites);
    tileset->n = 0;
}


void map_render(SDL_Renderer *renderer, Map *map, int upscale)
{
    for (int y = 0; y < map->size.h; y++)
    {
        for (int x = 0; x < map->size.w; x++)
        {
            int index = map->indexes[y][x];

            Sprite *sprites = &map->tiles->sprites[index];

            SDL_Rect rect;
            rect.x = x * sprites->rect.w * upscale;
            rect.y = y * sprites->rect.h * upscale;
            rect.w = sprites->rect.w * upscale;
            rect.h = sprites->rect.h * upscale; 

            SDL_RenderCopy(
                renderer,
                sprites->texture,
                &sprites->rect,
                &rect
            );
        }
    }

}