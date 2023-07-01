#ifndef _CHIP8_WINDOW_H_INCLUDED
#define _CHIP8_WINDOW_H_INCLUDED 1

#include <SDL.h>
#include <stdint.h>

#include "system.h"

#define EMU_DISPLAY_SCALE 8

int  InitWindow(); // creates a window with 64x32 scaled 6x
void CloseWindow(); // destroys all the memory allocated to create & used by window

/*
	update the chip 8 display, takes a uint8_t array 64 * 32 long
	with each element being either non-zero (true) or zero (false)
	representing a monchrome display.

	dirtyArea holds the area changed in the display so that only
	the changed part is updated, which is more efficient.
	leave it to NULL to update whole display.
*/
void UpdateWindowPixels(uint8_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT * 3]);

#endif // _CHIP8_WINDOW_H_INCLUDED

