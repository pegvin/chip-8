#include <stdbool.h>
#include <SDL2/SDL.h>
#include "front.h"

bool isRunning = true;
float Scale_X = 1.0f;
float Scale_Y = 1.0f;
float FontScale = 1.0f;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
nkctx_t* nkCtx = NULL;

void ProcessEvents();

int main(void) {
	if (InitChip8Front(&window, &renderer, &Scale_X, &Scale_Y, &FontScale, &nkCtx) != 0) {
		return 1;
	}

	while (isRunning) {
		Chip8Front_NewFrame();
		ProcessEvents();

		if (nk_begin(nkCtx, "Demo", nk_rect(50, 50, 230, 250), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
			enum { EASY, HARD };
			static int op = EASY;
			static int property = 20;

			nk_layout_row_static(nkCtx, 30, 80, 1);
			if (nk_button_label(nkCtx, "button"))
				fprintf(stdout, "button pressed\n");
			nk_layout_row_dynamic(nkCtx, 30, 2);
			if (nk_option_label(nkCtx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(nkCtx, "hard", op == HARD)) op = HARD;
			nk_layout_row_dynamic(nkCtx, 25, 1);
			nk_property_int(nkCtx, "Compression:", 0, &property, 100, 10, 1);
		}
		nk_end(nkCtx);
		Chip8Front_RenderToScreen(renderer);
	}

	QuitChip8Front(window, renderer, nkCtx);
	nkCtx = NULL;
	renderer = NULL;
	window = NULL;
	return 0;
}

void ProcessEvents() {
	SDL_Event e;
	nk_input_begin(nkCtx);
	while (SDL_PollEvent(&e)) {
		nk_sdl_handle_event(&e);
		switch (e.type) {
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_WINDOWEVENT:
				if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
					isRunning = false;
				}
				break;
		}
	}
	nk_input_end(nkCtx);
}

