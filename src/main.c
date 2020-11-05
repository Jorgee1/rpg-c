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
} Entity;

typedef struct
{
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
}
Data;

typedef struct{
	SDL_Rect position;
	SDL_Texture* texture;
} Sprite;

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



	SDL_Surface* surface = NULL;
	FILE *file = NULL;
	uint16_t n = 0;
	int upscale = 4;

	// Player idle
	SDL_Texture* texture = NULL;


	surface = IMG_Load("assets/textures/idle.png");
	if (surface == NULL) return 1;

	texture = SDL_CreateTextureFromSurface(screen.renderer, surface);
	SDL_FreeSurface(surface);


	int sprite_index = 0;


	SDL_RendererFlip flipType = SDL_FLIP_NONE;

	file = fopen("assets/textures/idle.data", "rb");
	if (file == NULL) return 1;

	fread(&n, sizeof(uint16_t), 1, file);

	Sprite *sprite_sheet_1 = malloc(sizeof(Sprite)*n);

	for (uint16_t i = 0; i < n; i++)
	{
		Data data;
		fread(&data, sizeof(data), 1, file);
		sprite_sheet_1[i].position.x = data.x;
		sprite_sheet_1[i].position.y = data.y;
		sprite_sheet_1[i].position.h = data.h;
		sprite_sheet_1[i].position.w = data.w;
		sprite_sheet_1[i].texture = texture;
	}
	fclose(file);


	// Player movement

	// front
	uint16_t n2 = 0;
	int sprite_index_2 = 0;

	SDL_Texture* texture2 = NULL;

	surface = IMG_Load("assets/textures/walk_front.png");
	if (surface == NULL) return 1;

	texture2 = SDL_CreateTextureFromSurface(screen.renderer, surface);
	SDL_FreeSurface(surface);


	file = fopen("assets/textures/walk_front.data", "rb");
	if (file == NULL) return 1;

	fread(&n2, sizeof(uint16_t), 1, file);

	Sprite *sprite_sheet_2 = malloc(sizeof(Sprite)*n2);

	for (uint16_t i=0; i < n2; i++)
	{
		Data data;
		fread(&data, sizeof(Data), 1, file);
		sprite_sheet_2[i].position.x = data.x;
		sprite_sheet_2[i].position.y = data.y;
		sprite_sheet_2[i].position.h = data.h;
		sprite_sheet_2[i].position.w = data.w;
		sprite_sheet_2[i].texture = texture2;
	}
	fclose(file);


	// side
	uint16_t n3 = 0;
	int sprite_index_3 = 0;

	SDL_Texture* texture3 = NULL;

	surface = IMG_Load("assets/textures/walk_side.png");
	if (surface == NULL) return 1;

	texture3 = SDL_CreateTextureFromSurface(screen.renderer, surface);
	SDL_FreeSurface(surface);

	file = fopen("assets/textures/walk_side.data", "rb");

	fread(&n3, sizeof(uint16_t), 1, file);

	Sprite *sprite_sheet_3 = malloc(sizeof(Sprite)*n3);

	for (uint16_t i = 0; i < n3; i++)
	{
		Data data;
		fread(&data, sizeof(data), 1, file);
		sprite_sheet_3[i].position.x = data.x;
		sprite_sheet_3[i].position.y = data.y;
		sprite_sheet_3[i].position.h = data.h;
		sprite_sheet_3[i].position.w = data.w;
		sprite_sheet_3[i].texture = texture3;
	}
	fclose(file);


	// back
	uint16_t n4 = 0;
	int sprite_index_4 = 0;

	SDL_Texture* texture4 = NULL;

	surface = IMG_Load("assets/textures/walk_back.png");
	if (surface == NULL) return 1;

	texture4 = SDL_CreateTextureFromSurface(screen.renderer, surface);
	SDL_FreeSurface(surface);

	file = fopen("assets/textures/walk_back.data", "rb");

	fread(&n4, sizeof(uint16_t), 1, file);

	Sprite *sprite_sheet_4 = malloc(sizeof(Sprite)*n4);

	for (uint16_t i = 0; i < n4; i++)
	{
		Data data;
		fread(&data, sizeof(data), 1, file);
		sprite_sheet_4[i].position.x = data.x;
		sprite_sheet_4[i].position.y = data.y;
		sprite_sheet_4[i].position.h = data.h;
		sprite_sheet_4[i].position.w = data.w;
		sprite_sheet_4[i].texture = texture4;
	}
	fclose(file);



	IMG_Quit();

	int player_state = 0;
	///////////////////////////////////////////////////////////////////////////////////////
	// GAME INIT //////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	Entity player;
	player.rect.x = 100;
	player.rect.y = 100;
	player.rect.h = 100;
	player.rect.w = 100;

	player.speed.x = 0;
	player.speed.y = 0;
	player.max_speed.x = 6;
	player.max_speed.y = 6;

	int acc = 0;

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
			sprite_index = 2;
			flipType = SDL_FLIP_NONE;
			player_state = 3;
		}
		else if (input.down.state)
		{
			player.speed.y = player.max_speed.y;
			player.speed.x = 0;
			sprite_index = 0;
			flipType = SDL_FLIP_NONE;
			player_state = 1;
		}
		else if (input.left.state)
		{
			player.speed.x = -player.max_speed.x;
			player.speed.y = 0;
			sprite_index = 1;
			flipType = SDL_FLIP_NONE;
			player_state = 2;
		}
		else if (input.right.state)
		{
			player.speed.x = player.max_speed.x;
			player.speed.y = 0;
			sprite_index = 1;
			flipType = SDL_FLIP_HORIZONTAL;
			player_state = 2;
		}
		else
		{
			player.speed.y = 0;
			player.speed.x = 0;
			player_state = 0;
			sprite_index_2 = 0;
		}

		///////////////////////////////////////////////////////////////////////////
		// Colition Detection /////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		///////////////////////////////////////////////////////////////////////////
		// Update World ///////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		player.rect.x += player.speed.x;
		player.rect.y += player.speed.y;

		acc += 1;

		if (acc > 2)
		{
			acc = 0;
			sprite_index_2 = (sprite_index_2 + 1) % n2;
		}

		///////////////////////////////////////////////////////////////////////////
		// Render /////////////////////////////////////////////////////////////////
		///////////////////////////////////////////////////////////////////////////

		set_render_draw_color(screen.renderer, &screen.clear_color);
		SDL_RenderClear(screen.renderer);

		//set_render_draw_color(screen.renderer, &colors.red);
		//SDL_RenderFillRect(screen.renderer, &player.rect);


		if (player_state == 0)
		{
			SDL_Rect sprite_position;
			sprite_position.x = player.rect.x;
			sprite_position.y = player.rect.y;
			sprite_position.h = sprite_sheet_1[sprite_index].position.h * upscale;
			sprite_position.w = sprite_sheet_1[sprite_index].position.w * upscale;
			SDL_RenderCopyEx(screen.renderer, sprite_sheet_1[sprite_index].texture, &(sprite_sheet_1[sprite_index].position), &sprite_position, 0, NULL, flipType);
		}
		else if (player_state == 1)
		{
			SDL_Rect sprite_position;
			sprite_position.x = player.rect.x;
			sprite_position.y = player.rect.y;
			sprite_position.h = sprite_sheet_2[sprite_index].position.h * upscale;
			sprite_position.w = sprite_sheet_2[sprite_index].position.w * upscale;
			SDL_RenderCopyEx(screen.renderer, sprite_sheet_2[sprite_index_2].texture, &(sprite_sheet_2[sprite_index_2].position), &sprite_position, 0, NULL, flipType);

		}
		else if (player_state == 2)
		{
			SDL_Rect sprite_position;
			sprite_position.x = player.rect.x;
			sprite_position.y = player.rect.y;
			sprite_position.h = sprite_sheet_3[sprite_index].position.h * upscale;
			sprite_position.w = sprite_sheet_3[sprite_index].position.w * upscale;
			SDL_RenderCopyEx(screen.renderer, sprite_sheet_3[sprite_index_2].texture, &(sprite_sheet_3[sprite_index_2].position), &sprite_position, 0, NULL, flipType);
		}
		else if (player_state == 3)
		{
			SDL_Rect sprite_position;
			sprite_position.x = player.rect.x;
			sprite_position.y = player.rect.y;
			sprite_position.h = sprite_sheet_4[sprite_index].position.h * upscale;
			sprite_position.w = sprite_sheet_4[sprite_index].position.w * upscale;
			SDL_RenderCopyEx(screen.renderer, sprite_sheet_4[sprite_index_2].texture, &(sprite_sheet_4[sprite_index_2].position), &sprite_position, 0, NULL, flipType);
		}



		//render_string(screen.renderer, 0, 0, letters_white, "This is a test!");

		SDL_RenderPresent(screen.renderer);
	}

	///////////////////////////////////////////////////////////////////////////////////////
	// Cleanup ////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	free(sprite_sheet_3);
	free(sprite_sheet_2);
	free(sprite_sheet_1);

	destroy_charset(letters_white, letters_size);
	destroy_charset(letters_green, letters_size);
	destroy_charset(letters_red,   letters_size);

	SDL_DestroyTexture(texture4);
	SDL_DestroyTexture(texture3);
	SDL_DestroyTexture(texture2);
	SDL_DestroyTexture(texture);

	SDL_DestroyRenderer(screen.renderer);
	SDL_DestroyWindow(screen.window);
	
	TTF_Quit();
	SDL_Quit();

	return 0;
}