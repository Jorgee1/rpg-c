#include <SDL.h>

#include "utils.h"
#include "controller.h"
#include "charset.h"

#include "views/index.h"

#ifndef START_H
#define START_H

void start_update_input(Screen *);
void start_render(Screen *, Letter *);

#endif