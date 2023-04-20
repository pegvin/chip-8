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
	memcpy(sys->ram, fontset, FONTSET_SIZE); // load font in starting of our ram
	memset(sys->registers, 0, 16 * sizeof(sys->registers[0]));
	memset(sys->stack, 0, 16 * sizeof(sys->stack[0]));
	memset(sys->display, 0, 64 * 32);
	sys->pctr = 0x200;
	sys->opcode = 0;
	sys->I = 0;
	sys->key = 0;
	sys->sptr = 0;
	sys->delayTimer = 0;
	sys->soundTimer = 0;
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

	fread(s->ram + 0x200, sz, 1, fp);
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
	printf("\nOPCODE: 0x%04X\n", s->opcode);
	printf("I: 0x%04X\n", s->I);
	printf("Program Counter: 0x%04X\n", s->pctr);
	printf("Stack Pointer: 0x%04X\n", s->sptr);
	printf("Delay Timer: %d\n", s->delayTimer);
	printf("Sound Timer: %d\n", s->soundTimer);
	printf("Key: 0x%02X\n", s->key);

	printf("Registers:\n");
	for (uint8_t vx = 0; vx < NUM_OF_REGISTERS; ++vx) printf("  V%02x: 0x%02X\n", vx, s->registers[vx]);

	printf("\nStack:\n");
	for (uint8_t sp = 0; sp < STACK_SIZE; ++sp) printf("  stack[%d]: 0x%04X\n", sp, s->stack[sp]);
}

