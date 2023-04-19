#include <string.h>
#include <time.h>

#include "system.h"

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
	memset(sys->ram, 0, 4096);
	memcpy(sys->ram, fontset, FONTSET_SIZE); // load font in starting of our ram
	memset(sys->registers, 0, 16 * sizeof(sys->registers[0]));
	memset(sys->stack, 0, 16 * sizeof(sys->stack[0]));
	memset(sys->keys, 0, 16 * sizeof(sys->keys[0]));
	sys->pctr = 0x200;
	sys->opcode = 0;
	sys->I = 0;
	sys->sptr = 0;
	sys->delayTimer = 0;
	sys->soundTimer = 0;
}

void sys_pctr_increment(chip8* sys) {
	sys->pctr += 2; // since every opcode is 2 bytes while we have ability to access 1 byte.
}

void sys_setkeydown(chip8* s, uint8_t key, bool isDown) {
	assert(key <= 0xF && "Un-reachable section reached, Key is greater than 0xF");
	s->keys[key] = isDown;
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
		case 0x3: { //  Skip next instruction if Vx == kk.
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
				assert(false && "Un-reachable Section reached!");
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


		case 0xE: {
			uint8_t code = (s->opcode & 0x00FF);
			uint8_t Vx = (s->opcode & 0x0F00) >> 8;
			uint8_t keyIdx = s->registers[Vx];
			if (code == 0x9E) {
				if (s->keys[keyIdx] == true) {
					sys_pctr_increment(s);
				}
			} else if (code == 0xA1) {
				if (s->keys[keyIdx] == false) {
					sys_pctr_increment(s);
				}
			} else {
				assert(false && "Un-reachable area reached!!!");
			}
			sys_pctr_increment(s);
			break;
		}
		// case 0x: {
		// 	sys_pctr_increment(s);
		// 	break;
		// }
	}
}

