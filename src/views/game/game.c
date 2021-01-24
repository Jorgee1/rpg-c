#include "game.h"

void game_update_input(Screen *screen, Entity *player)
{
    Input *input = &(screen->input);
    
    if (input->up.state)
    {
        player->speed.y = -player->max_speed.y;
        player->speed.x = 0;
        player->state = PLAYER_WALK;
        player->direction = PLAYER_BACK;
    }
    else if (input->down.state)
    {
        player->speed.y = player->max_speed.y;
        player->speed.x = 0;
        player->state = PLAYER_WALK;
        player->direction = PLAYER_FRONT;
    }
    else if (input->left.state)
    {
        player->speed.x = -player->max_speed.x;
        player->speed.y = 0;
        player->state = PLAYER_WALK;
        player->direction = PLAYER_LEFT;
    }
    else if (input->right.state)
    {
        player->speed.x = player->max_speed.x;
        player->speed.y = 0;
        player->state = PLAYER_WALK;
        player->direction = PLAYER_RIGHT;
    }
    else
    {
        player->speed.y = 0;
        player->speed.x = 0;
        player->state = PLAYER_IDLE;
    }

    if (input->start.action_state)
    {
        screen->view_index = VIEW_PAUSE;
    }
}

void game_update_word(Animation **animations, Entity entity[], int size)
{
    for (int i = 0; i < size; i++)
    {
        int state = entity[i].state;
        int direction = entity[i].direction;

        // Space update
        entity[i].rect.x += entity[i].speed.x;
        entity[i].rect.y += entity[i].speed.y;

        entity[i].animation_acc += 1;

        Animation *animation = &(animations[state][direction]);
        // animation frame update
        if (entity[i].animation_acc > animation->speed)
        {
            int index = (entity[i].animation_index + 1) % animation->n;
            entity[i].animation_acc = 0;
            entity[i].animation_index = index;
        }

        // To prevent overflows
        if (entity[i].animation_index >= animation->n)
        {
            entity[i].animation_index = 0;
            entity[i].animation_acc = 0;
        }
        // Current sprite pointer update
        int index = entity[i].animation_index;
        entity[i].sprite = &(animation->sprites[index]);
    }
}

void game_update_screen(Screen *screen, Entity entity[], int size, int upscale)
{
    SDL_Renderer *renderer = screen->renderer;

    set_render_draw_color(renderer, screen->clear_color);
    SDL_RenderClear(renderer);

    game_render(screen, entity, size, upscale);

    SDL_RenderPresent(renderer);
}


void game_render(Screen *screen, Entity entity[], int size, int upscale)
{
    SDL_Renderer *renderer = screen->renderer;
    
    for (int i = 0; i < size; i++)
    {
        SDL_Rect clip = entity[i].sprite->rect;
        SDL_Rect position;
        position.x = entity[i].rect.x;
        position.y = entity[i].rect.y;
        position.h = entity[i].sprite->rect.h * upscale;
        position.w = entity[i].sprite->rect.w * upscale;

        SDL_Texture *texture = entity[i].sprite->sheet->texture;

        SDL_RenderCopy(renderer, texture, &clip, &position);
    }

}
