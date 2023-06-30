#ifndef _CHIP8_SYSTEM_H_INCLUDED
#define _CHIP8_SYSTEM_H_INCLUDED 1
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

#define SPRITE_WIDTH 8
#define FONTSET_SIZE 80

#define NUM_OF_KEYS 16
#define NUM_OF_REGISTERS 16
#define STACK_SIZE 16

#define ROM_START 0x200
#define MEMORY_SIZE 4096
#define MEMORY_SIZE_AVAILABLE MEMORY_SIZE - ROM_START
#define FONTSET_ADDR 0x0

typedef struct {
	uint16_t opcode;
	uint8_t  ram[MEMORY_SIZE];
	uint8_t  registers[NUM_OF_REGISTERS];
	uint16_t I;
	uint16_t pctr;
	uint16_t stack[STACK_SIZE];
	uint8_t  sptr;
	uint8_t  dt;  // delay timer
	uint8_t  st;  // sound timer
	bool     keys[NUM_OF_KEYS]; // key being held down
	uint8_t  display[DISPLAY_WIDTH * DISPLAY_HEIGHT];

	// MISC - stuff not related to actual emulation.
	uint8_t  displayRGB[DISPLAY_WIDTH * DISPLAY_HEIGHT * 3]; // same as display[] but with RGB8 colors
	uint8_t  pixelOn[3];  // Color to use when display pixel is on
	uint8_t  pixelOff[3]; // Color to use when display pixel is off
} chip8;

void sys_init(chip8* sys, uint8_t displayColorOn[3], uint8_t displayColorOff[3]);
void sys_load_rom(chip8* s, const char* romPath);
void sys_pctr_increment(chip8* sys);
void sys_cycle(chip8* s);
void sys_setkeydown(chip8* s, uint8_t key, bool isDown);

// sets all the pixels to OFF.
void sys_ResetDisplay(chip8* s);

// changes chip8.pixelOn & chip8.pixelOff colors and updates them in displayRGB.
void sys_SetDisplayTheme(chip8* s, uint8_t on_color[3], uint8_t off_color[3]);

#endif // _CHIP8_SYSTEM_H_INCLUDED

