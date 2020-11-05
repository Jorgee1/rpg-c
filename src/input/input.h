/*
 *	File that handles everything related to player input from keyboard.
 *	This interfaces with SDL SCANCODE, documentation for this can be found on:
 *	https://wiki.libsdl.org/SDL_Scancode
 *
 *
 *	The Input structure gets pass arround to other components if player input is needed.
 *
 *	For declaration call the input in the main and declare all the abtributes by casting.
 *	Example:
 *
 *	struct Input input:
 *	input up     = (struct Button) {SDL_Key, 0, 0, 0};
 *	input down   = (struct Button) {SDL_Key, 0, 0, 0};
 *	input left   = (struct Button) {SDL_Key, 0, 0, 0};
 *	input right  = (struct Button) {SDL_Key, 0, 0, 0};
 *	...
 *	
 *	If you need to iterate over the buttons, an array of Button structs can be declared as follows using the above structure:
 *
 *	struct Button* buttons[size] = {
 *		&input.up,
 *		&input.down,
 *		&input.left,
 *		&input.right,
 *		....
 *
 *	}
 *
 *	this is needed by the function update_buttons.
 *
 */

#include <SDL.h>

#ifndef INPUT_H
#define INPUT_H

typedef struct
{
	/*
		Basic structure for the buttons.

		They flags and the state track two diferent type of aciton.
		The first is only checks if the key was pressed, usfull for character movement.

		The scond only picks up when it was pressed, then it resets the flag if they key is hold.
		This is very helpfull for menus.
	*/

	int key;
	int state;
	int action_state;
	int action_flag;
} Button;


typedef struct
{
	Button up;
	Button down;
	Button left;
	Button right;
	Button action;
	Button cancel;
	Button start;
} Input;


// Call this on every loop to update the state of the buttons
void update_buttons(Button* buttons[], int size);

#endif