#include <stdio.h>
#include <string.h>
#include <time.h>

#include "log.h"
#include "system.h"
#include "win.h"

static const uint8_t fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		 // F
};

void sys_init(chip8* sys) {
	memset(sys->ram, 0, MEMORY_SIZE);
	memcpy(sys->ram + FONTSET_ADDR, fontset, FONTSET_SIZE);
	memset(sys->registers, 0, NUM_OF_REGISTERS * sizeof(sys->registers[0]));
	memset(sys->stack, 0, STACK_SIZE * sizeof(sys->stack[0]));
	memset(sys->display, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT);
	sys->pctr = ROM_START;
	sys->opcode = 0;
	sys->I = 0;
	sys->key = 0;
	sys->sptr = 0;
	sys->dt = 0;
	sys->st = 0;
}

void sys_load_rom(chip8* s, const char* romPath) {
	FILE* fp = fopen(romPath, "rb");
	if (fp == NULL) {
		LOG_E("failed to load rom: %s", romPath);
		return;
	}

	fseek(fp, 0L, SEEK_END);
	uint32_t sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	if (sz >= MEMORY_SIZE_AVAILABLE) {
		LOG_E("failed to load ROM, available memory is %d bytes & rom size is %d bytes", MEMORY_SIZE_AVAILABLE, sz);
	}

	fread(s->ram + ROM_START, sz, 1, fp);
	fclose(fp);
	fp = NULL;
}

void sys_pctr_increment(chip8* sys) {
	sys->pctr += 2; // since every opcode is 2 bytes while we have ability to access 1 byte.
}

void sys_setkeydown(chip8* s, uint8_t key) {
	s->key = key;
}

void sys_printstate(chip8* s) {
	printf("\nOPCODE: 0x%X\n", s->opcode);
	printf("I: 0x%X\n", s->I);
	printf("Program Counter: 0x%X\n", s->pctr);
	printf("Stack Pointer: 0x%X\n", s->sptr);
	printf("Delay Timer: %d\n", s->dt);
	printf("Sound Timer: %d\n", s->st);
	printf("Key: 0x%X\n", s->key);

	printf("Registers:\n");
	for (uint8_t vx = 0; vx < NUM_OF_REGISTERS; ++vx) printf("  V%X: 0x%X\n", vx, s->registers[vx]);

	printf("\nStack:\n");
	for (uint8_t sp = 0; sp < STACK_SIZE; ++sp) printf("  stack[%d]: 0x%X\n", sp, s->stack[sp]);
}

