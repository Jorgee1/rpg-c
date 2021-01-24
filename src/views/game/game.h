#include <SDL.h>

#include "utils.h"
#include "controller.h"
#include "entity/entity.h"

#ifndef GAME_H
#define GAME_H

void game_update_input(Input *, Entity *);
void game_update_word(Animation **, Entity [], int);
void game_render(Screen *, Entity [], int, int);

#endif