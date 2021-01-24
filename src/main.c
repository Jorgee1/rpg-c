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
#include "sprites/sprite.h"

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

    int upscale = 3; // For sprites

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
        int is_loaded = load_sprite_sheet(
            screen.renderer,
            &sheet[i],
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
        animations[i] = malloc(sizeof(Animation)*PLAYER_FACES_TOTAL);
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
                animations[i][j].sprites[k].sheet = sheet[i].texture;
                animations[i][j].sprites[k].rect.x = k*cell_size;
                animations[i][j].sprites[k].rect.y = j*cell_size;
                animations[i][j].sprites[k].rect.h = cell_size;
                animations[i][j].sprites[k].rect.w = cell_size;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////
    // MAPS ///////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    
    const int tileset_size = 2;

    char *tileset_path[tileset_size] = {
        "assets/textures/inside.png",
        "assets/textures/outside.png"
    };

    SpriteSheet tileset[tileset_size];

    for (int i=0; i < tileset_size; i++)
    {
        int cell_size = 16;
        char *path = tileset_path[i];
        SDL_Renderer *renderer = screen.renderer;

        load_sprite_sheet(renderer, &tileset[i], path, cell_size);
    }

    // tile database
    Sprite **tiles = malloc(sizeof(Sprite *) * tileset_size);

    for (int t=0; t < tileset_size; t++)
    {
        int size = tileset[t].cell_size;

        int w = tileset[t].rect.w / size;
        int h = tileset[t].rect.h / size;


        tiles[t] = malloc(sizeof(Sprite) * w * h);

        for (int y=0; y < h; y++)
        {
            for (int x=0; x < w; x++)
            {
                int index = x + (y * w);

                tiles[t][index].sheet = tileset[t].texture;

                tiles[t][index].rect.x = x * size;
                tiles[t][index].rect.y = y * size;
                tiles[t][index].rect.w = size;
                tiles[t][index].rect.h = size;
            }
        }

    }

    // map
    typedef struct
    {
        SDL_Rect size;
        int tileset;
        int **tiles;
    } Maps;

    Maps map1;
    map1.size.x = 0;
    map1.size.y = 0;
    map1.size.w = 80;
    map1.size.h = 60;

    map1.tileset = 1; // Carefull with this value

    map1.tiles = malloc(sizeof(int *) * map1.size.h);
    for (int y = 0; y < map1.size.h; y++)
    {
        map1.tiles[y] = malloc(sizeof(int) * map1.size.w);
        for (int x = 0; x < map1.size.w; x++)
        {
            int size = tileset[map1.tileset].cell_size;
            int w = tileset[map1.tileset].rect.w / size;
            int h = tileset[map1.tileset].rect.h / size;

            int index = rand() % size;
            map1.tiles[y][x] = index; // Carefull with this value
        }
    }

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
        int direction = PLAYER_LEFT;

        entity[i].state = state;
        entity[i].direction = direction;
        
        entity[i].sprite = &(animations[state][direction].sprites[0]);
        entity[i].animation_index = 0;
        entity[i].animation_acc = 0;
    }

    Entity *player = &(entity[0]);



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
                // game_update_screen(&screen, entity, entity_size, upscale);


                SDL_Renderer *renderer = screen.renderer;

                set_render_draw_color(renderer, screen.clear_color);
                SDL_RenderClear(renderer);


                for (int y = 0; y < map1.size.h; y++)
                {
                    for (int x = 0; x < map1.size.h; x++)
                    {
                        int index = map1.tiles[y][x];


                        SDL_Rect rect;
                        rect.x = x * tiles[map1.tileset][index].rect.w * upscale;
                        rect.y = y * tiles[map1.tileset][index].rect.h * upscale;
                        rect.w = tiles[map1.tileset][index].rect.w * upscale;
                        rect.h = tiles[map1.tileset][index].rect.h * upscale; 

                        SDL_RenderCopy(
                            renderer,
                            tiles[map1.tileset][index].sheet,
                            &(tiles[map1.tileset][index].rect),
                            &rect
                        );
                    }
                }


                game_render(&screen, entity, entity_size, upscale);

                SDL_RenderPresent(renderer);

            }break;

            case VIEW_PAUSE:
            {
                pause_update_input(&screen);
                pause_update_screen(&screen, entity, entity_size, upscale, letters_black);
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
        SDL_DestroyTexture(sheet[i].texture);
        sheet[i].texture = NULL;
    }

    for (int i = 0; i < tileset_size; i++)
    {
        SDL_DestroyTexture(tileset[i].texture);
        tileset[i].texture = NULL;
    }

    for (int t=0; t < tileset_size; t++)
    {
        free(tiles[t]);
    }
    free(tiles);

    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        for (int j = 0; j < PLAYER_FACES_TOTAL; j++)
        {
            free(animations[i][j].sprites);
            animations[i][j].sprites = NULL;
        }

    }

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

