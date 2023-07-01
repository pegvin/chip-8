#include "win.h"
#include "log.h"

static SDL_Window* window = NULL;
static SDL_Surface* emuDisplaySurf = NULL;

int Win_InitWindow() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		LOG_E("failed to initialize SDL2: %s", SDL_GetError());
		return 1;
	}

	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	LOG_I("compiled With SDL version %u.%u.%u", compiled.major, compiled.minor, compiled.patch);
	LOG_I("linked With SDL version %u.%u.%u", linked.major, linked.minor, linked.patch);

	SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, 0);

	emuDisplaySurf = SDL_CreateRGBSurfaceWithFormat(0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 8 * 3, SDL_PIXELFORMAT_RGB24);

	window = SDL_CreateWindow(
		"Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DISPLAY_WIDTH * EMU_DISPLAY_SCALE, DISPLAY_HEIGHT * EMU_DISPLAY_SCALE, SDL_WINDOW_HIDDEN
	);

	if (window == NULL) {
		LOG_E("failed to create a window: %s", SDL_GetError());
		return 1;
	} else {
		SDL_ShowWindow(window);
	}
	return 0;
}

void Win_CloseWindow() {
	if (emuDisplaySurf) SDL_FreeSurface(emuDisplaySurf);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();

	emuDisplaySurf = NULL;
	window = NULL;
}

void Win_UpdateWindowPixels(uint8_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT * 3]) {
	SDL_LockSurface(emuDisplaySurf);
	SDL_memcpy(emuDisplaySurf->pixels, pixels, DISPLAY_WIDTH * DISPLAY_HEIGHT * 3);
	SDL_UnlockSurface(emuDisplaySurf);

	SDL_BlitScaled(emuDisplaySurf, NULL, SDL_GetWindowSurface(window), NULL);
	SDL_UpdateWindowSurface(window);
}

