#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "utils/sdl.h"
#include "input/input.h"
#include "charset/charset.h"


typedef struct
{
	char *texture;
	char *data;
} SpriteSheetAsset;

typedef struct
{
	SDL_Rect position;
	SDL_Texture* texture;
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
	SDL_RendererFlip flip;
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
	screen.rect.x = 0;
	screen.rect.y = 0;
	screen.rect.w = 640;
	screen.rect.h = 480;

	screen.name = "RPG Test 1";
	screen.clear_color = colors.white;
	screen.exit = SDL_GAME_RUN;


	if(SDL_Init(SDL_INIT_VIDEO)) return 1;

	screen.window = SDL_CreateWindow(
		screen.name,
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		screen.rect.w,
		screen.rect.h,
		SDL_WINDOW_HIDDEN
	);
	if (screen.window == NULL) return 1;


	screen.renderer = SDL_CreateRenderer(
		screen.window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);
	if (screen.renderer == NULL) return 1;

	SDL_ShowWindow(screen.window);
	///////////////////////////////////////////////////////////////////////////////////////
	// BUTTONS ////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	Input input;
	input.up     = (Button) {SDL_SCANCODE_UP   , 0, 0, 0};
	input.down   = (Button) {SDL_SCANCODE_DOWN , 0, 0, 0};
	input.left   = (Button) {SDL_SCANCODE_LEFT , 0, 0, 0};
	input.right  = (Button) {SDL_SCANCODE_RIGHT, 0, 0, 0};

	input.action = (Button) {SDL_SCANCODE_Z, 0, 0, 0};
	input.cancel = (Button) {SDL_SCANCODE_X, 0, 0, 0};
	input.start  = (Button) {SDL_SCANCODE_RETURN, 0, 0, 0};

	const int button_size = 7;
	Button* buttons[button_size] = {
		&input.up,
		&input.down,
		&input.left,
		&input.right,

		&input.action,
		&input.cancel,
		&input.start
	};

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
	int img_flags = IMG_INIT_PNG;
    if((IMG_Init(img_flags) & img_flags) != img_flags) return 1;

	enum PLAYER_STATES
	{
		PLAYER_IDLE,
		PLAYER_WALK,
		PLAYER_STATE_TOTAL
	};


	enum ANIMATION_STATES_VARIATION
	{
		ANIMATION_FRONT,
		ANIMATION_SIDE,
		ANIMATION_BACK,
		ANIMATION_STATE_TOTAL
	};

	SpriteSheetAsset assets[PLAYER_STATE_TOTAL];
	
	assets[PLAYER_IDLE].texture = "assets/textures/idle.png";
	assets[PLAYER_IDLE].data    = "assets/textures/idle.data";

	assets[PLAYER_WALK].texture = "assets/textures/walk.png";
	assets[PLAYER_WALK].data    = "assets/textures/walk.data";

	// Texture database
	SDL_Texture *textures[PLAYER_STATE_TOTAL];

	// Animation database
	Animation animations[PLAYER_STATE_TOTAL * ANIMATION_STATE_TOTAL];
	
	for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
	{
		// Load Sprite Positions
		FILE *file = fopen(assets[i].data, "rb");
		if (file == NULL) return 1;

		// Load Sprite Sheet
		SDL_Surface *surface = NULL;
		surface = IMG_Load(assets[i].texture);
		if (surface == NULL) return 1;

		textures[i] = SDL_CreateTextureFromSurface(screen.renderer, surface);
		SDL_FreeSurface(surface);

		int total_animations = 0;
		fread(&(total_animations), sizeof(int), 1, file);

		if (total_animations != ANIMATION_STATE_TOTAL) return 1; // Expecting fixed size

		for (int j = 0; j < ANIMATION_STATE_TOTAL; j++)
		{
			fread(&animations[(i*ANIMATION_STATE_TOTAL) + j].n, sizeof(int), 1, file);

			animations[(i*ANIMATION_STATE_TOTAL) + j].sprites = malloc(sizeof(Sprite)*animations[(i*ANIMATION_STATE_TOTAL) + j].n);
			animations[(i*ANIMATION_STATE_TOTAL) + j].speed = 2;
			for (int k = 0; k < animations[(i*ANIMATION_STATE_TOTAL) + j].n; k++)
			{
				fread(&(animations[(i*ANIMATION_STATE_TOTAL) + j].sprites[k].position), sizeof(SDL_Rect), 1, file);
				animations[(i*ANIMATION_STATE_TOTAL) + j].sprites[k].texture = textures[i];
			}
		}
		fclose(file);
	}

	
	IMG_Quit();



	///////////////////////////////////////////////////////////////////////////////////////
	// GAME INIT //////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	enum PLAYER_DIRECTIONS
	{
		PLAYER_DIRECTION_FRONT,
		PLAYER_DIRECTION_LEFT,
		PLAYER_DIRECTION_RIGHT,
		PLAYER_DIRECTION_BACK
	};

	const int entity_size = 400;
	Entity entity[entity_size];

	for (int i = 0; i < entity_size; i++)
	{
		entity[i].rect.x = 100 + i;
		entity[i].rect.y = 100;
		entity[i].rect.h = 100;
		entity[i].rect.w = 100;

		entity[i].speed.x = 0;
		entity[i].speed.y = 0;
		entity[i].max_speed.x = 6;
		entity[i].max_speed.y = 6;

		entity[i].state = PLAYER_WALK;
		entity[i].direction = PLAYER_DIRECTION_LEFT;
		
		entity[i].sprite = &(animations[(entity[i].state * ANIMATION_STATE_TOTAL) + ANIMATION_SIDE].sprites[0]);
		entity[i].animation_index = 0;
		entity[i].animation_acc = 0;		
		
		entity[i].flip = SDL_FLIP_NONE;
	}

	Entity *player = &(entity[0]);

	int upscale = 4;

	while(screen.exit == SDL_GAME_RUN)
	{
		///////////////////////////////////////////////////////////////////////////////////
		// Events /////////////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////

		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) screen.exit = SDL_GAME_EXIT;
		}

		///////////////////////////////////////////////////////////////////////////////////
		// Check Keyboard /////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////////////

		update_buttons(buttons, button_size);

		///////////////////////////////////////////////////////////////////////////
		// Action Logic ///////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		if (input.up.state)
		{
			player->speed.y = -player->max_speed.y;
			player->speed.x = 0;
			player->state = PLAYER_WALK;
			player->direction = PLAYER_DIRECTION_BACK;
			
		}
		else if (input.down.state)
		{
			player->speed.y = player->max_speed.y;
			player->speed.x = 0;
			player->state = PLAYER_WALK;
			player->direction = PLAYER_DIRECTION_FRONT;
		}
		else if (input.left.state)
		{
			player->speed.x = -player->max_speed.x;
			player->speed.y = 0;
			player->state = PLAYER_WALK;
			player->direction = PLAYER_DIRECTION_LEFT;
		}
		else if (input.right.state)
		{
			player->speed.x = player->max_speed.x;
			player->speed.y = 0;
			player->state = PLAYER_WALK;
			player->direction = PLAYER_DIRECTION_RIGHT;
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
			int animation_state;

			// Space update
			entity[i].rect.x += entity[i].speed.x;
			entity[i].rect.y += entity[i].speed.y;

			// Animation update
			if (entity[i].state == PLAYER_IDLE)
			{
				switch (entity[i].direction)
				{
					case PLAYER_DIRECTION_FRONT:{
						animation_state = (PLAYER_IDLE * ANIMATION_STATE_TOTAL)  + ANIMATION_FRONT;
						entity[i].flip = SDL_FLIP_NONE;
						break;
					}
					case PLAYER_DIRECTION_BACK:{
						animation_state = (PLAYER_IDLE * ANIMATION_STATE_TOTAL)  + ANIMATION_BACK;
						entity[i].flip = SDL_FLIP_NONE;
						break;
					}
					case PLAYER_DIRECTION_LEFT:{
						animation_state = (PLAYER_IDLE * ANIMATION_STATE_TOTAL)  + ANIMATION_SIDE;
						entity[i].flip = SDL_FLIP_NONE;
						break;
					}
					case PLAYER_DIRECTION_RIGHT:{
						animation_state = (PLAYER_IDLE * ANIMATION_STATE_TOTAL)  + ANIMATION_SIDE;
						entity[i].flip = SDL_FLIP_HORIZONTAL;
						break;
					}
				}
			}
			else if (entity[i].state == PLAYER_WALK)
			{
				switch (entity[i].direction)
				{
					case PLAYER_DIRECTION_FRONT:{
						animation_state = (PLAYER_WALK * ANIMATION_STATE_TOTAL) + ANIMATION_FRONT;
						entity[i].flip = SDL_FLIP_NONE;
						break;
					}
					case PLAYER_DIRECTION_BACK:{
						animation_state = (PLAYER_WALK * ANIMATION_STATE_TOTAL) + ANIMATION_BACK;
						entity[i].flip = SDL_FLIP_NONE;
						break;
					}
					case PLAYER_DIRECTION_LEFT:{
						animation_state = (PLAYER_WALK * ANIMATION_STATE_TOTAL) + ANIMATION_SIDE;
						entity[i].flip = SDL_FLIP_NONE;
						break;
					}
					case PLAYER_DIRECTION_RIGHT:{
						animation_state = (PLAYER_WALK * ANIMATION_STATE_TOTAL) + ANIMATION_SIDE;
						entity[i].flip = SDL_FLIP_HORIZONTAL;
						break;
					}
				}
			}
			
			entity[i].animation_acc += 1;
			// animation frame update
			if (entity[i].animation_acc > animations[animation_state].speed)
			{
				entity[i].animation_acc = 0;
				entity[i].animation_index = (entity[i].animation_index + 1) % animations[animation_state].n;
			}

			// To prevent overflows
			if (entity[i].animation_index >= animations[animation_state].n)
			{
				entity[i].animation_index = 0;
				entity[i].animation_acc = 0;
			}

			// Current sprite pointer update
			entity[i].sprite = &(animations[animation_state].sprites[entity[i].animation_index]);
		}


		///////////////////////////////////////////////////////////////////////////
		// Render /////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		set_render_draw_color(screen.renderer, &screen.clear_color);
		SDL_RenderClear(screen.renderer);

		for (int i = 0; i < entity_size; i++)
		{
			SDL_Rect sprite_position;
			sprite_position.x = entity[i].rect.x;
			sprite_position.y = entity[i].rect.y;
			sprite_position.h = entity[i].sprite->position.h * upscale;
			sprite_position.w = entity[i].sprite->position.w * upscale;

			SDL_RenderCopyEx(
				screen.renderer,
				entity[i].sprite->texture,
				&(entity[i].sprite->position),
				&sprite_position,
				0,
				NULL,
				entity[i].flip
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
		SDL_DestroyTexture(textures[i]);
	}

	for (int i = 0; i < ANIMATION_STATE_TOTAL; i++)
	{
		free(animations[i].sprites);
	}

	SDL_DestroyRenderer(screen.renderer);
	SDL_DestroyWindow(screen.window);

	TTF_Quit();
	SDL_Quit();

	return 0;
}
