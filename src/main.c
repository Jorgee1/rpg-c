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

    // Initialization
    animations = malloc(sizeof(Animation *) * PLAYER_STATE_TOTAL);

    for (int i=0; i < PLAYER_STATE_TOTAL; i++)
    {
        animations[i] = malloc(sizeof(Animation) * PLAYER_FACES_TOTAL);
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

    // Map

    Map map;
    load_map(&map, &tiles[1], "assets/maps/map.map");

    ///////////////////////////////////////////////////////////////////////////////////////
    // GAME INIT //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    const int entity_size = 10;

    Entity entity[entity_size];
    for (int i = 0; i < entity_size; i++)
    {
        SDL_Rect rect;
        rect.x = 90 + i*60;
        rect.y = 100;
        rect.w = 100;
        rect.h = 100;

        int state = PLAYER_WALK;
        int direction = PLAYER_FRONT;

        Sprite *sprite = &animations[state][direction].sprites[0];

        load_entity(&entity[i], rect, sprite, upscale, state, direction);
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
                game_update_screen(&screen, entity, &map, entity_size, upscale);
            }break;

            case VIEW_PAUSE:
            {
                pause_update_input(&screen);
                pause_update_screen(&screen, entity, &map, entity_size, upscale, letters_black);
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

    delete_map(&map);

    for (int i = 0; i < PLAYER_STATE_TOTAL; i++) delete_spritesheet(&sheet[i]);
    for (int i = 0; i < tileset_size; i++)  delete_spritesheet(&tile_texture[i]);
    for (int i = 0; i < tileset_size; i++) delete_tileset(&tiles[i]);


    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        for (int j = 0; j < PLAYER_FACES_TOTAL; j++)
        {
            delete_animation(&animations[i][j]);
        }
        free(animations[i]);
        animations[i] = NULL;
    }
    free(animations);
    animations = NULL;

    delete_screen(&screen);
    return 0;
}

