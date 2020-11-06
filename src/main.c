#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include "utils/sdl.h"
#include "input/input.h"
#include "charset/charset.h"

typedef struct{
	SDL_Rect rect;
	SDL_Point speed;
	SDL_Point max_speed;
	int direction;
	int state;
} Entity;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
}
Data;

typedef struct
{
	SDL_Rect position;
	SDL_Texture* texture;
} Sprite;


typedef struct{
	uint16_t n;
	Sprite *sprites;
} Animation;


typedef struct
{
	char *texture;
	char *data;
} SpriteAsset;

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

	enum ANIMATION_STATES{
		PLAYER_STATE_FRONT_IDLE,
		PLAYER_STATE_SIDE_IDLE,
		PLAYER_STATE_BACK_IDLE,
		PLAYER_STATE_FRONT_WALK,
		PLAYER_STATE_SIDE_WALK,
		PLAYER_STATE_BACK_WALK,
		PLAYER_STATE_TOTAL
	};

	SpriteAsset assets[PLAYER_STATE_TOTAL];
	
	assets[PLAYER_STATE_FRONT_IDLE].texture = "assets/textures/idle_front.png";
	assets[PLAYER_STATE_FRONT_IDLE].data    = "assets/textures/idle_front.data";
	assets[PLAYER_STATE_SIDE_IDLE].texture  = "assets/textures/idle_side.png";
	assets[PLAYER_STATE_SIDE_IDLE].data     = "assets/textures/idle_side.data";
	assets[PLAYER_STATE_BACK_IDLE].texture  = "assets/textures/idle_back.png";
	assets[PLAYER_STATE_BACK_IDLE].data     = "assets/textures/idle_back.data";

	assets[PLAYER_STATE_FRONT_WALK].texture = "assets/textures/walk_front.png";
	assets[PLAYER_STATE_FRONT_WALK].data    = "assets/textures/walk_front.data";
	assets[PLAYER_STATE_SIDE_WALK].texture  = "assets/textures/walk_side.png";
	assets[PLAYER_STATE_SIDE_WALK].data     = "assets/textures/walk_side.data";
	assets[PLAYER_STATE_BACK_WALK].texture  = "assets/textures/walk_back.png";
	assets[PLAYER_STATE_BACK_WALK].data     = "assets/textures/walk_back.data";

	SDL_Texture *textures[PLAYER_STATE_TOTAL];
	Animation animations[PLAYER_STATE_TOTAL];

	for (int i = 0; i < PLAYER_STATE_TOTAL; i++)
	{
		// Load Sprite Sheet
		SDL_Surface *surface = NULL;
		surface = IMG_Load(assets[i].texture);
		if (surface == NULL) return 1;

		textures[i] = SDL_CreateTextureFromSurface(screen.renderer, surface);
		SDL_FreeSurface(surface);

		// Load Sprite Positions
		FILE *file = fopen(assets[i].data, "rb");
		if (file == NULL) return 1;

		fread(&animations[i].n, sizeof(uint16_t), 1, file);

		animations[i].sprites = malloc(sizeof(Sprite)*animations[i].n);

		for (uint16_t j = 0; j < animations[i].n; j++)
		{
			Data data;
			fread(&data, sizeof(Data), 1, file);
			animations[i].sprites[j].position.x = data.x;
			animations[i].sprites[j].position.y = data.y;
			animations[i].sprites[j].position.h = data.h;
			animations[i].sprites[j].position.w = data.w;
			animations[i].sprites[j].texture = textures[i];
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

	enum PLAYER_STATES
	{
		PLAYER_IDLE,
		PLAYER_WALK
	};

	Entity player;
	player.rect.x = 100;
	player.rect.y = 100;
	player.rect.h = 100;
	player.rect.w = 100;

	player.speed.x = 0;
	player.speed.y = 0;
	player.max_speed.x = 6;
	player.max_speed.y = 6;
	player.state = PLAYER_IDLE;
	player.direction = PLAYER_DIRECTION_FRONT;


	int upscale = 4;
	int animation_state = PLAYER_STATE_FRONT_IDLE;
	int animation_acc = 0;
	int animation_index = 0;

	SDL_RendererFlip flipType = SDL_FLIP_NONE;

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
			player.speed.y = -player.max_speed.y;
			player.speed.x = 0;
			player.state = PLAYER_WALK;
			player.direction = PLAYER_DIRECTION_BACK;
			
		}
		else if (input.down.state)
		{
			player.speed.y = player.max_speed.y;
			player.speed.x = 0;
			player.state = PLAYER_WALK;
			player.direction = PLAYER_DIRECTION_FRONT;
		}
		else if (input.left.state)
		{
			player.speed.x = -player.max_speed.x;
			player.speed.y = 0;
			player.state = PLAYER_WALK;
			player.direction = PLAYER_DIRECTION_LEFT;
		}
		else if (input.right.state)
		{
			player.speed.x = player.max_speed.x;
			player.speed.y = 0;
			player.state = PLAYER_WALK;
			player.direction = PLAYER_DIRECTION_RIGHT;
		}
		else
		{
			player.speed.y = 0;
			player.speed.x = 0;
			player.state = PLAYER_IDLE;
		}

		///////////////////////////////////////////////////////////////////////////
		// Colition Detection /////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		// Update World ///////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		player.rect.x += player.speed.x;
		player.rect.y += player.speed.y;

		// Animation update
		if (player.state == PLAYER_IDLE)
		{
			switch (player.direction)
			{
				case PLAYER_DIRECTION_FRONT:{
					animation_state = PLAYER_STATE_FRONT_IDLE;
					flipType = SDL_FLIP_NONE;
					break;
				}
				case PLAYER_DIRECTION_BACK:{
					animation_state = PLAYER_STATE_BACK_IDLE;
					flipType = SDL_FLIP_NONE;
					break;
				}
				case PLAYER_DIRECTION_LEFT:{
					animation_state = PLAYER_STATE_SIDE_IDLE;
					flipType = SDL_FLIP_NONE;
					break;
				}
				case PLAYER_DIRECTION_RIGHT:{
					animation_state = PLAYER_STATE_SIDE_IDLE;
					flipType = SDL_FLIP_HORIZONTAL;
					break;
				}
			}
		}
		else if (player.state == PLAYER_WALK)
		{
			switch (player.direction)
			{
				case PLAYER_DIRECTION_FRONT:{
					animation_state = PLAYER_STATE_FRONT_WALK;
					flipType = SDL_FLIP_NONE;
					break;
				}
				case PLAYER_DIRECTION_BACK:{
					animation_state = PLAYER_STATE_BACK_WALK;
					flipType = SDL_FLIP_NONE;
					break;
				}
				case PLAYER_DIRECTION_LEFT:{
					animation_state = PLAYER_STATE_SIDE_WALK;
					flipType = SDL_FLIP_NONE;
					break;
				}
				case PLAYER_DIRECTION_RIGHT:{
					animation_state = PLAYER_STATE_SIDE_WALK;
					flipType = SDL_FLIP_HORIZONTAL;
					break;
				}
			}
		}

		animation_acc += 1;

		if (animation_acc > 2)
		{
			animation_acc = 0;
			animation_index = (animation_index + 1) % animations[animation_state].n;
		}

		// To prevent overflows
		if (animation_index >= animations[animation_state].n)
		{
			animation_index = 0;
		}

		///////////////////////////////////////////////////////////////////////////
		// Render /////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		set_render_draw_color(screen.renderer, &screen.clear_color);
		SDL_RenderClear(screen.renderer);

		SDL_Rect sprite_position;
		sprite_position.x = player.rect.x;
		sprite_position.y = player.rect.y;
		sprite_position.h = animations[animation_state].sprites[animation_index].position.h * upscale;
		sprite_position.w = animations[animation_state].sprites[animation_index].position.w * upscale;

		SDL_RenderCopyEx(
			screen.renderer,
			animations[animation_state].sprites[animation_index].texture,
			&(animations[animation_state].sprites[animation_index].position),
			&sprite_position,
			0,
			NULL,
			flipType
		);
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
		free(animations[i].sprites);
	}

	SDL_DestroyRenderer(screen.renderer);
	SDL_DestroyWindow(screen.window);

	TTF_Quit();
	SDL_Quit();

	return 0;
}
