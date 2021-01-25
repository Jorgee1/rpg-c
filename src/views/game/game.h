#include <SDL.h>

#include "utils.h"
#include "controller.h"
#include "entity/entity.h"

#include "views/index.h"

#ifndef GAME_H
#define GAME_H

void game_update_input(Screen *, Entity *);
void game_update_word(Animation **, Entity [], int);
void game_update_screen(Screen *, Entity [], Map*, int, int);

void game_render_world(Screen *, Map*, int);
void game_render(Screen *, Entity [], int, int);

#endif