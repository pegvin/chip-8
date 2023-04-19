#ifndef _CHIP8_SYSTEM_H_INCLUDED
#define _CHIP8_SYSTEM_H_INCLUDED 1
#pragma once

#include <stdint.h>

#define FONTSET_SIZE 80

typedef struct {
	uint16_t opcode;
	uint8_t  ram[4096];
	uint8_t  registers[16];
	uint16_t I;
	uint16_t pctr;
	uint16_t stack[16];
	uint16_t sptr;
	uint8_t  delayTimer;
	uint8_t  soundTimer;
	bool     keys[16];
	uint8_t  display[64 * 32];
} chip8;

void sys_init(chip8* sys);
void sys_pctr_increment(chip8* sys);
void sys_cycle(chip8* s);
void sys_setkeydown(chip8* s, uint8_t key, bool isDown);

#endif // _CHIP8_SYSTEM_H_INCLUDED

