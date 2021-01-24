#include "game.h"

void game_update_input(Input *input, Entity *player)
{
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

        // animation frame update
        if (entity[i].animation_acc > animations[state][direction].speed)
        {
            int index = (entity[i].animation_index + 1) % animations[state][direction].n;
            entity[i].animation_acc = 0;
            entity[i].animation_index = index;
        }

        // To prevent overflows
        if (entity[i].animation_index >= animations[state][direction].n)
        {
            entity[i].animation_index = 0;
            entity[i].animation_acc = 0;
        }
        // Current sprite pointer update
        entity[i].sprite = &(animations[state][direction].sprites[entity[i].animation_index]);
    }
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
