#include "pause.h"

void pause_update_input(Screen *screen)
{
    Input *input = &(screen->input);
    if (input->start.action_state)
    {
        screen->view_index = VIEW_GAME;
    }
}

void pause_update_screen(Screen *screen, Entity entity[], int size, int upscale, Letter *letters)
{
    SDL_Renderer *renderer = screen->renderer;

    set_render_draw_color(renderer, screen->clear_color);
    SDL_RenderClear(renderer);

    game_render(screen, entity, size, upscale);
    render_string(renderer, 0, 0, letters, "Pause");

    SDL_RenderPresent(renderer);
}