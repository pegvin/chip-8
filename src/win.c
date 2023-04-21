#include "win.h"
#include "log.h"

static SDL_Window* window = NULL;

static SDL_Surface* emuDisplaySurf = NULL;
static uint8_t  pixelOn[3]; // Pixel ON Color
static uint8_t  pixelOff[3]; // Pixel OFF Color

int InitWindow() {
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

void CloseWindow() {
	if (window == NULL) return;
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void UpdateWindowPixels(uint8_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT * 3]) {
	SDL_LockSurface(emuDisplaySurf);
	for (uint16_t y = 0; y < DISPLAY_HEIGHT; ++y) {
		for (uint16_t x = 0; x < DISPLAY_WIDTH; ++x) {
			memcpy(
				emuDisplaySurf->pixels + (((y * DISPLAY_WIDTH) + x) * 3),
				pixels[(y * DISPLAY_WIDTH) + x] ? pixelOn : pixelOff,
				3
			);
		}
	}
	SDL_UnlockSurface(emuDisplaySurf);
	SDL_BlitScaled(emuDisplaySurf, NULL, SDL_GetWindowSurface(window), NULL);
	SDL_UpdateWindowSurface(window);
}

void SetDisplayTheme(uint8_t on_color[3], uint8_t off_color[3]) {
	if (on_color != NULL) memcpy(pixelOn, on_color, 3);
	if (off_color != NULL) memcpy(pixelOff, off_color, 3);
}

