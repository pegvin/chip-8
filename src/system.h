#ifndef _CHIP8_SYSTEM_H_INCLUDED
#define _CHIP8_SYSTEM_H_INCLUDED 1
#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32
#define SPRITE_WIDTH 8
#define FONTSET_SIZE 80
#define NUM_OF_REGISTERS 16
#define MEMORY_SIZE 4096
#define MEMORY_SIZE_AVAILABLE 4096 - 0x200
#define STACK_SIZE 16

typedef struct {
	uint16_t opcode;
	uint8_t  ram[MEMORY_SIZE];
	uint8_t  registers[NUM_OF_REGISTERS];
	uint16_t I;
	uint16_t pctr;
	uint16_t stack[STACK_SIZE];
	uint16_t sptr;
	uint8_t  delayTimer;
	uint8_t  soundTimer;
	uint8_t  key;
	uint8_t  display[DISPLAY_WIDTH * DISPLAY_HEIGHT];
} chip8;

void sys_init(chip8* sys);
void sys_load_rom(chip8* s, const char* romPath);
void sys_pctr_increment(chip8* sys);
void sys_cycle(chip8* s);
void sys_setkeydown(chip8* s, uint8_t key);

#endif // _CHIP8_SYSTEM_H_INCLUDED

