#include "utils.h"
#include "controller.h"
#include "charset.h"

#include "views/index.h"
#include "views/game/game.h"

#ifndef PAUSE_H
#define PAUSE_H

void pause_update_input(Screen *);
void pause_update_screen(Screen *, Entity [], Map *, int, int, Letter *);

#endif