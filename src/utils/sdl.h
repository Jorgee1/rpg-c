#include <SDL.h>

#ifndef SDL_UTILS_H
#define SDL_UTILS_H

typedef struct
{
	SDL_Color red;
	SDL_Color green;
	SDL_Color blue;
	SDL_Color white;
	SDL_Color black;
} Colors;


enum EXIT_SATE
{
	SDL_GAME_RUN,
	SDL_GAME_EXIT
};

typedef struct
{
	char *name;
	int view_index;
	int exit;
	
	SDL_Rect rect;
	SDL_Color clear_color;

	SDL_Window* window;
	SDL_Renderer* renderer;
} Screen;

int check_collition(SDL_Rect, SDL_Rect);

void set_render_draw_color(SDL_Renderer*, SDL_Color*);

#endif
