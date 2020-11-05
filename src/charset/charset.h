/* 
 * File that handle everyting related to printing text on a window.
 * 
 * 
 * make sure to call TTF_Init()
 *
 *
 * the struct Letter should be called as an array of 128 positions
 * representing ASCII.
 * struct Letter letters[128];
 * 
 * To use, first you need to load the font.
 * 
 * font = TTF_OpenFont('path_to_font', font_size);
 * 
 * then call the funtionc to load the letters textures
 * create_charset(renderer, font, letter_struct_array, struct_size, sdl_color)
 * 
 * To print a sting on screen call the function render_string
 * 
 * render_string(renderer, x, y, letter_struct_array, string)
 * 
 * There is also a function similar to print an int (buffer to 10 spaces)
 * 
 * When done loading, close the font as is no longer needed
 *
 * TTF_CloseFont(font);
 * font = NULL;
 * 
 */


#include <SDL.h>
#include <SDL_ttf.h>

#ifndef CHARSET_H
#define CHARSET_H

typedef struct
{
	char letter;
	SDL_Texture* texture;
	int h;
	int w;	
} Letter;

int get_string_w(Letter*, char[]);

int get_string_h(Letter*, char[]);

void create_charset(
	SDL_Renderer*,
	TTF_Font*,
	Letter[],
	int,
	SDL_Color
);

void destroy_charset(Letter[], int);

void render_string(
	SDL_Renderer*,
	int x,
	int y,
	Letter*,
	char word[]
);

void render_int(
	SDL_Renderer*,
	int x,
	int y,
	Letter*,
	int
);

#endif
