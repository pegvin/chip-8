#include <stdint.h>
#include <stdbool.h>
#include "win.h"
#include "system.h"

bool isRunning = true;
void ProcessEvents();

int main(void) {
	if (InitWindow() != 0) return 1;

	{
		// 1-bit palette: lospec.com/palette-list/1bit-monitor-glow
		uint8_t on_color[3] = { 0xf0, 0xf6, 0xf0 };
		uint8_t off_color[3] = { 0x22, 0x23, 0x23 };
		SetDisplayTheme(on_color, off_color);
	}

	chip8 sys;
	sys_init(&sys);

	UpdateWindowPixels(sys.display, NULL);

	while (isRunning) {
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			switch (e.type) {
				case SDL_QUIT: isRunning = false; break;
				case SDL_WINDOWEVENT:
					if (e.window.event == SDL_WINDOWEVENT_CLOSE) isRunning = false;
					break;
				case SDL_KEYUP:
				case SDL_KEYDOWN: {
					switch(e.key.keysym.sym) {
						case SDLK_1: { sys_setkeydown(&sys, 0x1, e.type == SDL_KEYDOWN); break; }
						case SDLK_2: { sys_setkeydown(&sys, 0x2, e.type == SDL_KEYDOWN); break; }
						case SDLK_3: { sys_setkeydown(&sys, 0x3, e.type == SDL_KEYDOWN); break; }
						case SDLK_4: { sys_setkeydown(&sys, 0x4, e.type == SDL_KEYDOWN); break; }
						case SDLK_5: { sys_setkeydown(&sys, 0x5, e.type == SDL_KEYDOWN); break; }
						case SDLK_6: { sys_setkeydown(&sys, 0x6, e.type == SDL_KEYDOWN); break; }
						case SDLK_7: { sys_setkeydown(&sys, 0x7, e.type == SDL_KEYDOWN); break; }
						case SDLK_8: { sys_setkeydown(&sys, 0x8, e.type == SDL_KEYDOWN); break; }
						case SDLK_9: { sys_setkeydown(&sys, 0x9, e.type == SDL_KEYDOWN); break; }
						case SDLK_0: { sys_setkeydown(&sys, 0x0, e.type == SDL_KEYDOWN); break; }
						case SDLK_q: { sys_setkeydown(&sys, 0xA, e.type == SDL_KEYDOWN); break; }
						case SDLK_w: { sys_setkeydown(&sys, 0xB, e.type == SDL_KEYDOWN); break; }
						case SDLK_e: { sys_setkeydown(&sys, 0xC, e.type == SDL_KEYDOWN); break; }
						case SDLK_r: { sys_setkeydown(&sys, 0xD, e.type == SDL_KEYDOWN); break; }
						case SDLK_t: { sys_setkeydown(&sys, 0xE, e.type == SDL_KEYDOWN); break; }
						case SDLK_y: { sys_setkeydown(&sys, 0xF, e.type == SDL_KEYDOWN); break; }
					}
					break;
				}
			}
		}

		sys_cycle(&sys);
	}

	CloseWindow();
	return 0;
}

