#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "utils.h"
#include "controller.h"
#include "charset.h"


typedef struct
{
	SDL_Texture *texture;
    SDL_Rect rect;
    char *path;
} SpriteSheet;

int load_sprite_sheet(SDL_Renderer *renderer, SpriteSheet *sheet, char *path)
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

    SDL_QueryTexture(sheet->texture, NULL, NULL, &sheet->rect.w, &sheet->rect.h);
    SDL_FreeSurface(surface);

    IMG_Quit();
    return 0;
}

typedef struct
{
	SpriteSheet *sheet;
    SDL_Rect rect;
} Sprite;

typedef struct
{
	int n;
	int speed;
	Sprite *sprites;
} Animation;

typedef struct
{
	SDL_Rect rect;
	SDL_Point speed;
	SDL_Point max_speed;
	int direction;
	int state;

	int animation_index;
	int animation_acc;

	Sprite *sprite;
} Entity;


int main(int argc, char* argv[])
{

    ///////////////////////////////////////////////////////////////////////////////////////
    // COLOR //////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    Colors colors;
    colors.red   = (SDL_Color) {0xFF, 0x00, 0x00, 0xFF};
    colors.green = (SDL_Color) {0x00, 0xFF, 0x00, 0xFF};
    colors.blue  = (SDL_Color) {0x00, 0x00, 0xFF, 0xFF};
    colors.white = (SDL_Color) {0xFF, 0xFF, 0xFF, 0xFF};
    colors.black = (SDL_Color) {0x00, 0x00, 0x00, 0xFF};

    ///////////////////////////////////////////////////////////////////////////////////////
    // SDL INIT ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

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

    ///////////////////////////////////////////////////////////////////////////////////////
    // BUTTONS ////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    Input input;
    input.up     = init_button(SDL_SCANCODE_UP);
    input.down   = init_button(SDL_SCANCODE_DOWN);
    input.left   = init_button(SDL_SCANCODE_LEFT);
    input.right  = init_button(SDL_SCANCODE_RIGHT);

    input.action = init_button(SDL_SCANCODE_Z);
    input.cancel = init_button(SDL_SCANCODE_X);
    input.start  = init_button(SDL_SCANCODE_RETURN);

    ///////////////////////////////////////////////////////////////////////////////////////
    // TTF INIT ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    int font_size = 30;

    const int letters_size = 128;
    Letter letters_white[letters_size];
    Letter letters_green[letters_size];
    Letter letters_red[letters_size];

    if (TTF_Init() < 0) return 1;

    TTF_Font* font = TTF_OpenFont("assets/fonts/RobotoMono-Regular.ttf", font_size);
    if (font == NULL) return 1;

    create_charset(screen.renderer, font, letters_white, letters_size, colors.white);
    create_charset(screen.renderer, font, letters_green, letters_size, colors.green);
    create_charset(screen.renderer, font, letters_red,   letters_size,   colors.red);

    TTF_CloseFont(font);
    font = NULL;

    ///////////////////////////////////////////////////////////////////////////////////////
    // TEXTURES ///////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    enum PLAYER_STATES
    {
        PLAYER_IDLE,
        PLAYER_WALK,
        PLAYER_STATE_TOTAL
    };

    enum PLAYER_FACES
    {
        PLAYER_FRONT,
        PLAYER_RIGHT,
        PLAYER_LEFT,
        PLAYER_BACK,
        PLAYER_FACES_TOTAL
    };


    // Load sprite sheets
    char* sprite_sheet_path[PLAYER_STATE_TOTAL] = {
        "assets/textures/idle.png",
        "assets/textures/walk.png"
    };
    SpriteSheet sheet[PLAYER_STATE_TOTAL];


    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        int is_loaded = load_sprite_sheet(
            screen.renderer,
            &sheet[i],
            sprite_sheet_path[i]
        );
        if (is_loaded) return 1;
    }

    // Animation database

    Animation animations[PLAYER_STATE_TOTAL][PLAYER_FACES_TOTAL];
    int cell_size = 26;
    int animation_speed = 2;


    for (int i=0; i < PLAYER_STATE_TOTAL; i++)
    {
        int animation_size = sheet[i].rect.w / cell_size;

        for (int j=0; j < PLAYER_FACES_TOTAL; j++)
        {
            animations[i][j].n = animation_size;
            animations[i][j].speed = animation_speed;
            animations[i][j].sprites = malloc(sizeof(Sprite)*animation_size);
            for (int k=0; k < animation_size; k++)
            {
                printf("X:%i, Y:%i S:%i\n", i*cell_size, j*cell_size, animation_size);
                animations[i][j].sprites[k].sheet = &sheet[i];
                animations[i][j].sprites[k].rect.x = k*cell_size;
                animations[i][j].sprites[k].rect.y = j*cell_size;
                animations[i][j].sprites[k].rect.h = cell_size;
                animations[i][j].sprites[k].rect.w = cell_size;
            }
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////
    // GAME INIT //////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////

    const int entity_size = 500;
    Entity entity[entity_size];

    for (int i = 0; i < entity_size; i++)
    {
        entity[i].rect.x = 100 + i;
        entity[i].rect.y = 100;
        entity[i].rect.h = 100;
        entity[i].rect.w = 100;

        entity[i].speed.x = 0;
        entity[i].speed.y = 0;
        entity[i].max_speed.x = 5;
        entity[i].max_speed.y = 5;

        entity[i].state = PLAYER_WALK;
        entity[i].direction = PLAYER_LEFT;
        
        entity[i].sprite = &(animations[entity[i].state][entity[i].direction].sprites[0]);
        entity[i].animation_index = 0;
        entity[i].animation_acc = 0;
    }

    Entity *player = &(entity[0]);

    int upscale = 3;

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

        update_buttons(&input);

        ///////////////////////////////////////////////////////////////////////////
        // Action Logic ///////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        if (input.up.state)
        {
            player->speed.y = -player->max_speed.y;
            player->speed.x = 0;
            player->state = PLAYER_WALK;
            player->direction = PLAYER_BACK;
            
        }
        else if (input.down.state)
        {
            player->speed.y = player->max_speed.y;
            player->speed.x = 0;
            player->state = PLAYER_WALK;
            player->direction = PLAYER_FRONT;
        }
        else if (input.left.state)
        {
            player->speed.x = -player->max_speed.x;
            player->speed.y = 0;
            player->state = PLAYER_WALK;
            player->direction = PLAYER_LEFT;
        }
        else if (input.right.state)
        {
            player->speed.x = player->max_speed.x;
            player->speed.y = 0;
            player->state = PLAYER_WALK;
            player->direction = PLAYER_RIGHT;
        }
        else
        {
            player->speed.y = 0;
            player->speed.x = 0;
            player->state = PLAYER_IDLE;
        }

        ///////////////////////////////////////////////////////////////////////////
        // Colition Detection /////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////
        // Update World ///////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < entity_size; i++)
        {

            // Space update
            entity[i].rect.x += entity[i].speed.x;
            entity[i].rect.y += entity[i].speed.y;


            entity[i].animation_acc += 1;
            // animation frame update
            if (entity[i].animation_acc > animations[entity[i].state][entity[i].direction].speed)
            {
                entity[i].animation_acc = 0;
                entity[i].animation_index = (entity[i].animation_index + 1) % animations[entity[i].state][entity[i].direction].n;
            }

            // To prevent overflows
            if (entity[i].animation_index >= animations[entity[i].state][entity[i].direction].n)
            {
                entity[i].animation_index = 0;
                entity[i].animation_acc = 0;
            }
            // Current sprite pointer update
            entity[i].sprite = &(animations[entity[i].state][entity[i].direction].sprites[entity[i].animation_index]);
        }


        ///////////////////////////////////////////////////////////////////////////
        // Render /////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////////

        set_render_draw_color(screen.renderer, screen.clear_color);
        SDL_RenderClear(screen.renderer);

        for (int i = 0; i < entity_size; i++)
        {
            SDL_Rect sprite_position;
            sprite_position.x = entity[i].rect.x;
            sprite_position.y = entity[i].rect.y;
            sprite_position.h = entity[i].sprite->rect.h * upscale;
            sprite_position.w = entity[i].sprite->rect.w * upscale;

            SDL_RenderCopy(
                screen.renderer,
                entity[i].sprite->sheet->texture,
                &(entity[i].sprite->rect),
                &sprite_position
            );

        }
        SDL_RenderPresent(screen.renderer);
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

    for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
    {
        for (int j = 0; j < PLAYER_FACES_TOTAL; j++)
        {
            free(animations[i][j].sprites);
            animations[i][j].sprites = NULL;
        }

    }

    SDL_DestroyRenderer(screen.renderer);
    SDL_DestroyWindow(screen.window);
    screen.renderer = NULL;
    screen.window = NULL;

    TTF_Quit();
    SDL_Quit();

    return 0;
}

