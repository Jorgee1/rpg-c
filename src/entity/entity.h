#include "sprite.h"

#ifndef ENTITY_H
#define ENTITY_H

enum PLAYER_STATES
{
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_STATE_TOTAL
};

enum PLAYER_FACES
{
    PLAYER_FRONT,
    PLAYER_RIGHT,
    PLAYER_LEFT,
    PLAYER_BACK,
    PLAYER_FACES_TOTAL
};

typedef struct
{
	SDL_Rect rect;
	SDL_Point speed;
	SDL_Point max_speed;
	int direction;
	int state;

	int animation_index;
	int animation_acc; // For controlling speed

	Sprite *sprite;
} Entity;


#endif


