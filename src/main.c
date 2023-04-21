#include <stdint.h>
#include <stdbool.h>
#include "win.h"
#include "system.h"

bool isRunning = true;

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <path-to-rom>\n", argv[0]);
		return 1;
	}

	if (InitWindow() != 0) return 1;

	{
		// 1-bit palette: lospec.com/palette-list/1bit-monitor-glow
		uint8_t on_color[3] = { 0xf0, 0xf6, 0xf0 };
		uint8_t off_color[3] = { 0x22, 0x23, 0x23 };
		SetDisplayTheme(on_color, off_color);
	}

	chip8 sys;
	sys_init(&sys);
	sys_load_rom(&sys, argv[1]);

	UpdateWindowPixels(sys.display, NULL);

	unsigned int clockStart, clockEnd;
	unsigned int clockFreq = 1000 / 700; // 700 MHz

	while (isRunning) {
		clockStart = SDL_GetTicks();

		sys_cycle(&sys);

		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: isRunning = false; break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_CLOSE) isRunning = false;
					break;
				case SDL_KEYUP:
					default:
						sys_setkeydown(&sys, 0x0);
						break;
				case SDL_KEYDOWN: {
					switch(e.key.keysym.sym) {
						case SDLK_0: { sys_setkeydown(&sys, 0x0); break; }
						case SDLK_1: { sys_setkeydown(&sys, 0x1); break; }
						case SDLK_2: { sys_setkeydown(&sys, 0x2); break; }
						case SDLK_3: { sys_setkeydown(&sys, 0x3); break; }
						case SDLK_4: { sys_setkeydown(&sys, 0x4); break; }
						case SDLK_5: { sys_setkeydown(&sys, 0x5); break; }
						case SDLK_6: { sys_setkeydown(&sys, 0x6); break; }
						case SDLK_7: { sys_setkeydown(&sys, 0x7); break; }
						case SDLK_8: { sys_setkeydown(&sys, 0x8); break; }
						case SDLK_9: { sys_setkeydown(&sys, 0x9); break; }
						case SDLK_q: { sys_setkeydown(&sys, 0xA); break; }
						case SDLK_w: { sys_setkeydown(&sys, 0xB); break; }
						case SDLK_e: { sys_setkeydown(&sys, 0xC); break; }
						case SDLK_r: { sys_setkeydown(&sys, 0xD); break; }
						case SDLK_t: { sys_setkeydown(&sys, 0xE); break; }
						case SDLK_y: { sys_setkeydown(&sys, 0xF); break; }
					}
					break;
				}
			}
		}

		UpdateWindowPixels(sys.display, NULL);

		clockEnd = SDL_GetTicks() - clockStart;
		if (clockFreq > clockEnd) SDL_Delay(clockFreq - clockEnd);
		clockStart = SDL_GetTicks();
	}

	CloseWindow();
	return 0;
}