void sys_cycle(chip8* s) {
	if (s->dt > 0) s->dt--;
	if (s->st > 0) s->st--;

	s->opcode = s->ram[s->pctr] << 8 | s->ram[s->pctr + 1]; // fetch next instruction.
	sys_pctr_increment(s);

	uint8_t identifier = s->opcode >> 12; // Get "A" from opcode "ABCD"
	switch (identifier) {
		case 0x0: {
			if (s->opcode == 0x00E0) { // Clear: Clear The Display
				memset(s->display, 0, 64 * 32);
			} else if (s->opcode == 0x00EE) { // Return from a subroutine: The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
				if (s->sptr > 0) s->sptr--;
				s->pctr = s->stack[s->sptr];
			} else { // 0x0nnn - Jump to a machine code routine at nnn:  This instruction is only used on the old computers on which Chip-8 was originally implemented. It is ignored by modern interpreters.
			}
			break;
		}
		case 0x1: { // Jump to location nnn.
			uint16_t jmpAddr = s->opcode & 0x0FFF;
			s->pctr = jmpAddr;
			break;
		}
		case 0x2: { // Call subroutine at nnn.
			uint16_t subrtnAddr = s->opcode & 0x0FFF;
			s->stack[s->sptr] = s->pctr;
			s->sptr++;
			s->pctr = subrtnAddr;
			break;
		}
		case 0x3: { // Skip next instruction if Vx == kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			if (s->registers[Vx] == KK) {
				sys_pctr_increment(s);
			}
			break;
		}
		case 0x4: { // Skip next instruction if Vx != kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			if (s->registers[Vx] != KK) {
				sys_pctr_increment(s);
			}
			break;
		}
		case 0x5: { // Skip next instruction if Vx == Vy.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;
			if (s->registers[Vx] == s->registers[Vy]) {
				sys_pctr_increment(s);
			}
			break;
		}
		case 0x6: { // Set Vx = kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			s->registers[Vx] = KK;
			break;
		}
		case 0x7: { // Set Vx = Vx + kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			s->registers[Vx] += KK;
			break;
		}
		case 0x8: {
			uint8_t code = s->opcode & 0x000F;
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;
			if (code == 0x0) { // Set Vx = Vy.
				s->registers[Vx] = s->registers[Vy];
			} else if (code == 0x1) { // Set Vx = Vx OR Vy.
				s->registers[Vx] |= s->registers[Vy];
			} else if (code == 0x2) { // Set Vx = Vx AND Vy.
				s->registers[Vx] &= s->registers[Vy];
			} else if (code == 0x3) { // Set Vx = Vx XOR Vy.
				s->registers[Vx] ^= s->registers[Vy];
			} else if (code == 0x4) { // Set Vx = Vx + Vy, set VF = carry.
				s->registers[0xF] = (s->registers[Vx] + s->registers[Vy]) > 255;
				s->registers[Vx] += s->registers[Vy];
			} else if (code == 0x5) { // Set Vx = Vx - Vy, set VF = NOT borrow.
				s->registers[0xF] = (s->registers[Vx] > s->registers[Vy]);
				s->registers[Vx] -= s->registers[Vy];
			} else if (code == 0x6) { // Set Vx = Vx SHR 1.
				s->registers[0xF] = s->registers[Vx] & 0x1;
				s->registers[Vx] >>= 1;
			} else if (code == 0x7) { // Set Vx = Vy - Vx, set VF = NOT borrow.
				s->registers[0xF] = (s->registers[Vy] > s->registers[Vx]);
				s->registers[Vx] = s->registers[Vy] - s->registers[Vx];
			} else if (code == 0xE) { // Set Vx = Vx SHL 1.
				s->registers[0xF] = (s->registers[Vx] & 0x80) >> 7;
				s->registers[Vx] <<= 1;
			} else {
				LOG_E("Un-reachable Section reached!");
				sys_printstate(s);
				exit(1);
			}
			break;
		}
		case 0x9: { // Skip next instruction if Vx != Vy.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;

			if (s->registers[Vx] != s->registers[Vy]) {
				sys_pctr_increment(s);
			}
			break;
		}
		case 0xA: {
			uint16_t addr = (s->opcode & 0x0FFF);
			s->I = addr;
			break;
		}
		case 0xB: { // Jump to location nnn + V0.
			uint16_t jmpLocation = (s->opcode & 0x0FFF) + s->registers[0x0];
			s->pctr = jmpLocation;
			break;
		}
		case 0xC: { // Set Vx = random byte AND kk.
			srand(time(NULL));
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			s->registers[Vx] = (rand() % (255 + 1 - 0) + 0) & KK; // It was simple math to convert the random number between a range: https://stackoverflow.com/a/1202706/14516016
			break;
		}
		case 0xD: { // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;
			uint8_t height = s->opcode & 0x000F; // this is the height of our sprite.

			uint8_t xPos = s->registers[Vx] % (DISPLAY_WIDTH + 1); // wraps around our sum, so 64 + 2 = 66 but since we only have a screen size of 64, 66 % 64 = 2.
			uint8_t yPos = s->registers[Vy] % (DISPLAY_HEIGHT + 1); // same wrapping as above

			s->registers[0xF] = 0;

			for (uint16_t y = 0; y < height; ++y) {
				if ((s->I + y) >= MEMORY_SIZE) {
					LOG_E("memory access out of bound: %d !!!", (s->I + y));
					sys_printstate(s);
					exit(1);
				}
				uint8_t spriteByte = s->ram[s->I + y];
				for (uint16_t x = 0; x < SPRITE_WIDTH; ++x) {
					bool isPixelOn = (spriteByte & (0x80 >> x)) != 0;
					if (isPixelOn) {
						uint16_t pX = xPos + x;
						uint16_t pY = yPos + y;
						uint32_t idx = pY * DISPLAY_WIDTH + pX;

						if (s->display[idx] == 1) s->registers[0xF] = 1;
						s->display[idx] ^= 1;
					}
				}
			}
			break;
		}
		case 0xE: {
			uint8_t code = (s->opcode & 0x00FF);
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			if (code == 0x9E) { // Skip next instruction if key with the value of Vx is pressed.
				if (s->key == s->registers[Vx]) {
					sys_pctr_increment(s);
				}
			} else if (code == 0xA1) { // Skip next instruction if key with the value of Vx is not pressed.
				if (s->key != s->registers[Vx]) {
					sys_pctr_increment(s);
				}
			} else {
				LOG_E("Un-reachable Section reached!");
				sys_printstate(s);
				exit(1);
			}
			break;
		}
		case 0xF: {
			uint8_t code = (s->opcode & 0x00FF);
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			if (code == 0x07) { // Set Vx = delay timer value.
				s->registers[Vx] = s->dt;
			} else if (code == 0x0A) { // Wait for a key press, store the value of the key in Vx.
				if (s->key == 0x0) {
					return; // return-ing out of the function is important else pctr will be incremented and CPU will run the next instruction.
				} else {
					s->registers[Vx] = s->key;
				}
			} else if (code == 0x15) { // Set delay timer = Vx.
				s->dt = s->registers[Vx];
			} else if (code == 0x18) { // Set sound timer = Vx.
				s->st = s->registers[Vx];
			} else if (code == 0x1E) { // Set I = I + Vx.
				s->I += s->registers[Vx];
			} else if (code == 0x29) { // Set I = location of sprite for digit Vx.
				s->I = FONTSET_ADDR + (s->registers[Vx] * 5); // since each sprite consists of 5 bytes.
			} else if (code == 0x33) { // Store BCD representation of Vx in memory locations I, I+1, and I+2.
				s->ram[s->I + 0] = s->registers[Vx] / 100;
				s->ram[s->I + 1] = (s->registers[Vx] / 10) % 10;
				s->ram[s->I + 2] = s->registers[Vx] % 10;
			} else if (code == 0x55) { // Store registers V0 through Vx in memory starting at location I.
				for (uint8_t i = 0; i <= Vx; ++i) {
					s->ram[s->I + i] = s->registers[i];
				}
				s->I += Vx + 1;
			} else if (code == 0x65) { // Read registers V0 through Vx from memory starting at location I.
				for (uint8_t i = 0; i <= Vx; ++i) {
					s->registers[i] = s->ram[s->I + i];
				}
				s->I += Vx + 1;
			}
			break;
		}
	}
}

