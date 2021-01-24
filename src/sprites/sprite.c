#include "sprite.h"

int load_sprite_sheet(SDL_Renderer *renderer, SpriteSheet *sheet, char *path, int cell_size)
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

