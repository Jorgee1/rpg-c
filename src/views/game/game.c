#include "game.h"

void update_player_input(Input *input, Entity *player)
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