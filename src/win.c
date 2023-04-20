#include "win.h"
#include "log.h"

static SDL_Window* window = NULL;

static SDL_Surface* emuDisplaySurf = NULL;
static uint8_t ON_Color[3] = { 0, 0, 255 };
static uint8_t OFF_Color[3] = { 255, 0, 0 };

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

void UpdateWindowPixels(uint8_t pixels[DISPLAY_WIDTH * DISPLAY_HEIGHT], SDL_Rect* _dirtyArea) {
	SDL_Rect dirtyArea = { 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT };
	if (_dirtyArea != NULL) {
		dirtyArea.x = _dirtyArea->x;
		dirtyArea.y = _dirtyArea->y;
		dirtyArea.w = _dirtyArea->w;
		dirtyArea.h = _dirtyArea->h;
	}
	SDL_Rect dirtyAreaScaled = {
		dirtyArea.x * EMU_DISPLAY_SCALE,
		dirtyArea.y * EMU_DISPLAY_SCALE,
		dirtyArea.w * EMU_DISPLAY_SCALE,
		dirtyArea.h * EMU_DISPLAY_SCALE
	};

	SDL_LockSurface(emuDisplaySurf);
	for (uint16_t y = dirtyArea.y; y < dirtyArea.h; ++y) {
		for (uint16_t x = dirtyArea.x; x < dirtyArea.w; ++x) {
			memcpy(
				emuDisplaySurf->pixels + ((y * DISPLAY_WIDTH + x) * 3),
				pixels[y * DISPLAY_WIDTH + x] != 0 ? ON_Color : OFF_Color,
				3
			);
		}
	}
	SDL_UnlockSurface(emuDisplaySurf);

	SDL_BlitScaled(
		emuDisplaySurf,
		&dirtyArea,
		SDL_GetWindowSurface(window),
		&dirtyAreaScaled
	);
	SDL_UpdateWindowSurface(window);
}

void SetDisplayTheme(uint8_t on_color[3], uint8_t off_color[3]) {
	if (on_color != NULL) {
		memcpy(ON_Color, on_color, 3);
	}
	if (off_color != NULL) {
		memcpy(OFF_Color, off_color, 3);
	}
}

