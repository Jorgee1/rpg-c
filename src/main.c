#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "utils.h"
#include "controller.h"
#include "charset.h"

#include "entity/entity.h"
#include "sprite.h"

#include "views/index.h"
#include "views/game/game.h"
#include "views/start/start.h"
#include "views/pause/pause.h"



int main (int argc, char* argv[])
{
    time_t rand_seed;
    srand((unsigned) time(&rand_seed));

    Colors colors;
    colors.red   = (SDL_Color) {0xFF, 0x00, 0x00, 0xFF};
    colors.green = (SDL_Color) {0x00, 0xFF, 0x00, 0xFF};
    colors.blue  = (SDL_Color) {0x00, 0x00, 0xFF, 0xFF};
    colors.white = (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF};
    colors.black = (SDL_Color) {0x00, 0x00, 0x00, 0xFF};

    ///////////////////////////////////////////////////////////////////////////////////////
    // SDL INIT ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    int upscale = 4; // For sprites

    Screen screen;
    int init = init_screen(
        &screen,
        "RPG Test 1",
        800, 600,
        colors.white,
        SDL_INIT_VIDEO,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (init) return 1;

    screen.view_index = VIEW_START;

    screen.input.up     = init_button(SDL_SCANCODE_UP);
    screen.input.down   = init_button(SDL_SCANCODE_DOWN);
    screen.input.left   = init_button(SDL_SCANCODE_LEFT);
    screen.input.right  = init_button(SDL_SCANCODE_RIGHT);

    screen.input.action = init_button(SDL_SCANCODE_Z);
    screen.input.cancel = init_button(SDL_SCANCODE_X);
    screen.input.start  = init_button(SDL_SCANCODE_RETURN);

    ///////////////////////////////////////////////////////////////////////////////////////
    // TTF INIT ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    int font_size = 30;

    const int letters_size = 128;
    Letter letters_black[letters_size];
    Letter letters_white[letters_size];
    Letter letters_green[letters_size];
    Letter letters_red[letters_size];


    if (TTF_Init() < 0) return 1;

    TTF_Font* font = TTF_OpenFont("assets/fonts/RobotoMono-Regular.ttf", font_size);
    if (font == NULL) return 1;

    create_charset(screen.renderer, font, letters_black, letters_size, colors.black);
    create_charset(screen.renderer, font, letters_white, letters_size, colors.white);
    create_charset(screen.renderer, font, letters_green, letters_size, colors.green);
    create_charset(screen.renderer, font, letters_red,   letters_size,   colors.red);

    TTF_CloseFont(font);
    TTF_Quit();

    font = NULL;

    ///////////////////////////////////////////////////////////////////////////////////////
    // CHARACTERS /////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    // Load sprite sheets
    char* sprite_sheet_path[PLAYER_STATE_TOTAL] = {
        "assets/textures/idle.png",
        "assets/textures/walk.png"
    };
    SpriteSheet sheet[PLAYER_STATE_TOTAL];


    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        int cell_size = 26; // spritesheet is divided in a 26x26 grid
        int is_loaded = load_spritesheet(
            &sheet[i],
            screen.renderer,
            sprite_sheet_path[i],
            cell_size
        );
        if (is_loaded) return 1;
    }

    // Animation database

    Animation **animations; // declaring using [][] crashes the program

    // Alocationg space
    animations = malloc(sizeof(Animation *) * PLAYER_STATE_TOTAL);
    for (int i=0; i < PLAYER_STATE_TOTAL; i++)
    {
        animations[i] = malloc(sizeof(Animation) * PLAYER_FACES_TOTAL);
    }

    // Initialization
    for (int i=0; i < PLAYER_STATE_TOTAL; i++)
    {
        int cell_size = sheet[i].cell_size;
        int animation_size = sheet[i].rect.w / sheet[i].cell_size;


        for (int j=0; j < PLAYER_FACES_TOTAL; j++)
        {
            animations[i][j].speed = 2;
            animations[i][j].n = animation_size;
            animations[i][j].sprites = malloc(sizeof(Sprite) * animation_size);
            for (int k=0; k < animation_size; k++)
            {
                int x = k * cell_size;
                int y = j * cell_size;
                Sprite *sprite = &animations[i][j].sprites[k];

                load_sprite(sprite, &sheet[i], x, y, cell_size);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // MAPS ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    const int tileset_size = 2;

    char *tileset_path[] = {
        "assets/textures/inside.png",
        "assets/textures/outside.png"
    };

    SpriteSheet tile_texture[tileset_size];

    for (int i=0; i < tileset_size; i++)
    {
        int cell_size = 16;
        char *path = tileset_path[i];
        SDL_Renderer *renderer = screen.renderer;

        load_spritesheet(&tile_texture[i], renderer, path, cell_size);
    }

    TileSet *tiles = malloc(sizeof(TileSet) * tileset_size);

    for (int i = 0; i < tileset_size; i++)
    {
        load_tileset(&tiles[i], &tile_texture[i]);
    }

    // map

    Map maps[tileset_size];

    for (int i = 0; i < tileset_size; i++)
    {
        maps[i].size.x = 0;
        maps[i].size.y = 0;
        maps[i].size.w = 40;
        maps[i].size.h = 30;

        maps[i].tiles = &tiles[1]; // Carefull with this value

        maps[i].indexes = malloc(sizeof(int *) * maps[i].size.h);
        for (int y = 0; y < maps[i].size.h; y++)
        {
            maps[i].indexes[y] = malloc(sizeof(int) * maps[i].size.w);
            for (int x = 0; x < maps[i].size.w; x++)
            {
                int index = rand() % maps[i].tiles->n;
                maps[i].indexes[y][x] = index; // Carefull with this value
            }
        }
    }



    char *map_path = "assets/maps/map.map";
    FILE *file = fopen(map_path, "r");
    uint32_t _w = 0;
    uint32_t _h = 0;
    fread(&_w, sizeof(uint32_t), 1, file);
    fread(&_h, sizeof(uint32_t), 1, file);
    printf("%i, %i\n", (int) _w, (int) _h);

    Map map2;
    map2.tiles = &tiles[0];
    map2.size.x = 0;
    map2.size.y = 0;
    map2.size.h = (int) _h;
    map2.size.w = (int) _w;
    map2.indexes = malloc(sizeof(int *) * _h);
    for (int y = 0; y < _h; y++)
    {
        map2.indexes[y] = malloc(sizeof(int) * _w); 
        for (int x = 0; x < _w; x++)
        {
            uint32_t _index = 0;
            fread(&_index, sizeof(uint32_t), 1, file);
            map2.indexes[y][x] = (int) _index;
            printf("%i\n", map2.indexes[y][x]);
        }
    }

    fclose(file);
    file = NULL;

    Map *map1 = &map2;
    ///////////////////////////////////////////////////////////////////////////////////////
    // GAME INIT //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    const int entity_size = 10;

    Entity entity[entity_size];
    for (int i = 0; i < entity_size; i++)
    {
        entity[i].rect.x = 90 + i*60;
        entity[i].rect.y = 100;
        entity[i].rect.h = 100;
        entity[i].rect.w = 100;

        entity[i].speed = (SDL_Point) {0, 0};
        entity[i].max_speed = (SDL_Point) {5, 5};

        int state = PLAYER_WALK;
        int direction = PLAYER_FRONT;

        entity[i].state = state;
        entity[i].direction = direction;
        
        entity[i].sprite = &animations[state][direction].sprites[0];
        entity[i].animation_index = 0;
        entity[i].animation_acc = 0;
    }

    Entity *player = &entity[0];



    while(screen.exit == SDL_GAME_RUN)
    {
        ///////////////////////////////////////////////////////////////////////////////////
        // Events /////////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////

        while(true)
        {
            SDL_Event event;
            if (SDL_PollEvent(&event) == 0) break;
            if (event.type == SDL_QUIT) screen.exit = SDL_GAME_EXIT;
        }

        ///////////////////////////////////////////////////////////////////////////////////
        // Check Keyboard /////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////

        update_buttons(&(screen.input));

        ///////////////////////////////////////////////////////////////////////////////////
        // VIEW INDEX /////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////////////

        switch (screen.view_index)
        {
            case VIEW_START:
            {
                start_update_input(&screen);
                start_render(&screen, letters_black);
            }break;

            case VIEW_GAME:
            {
                game_update_input(&screen, player);
                game_update_word(animations, entity, entity_size);
                game_update_screen(&screen, entity, map1, entity_size, upscale);
            }break;

            case VIEW_PAUSE:
            {
                pause_update_input(&screen);
                pause_update_screen(&screen, entity, map1, entity_size, upscale, letters_black);
            }break;

            default:
            {

            }break;
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    // Cleanup ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    
    destroy_charset(letters_white, letters_size);
    destroy_charset(letters_green, letters_size);
    destroy_charset(letters_red,   letters_size);

    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        delete_spritesheet(&sheet[i]);
    }


    for (int i = 0; i < tileset_size; i++)
    {
        delete_spritesheet(&tile_texture[i]);
    }

    for (int i = 0; i < tileset_size; i++)
    {
        delete_tileset(&tiles[i]);
    }


    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        for (int j = 0; j < PLAYER_FACES_TOTAL; j++)
        {
            free(animations[i][j].sprites);
            animations[i][j].sprites = NULL;
        }

    }


    for (int i = 0; i < tileset_size; i++)
    {
        for (int y = 0; y < maps[i].size.h; y++)
        {
            free(maps[i].indexes[y]);
        }
        free(maps[i].indexes);
    }

    for (int y = 0; y < map2.size.h; y++)
    {
        free(map2.indexes[y]);
    }
    free(map2.indexes);

    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        free(animations[i]);
        animations[i] = NULL;
    }
    free(animations);
    animations = NULL;

    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    screen.renderer = NULL;
    screen.window = NULL;

    SDL_Quit();
    return 0;
}

