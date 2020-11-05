#include "input.h"


void update_buttons(Button* buttons[], int size)
{
	const uint8_t* key_state = SDL_GetKeyboardState(NULL);

	for(int i = 0; i < size; i++)
	{
		buttons[i]->state = key_state[buttons[i]->key];

		if ((!buttons[i]->action_flag) && (key_state[buttons[i]->key]))
		{
			buttons[i]->action_state = 1;
			buttons[i]->action_flag  = 1;
		}
		else if ((buttons[i]->action_flag) && (!key_state[buttons[i]->key]))
		{
			buttons[i]->action_state = 0;
			buttons[i]->action_flag  = 0;
		}
		else
		{
			buttons[i]->action_state = 0;
		}
	}
}