void sys_cycle(chip8* s) {
	s->opcode = s->ram[s->pctr] << 8 | s->ram[s->pctr + 1]; // fetch next instruction.

	uint8_t identifier = s->opcode >> 12; // Get "A" from opcode "ABCD"
	switch (identifier) {
		case 0x0: {
			if (s->opcode == 0x00E0) { // Clear: Clear The Display
				memset(s->display, 0, 64 * 32);
			} else if (s->opcode == 0x00EE) { // Return from a subroutine: The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
				s->pctr = s->stack[s->sptr];
				s->sptr--;
			} else { // 0x0nnn - Jump to a machine code routine at nnn:  This instruction is only used on the old computers on which Chip-8 was originally implemented. It is ignored by modern interpreters.
			}
			sys_pctr_increment(s);
			break;
		}
		case 0x1: { // Jump to location nnn.
			uint16_t jmpAddr = s->opcode & 0x0FFF;
			s->pctr = jmpAddr;
			sys_pctr_increment(s);
			break;
		}
		case 0x2: { // Call subroutine at nnn.
			s->sptr++;
			s->stack[s->sptr] = s->pctr;
			uint16_t subrtnAddr = s->opcode & 0x0FFF;
			s->pctr = subrtnAddr;
			sys_pctr_increment(s);
			break;
		}
		case 0x3: { // Skip next instruction if Vx == kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			if (s->registers[Vx] == KK) {
				sys_pctr_increment(s);
			}
			sys_pctr_increment(s);
			break;
		}
		case 0x4: { // Skip next instruction if Vx != kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			if (s->registers[Vx] != KK) {
				sys_pctr_increment(s);
			}
			sys_pctr_increment(s);
			break;
		}
		case 0x5: { // Skip next instruction if Vx == Vy.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;
			if (s->registers[Vx] == s->registers[Vy]) {
				sys_pctr_increment(s);
			}
			sys_pctr_increment(s);
			break;
		}
		case 0x6: { // Set Vx = kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			s->registers[Vx] = KK;
			sys_pctr_increment(s);
			break;
		}
		case 0x7: { // Set Vx = Vx + kk.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			s->registers[Vx] += KK;
			sys_pctr_increment(s);
			break;
		}
		case 0x8: {
			uint8_t code = s->opcode & 0x000F;
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;
			if (code == 0) { // Set Vx = Vy.
				s->registers[Vx] = s->registers[Vy];
			} else if (code == 1) { // Set Vx = Vx OR Vy.
				s->registers[Vx] |= s->registers[Vy];
			} else if (code == 2) { // Set Vx = Vx AND Vy.
				s->registers[Vx] &= s->registers[Vy];
			} else if (code == 3) { // Set Vx = Vx XOR Vy.
				s->registers[Vx] ^= s->registers[Vy];
			} else if (code == 4) { // Set Vx = Vx + Vy, set VF = carry.
				if ((s->registers[Vx] + s->registers[Vy]) > 255) {
					s->registers[0xF] = 1;
				} else {
					s->registers[0xF] = 0;
				}
				s->registers[Vx] += s->registers[Vy];
			} else if (code == 5) { // Set Vx = Vx - Vy, set VF = NOT borrow.
				s->registers[0xF] = (s->registers[Vx] > s->registers[Vy]);
				s->registers[Vx] -= s->registers[Vy];
			} else if (code == 6) { // Set Vx = Vx SHR 1.
				s->registers[0xF] = s->registers[Vx] & 1;
				s->registers[Vx] = s->registers[Vx] >> 1;
			} else if (code == 7) { // Set Vx = Vy - Vx, set VF = NOT borrow.
				s->registers[0xF] = (s->registers[Vy] > s->registers[Vx]);
				s->registers[Vx] = s->registers[Vy] - s->registers[Vx];
			} else if (code == 0xE) { // Set Vx = Vx SHL 1.
				s->registers[0xF] = (s->registers[Vx] & 0x80) != 0;
				s->registers[Vx] = s->registers[Vx] << 1;
			} else {
				LOG_E("Un-reachable Section reached!");
				sys_printstate(s);
				exit(1);
			}
			sys_pctr_increment(s);
			break;
		}
		case 0x9: { // Skip next instruction if Vx != Vy.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;

			if (s->registers[Vx] != s->registers[Vy]) {
				sys_pctr_increment(s);
			}

			sys_pctr_increment(s);
			break;
		}
		case 0xA: {
			uint16_t addr = (s->opcode & 0x0FFF);
			s->I = addr;
			sys_pctr_increment(s);
			break;
		}
		case 0xB: { // Jump to location nnn + V0.
			uint16_t jmpLocation = (s->opcode & 0x0FFF) + s->registers[0x0];
			s->pctr = jmpLocation;
			sys_pctr_increment(s);
			break;
		}
		case 0xC: { // Set Vx = random byte AND kk.
			srand(time(NULL));
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t KK = s->opcode & 0x00FF;
			s->registers[Vx] = (rand() % (255 + 1 - 0) + 0) & KK; // It was simple math to convert the random number between a range: https://stackoverflow.com/a/1202706/14516016
			sys_pctr_increment(s);
			break;
		}
		case 0xD: { // Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t Vy = (s->opcode & 0x00F0) >> 4;
			uint8_t height = s->opcode & 0x000F; // this is the height of our sprite.

			uint8_t rX = s->registers[Vx];
			uint8_t rY = s->registers[Vy];

			s->registers[0xF] = 0;

			for (uint16_t y = 0; y < height; ++y) {
				if ((s->I + y) >= MEMORY_SIZE) {
					LOG_E("memory access out of bound: %d !!!", (s->I + y));
					sys_printstate(s);
					exit(1);
				}
				uint8_t pixelRow = s->ram[s->I + y];
				for (uint16_t x = 0; x < SPRITE_WIDTH; ++x) {
					if ((pixelRow & 0x80) > 0) {
						uint8_t pX = (rX % DISPLAY_WIDTH) + x; // wraps around our sum, so 64 + 2 = 66 but since we only have a screen size of 64, 66 % 64 = 2.
						uint8_t pY = (rY % DISPLAY_HEIGHT) + y; // same wrapping as above
						if (pX > DISPLAY_WIDTH || pX < 0 || pY > DISPLAY_HEIGHT || pY < 0) break;

						uint8_t idx = (pY * DISPLAY_WIDTH + pX);

						s->display[idx] ^= 1; // if pixel at idx was 1 then XOR will set it to 0
						if (s->display[idx] == 0) {
							s->registers[0xF] = 1;
						}
					}

					pixelRow <<= 1;
				}
			}
			sys_pctr_increment(s);
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
			sys_pctr_increment(s);
			break;
		}
		case 0xF: {
			uint8_t code = (s->opcode & 0x00FF);
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			if (code == 0x07) { // Set Vx = delay timer value.
				s->registers[Vx] = s->delayTimer;
			} else if (code == 0x0A) { // Wait for a key press, store the value of the key in Vx.
				if (s->key == 0x0) {
					return; // return-ing out of the function is important else pctr will be incremented and CPU will run the next instruction.
				} else {
					s->registers[Vx] = s->key;
				}
			} else if (code == 0x15) { // Set delay timer = Vx.
				s->delayTimer = s->registers[Vx];
			} else if (code == 0x18) { // Set sound timer = Vx.
				s->soundTimer = s->registers[Vx];
			} else if (code == 0x1E) { // Set I = I + Vx.
				s->I += s->registers[Vx];
			} else if (code == 0x29) { // Set I = location of sprite for digit Vx.
				s->I = s->registers[Vx] * 5; // since each sprite consists of 5 bytes.
			} else if (code == 0x33) { // Store BCD representation of Vx in memory locations I, I+1, and I+2.
				s->ram[s->I + 0] = s->registers[Vx] / 100;
				s->ram[s->I + 1] = (s->registers[Vx] / 10) % 10;
				s->ram[s->I + 2] = s->registers[Vx] % 10;
			} else if (code == 0x55) { // Store registers V0 through Vx in memory starting at location I.
				for (uint8_t i = 0; (i < Vx) && (i < NUM_OF_REGISTERS); ++i) {
					s->ram[s->I + i] = s->registers[i];
				}
			} else if (code == 0x65) { // Read registers V0 through Vx from memory starting at location I.
				for (uint8_t i = 0; (i < Vx) && (i < NUM_OF_REGISTERS); ++i) {
					s->registers[i] = s->ram[s->I + i];
				}
			}
			sys_pctr_increment(s);
			break;
		}
	}
	if (s->delayTimer > 0) s->delayTimer--;
	if (s->soundTimer > 0) s->soundTimer--;
}

