#include "charset.h"

int get_string_w(Letter* letters, char word[])
{
	int acc = 0;
	int word_size = strlen(word);
	for (int i = 0; i < word_size; i++)
	{
		int word_index = (int) word[i];
		acc += letters[word_index].w;
	}
	return acc;
}

int get_string_h(Letter* letters, char word[])
{
	int acc = 0;
	int word_size = strlen(word);
	for (int i = 0; i < word_size; i++)
	{
		int word_index = (int) word[i];
		if (acc < letters[word_index].h) acc = letters[word_index].h;
	}
	return acc;
}

void create_charset
(
	SDL_Renderer* renderer,
	TTF_Font* font,
	Letter letters[],
	int size,
	SDL_Color color
)
{
	for (int i = 0; i < size; i++)
	{
		char string_char[] = {(char) i, '\0'};
		SDL_Surface* surface = TTF_RenderText_Blended(font, string_char, color);

		letters[i].letter = (char) i;
		letters[i].texture = SDL_CreateTextureFromSurface(renderer, surface);
		TTF_SizeText(font, string_char, &letters[i].w, &letters[i].h);

		SDL_FreeSurface(surface);
	}
}

void destroy_charset(Letter letters[], int size)
{
	for (int i = 0; i < size; i++)
	{
		SDL_DestroyTexture(letters[i].texture);
		letters[i].texture = NULL;
		letters[i].h = 0;
		letters[i].w = 0;
	}
}

void render_string(SDL_Renderer* renderer, int x, int y, Letter* letters, char word[])
{
	int word_size = strlen(word);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.h = 0;
	rect.w = 0;

	for (int i = 0; i < word_size; i++)
	{
		int word_index = (int) word[i];

		rect.h = letters[word_index].h;
		rect.w = letters[word_index].w;

		SDL_RenderCopy(renderer, letters[word_index].texture, NULL, &rect);

		rect.x += letters[word_index].w;
	}
}

void render_int(SDL_Renderer* renderer, int x, int y, Letter* letters, int value)
{
	char buffer[10];
	sprintf(buffer, "%d", value);

	render_string(renderer, x, y, letters, buffer);
}