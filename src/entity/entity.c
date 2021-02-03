#include "entity.h"


void load_entity(Entity *entity, SDL_Rect rect, Sprite *sprite, int speed, int state, int direction)
{
    entity->rect  = rect;
    entity->speed = (SDL_Point) {0, 0};
    entity->max_speed = (SDL_Point) {speed, speed};

    entity->state = state;
    entity->direction = direction;
    
    entity->sprite = sprite;
    entity->animation_index = 0;
    entity->animation_acc = 0;
}