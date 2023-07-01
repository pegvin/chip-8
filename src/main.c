/* INFO:
	- "Display wait" doesn't work:
		Drawing sprites to the display waits for the vertical blank interrupt,
		limiting their speed to max 60 sprites per second, but i don't think i'll implement it.
*/

#include <stdint.h>
#include <stdbool.h>
#include "win.h"
#include "log.h"
#include "system.h"

bool isRunning = true;

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <path-to-rom>\n", argv[0]);
		return 1;
	}

	if (Win_InitWindow() != 0) return 1;

	chip8 sys;
	uint8_t on_color[3] = { 0xb8, 0xc2, 0xb9 };
	uint8_t off_color[3] = { 0x22, 0x23, 0x23 };
	sys_init(&sys, on_color, off_color);
	sys_load_rom(&sys, argv[1]);

	unsigned int clockStart, clockEnd;
	unsigned int clockFreq = 1000 / 700; // 700Hz

	while (isRunning) {
		clockStart = SDL_GetTicks();

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: isRunning = false; break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_CLOSE) isRunning = false;
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN: {
					/*      CHIP-8 KEYMAP
						Keypad       Keyboard
						+-+-+-+-+    +-+-+-+-+
						|1|2|3|C|    |1|2|3|4|
						+-+-+-+-+    +-+-+-+-+
						|4|5|6|D|    |Q|W|E|R|
						+-+-+-+-+ => +-+-+-+-+
						|7|8|9|E|    |A|S|D|F|
						+-+-+-+-+    +-+-+-+-+
						|A|0|B|F|    |Z|X|C|V|
						+-+-+-+-+    +-+-+-+-+ */
					switch(e.key.keysym.sym) {
						case SDLK_1: { sys_setkeydown(&sys, 0x1, e.type == SDL_KEYDOWN); break; }
						case SDLK_2: { sys_setkeydown(&sys, 0x2, e.type == SDL_KEYDOWN); break; }
						case SDLK_3: { sys_setkeydown(&sys, 0x3, e.type == SDL_KEYDOWN); break; }
						case SDLK_4: { sys_setkeydown(&sys, 0xC, e.type == SDL_KEYDOWN); break; }

						case SDLK_q: { sys_setkeydown(&sys, 0x4, e.type == SDL_KEYDOWN); break; }
						case SDLK_w: { sys_setkeydown(&sys, 0x5, e.type == SDL_KEYDOWN); break; }
						case SDLK_e: { sys_setkeydown(&sys, 0x6, e.type == SDL_KEYDOWN); break; }
						case SDLK_r: { sys_setkeydown(&sys, 0xD, e.type == SDL_KEYDOWN); break; }

						case SDLK_a: { sys_setkeydown(&sys, 0x7, e.type == SDL_KEYDOWN); break; }
						case SDLK_s: { sys_setkeydown(&sys, 0x8, e.type == SDL_KEYDOWN); break; }
						case SDLK_d: { sys_setkeydown(&sys, 0x9, e.type == SDL_KEYDOWN); break; }
						case SDLK_f: { sys_setkeydown(&sys, 0xE, e.type == SDL_KEYDOWN); break; }

						case SDLK_z: { sys_setkeydown(&sys, 0xA, e.type == SDL_KEYDOWN); break; }
						case SDLK_x: { sys_setkeydown(&sys, 0x0, e.type == SDL_KEYDOWN); break; }
						case SDLK_c: { sys_setkeydown(&sys, 0xB, e.type == SDL_KEYDOWN); break; }
						case SDLK_v: { sys_setkeydown(&sys, 0xF, e.type == SDL_KEYDOWN); break; }
					}
					break;
				}
			}
		}

		sys_cycle(&sys);
		Win_UpdateWindowPixels(sys.displayRGB);

		clockEnd = SDL_GetTicks() - clockStart;
		if (clockFreq > clockEnd) SDL_Delay(clockFreq - clockEnd);
		clockStart = SDL_GetTicks();
	}

	Win_CloseWindow();
	return 0;
}

