#include "start.h"

void start_update_input(Screen *screen)
{
    Input *input = &(screen->input);
    if (input->start.action_state)
    {
        screen->view_index = VIEW_GAME;
    }
}

void start_render(Screen *screen, Letter *letters)
{
    SDL_Renderer *renderer = screen->renderer;

    set_render_draw_color(renderer, screen->clear_color);
    SDL_RenderClear(renderer);

    render_string(renderer, 0, 0, letters, "RPG Test");
    render_string(renderer, 0, 100, letters, "Prest Start");

    SDL_RenderPresent(renderer);
}