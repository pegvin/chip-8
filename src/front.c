#include "front.h"
#include "third_party/log/log.h"

int InitChip8Front(SDL_Window** _win, SDL_Renderer** _ren, float* scaleX, float* scaleY, float* fontScale, nkctx_t** _ctx) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
		log_error("failed to initialize SDL2: %s", SDL_GetError());
		return 1;
	}

	SDL_version compiled;
	SDL_version linked;
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	log_info("Compiled With SDL version %u.%u.%u", compiled.major, compiled.minor, compiled.patch);
	log_info("Linked With SDL version %u.%u.%u", linked.major, linked.minor, linked.patch);

	SDL_Window* window = SDL_CreateWindow("Chip-8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_MAXIMIZED);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		log_error("Failed to create SDL_Renderer, SDL_CreateRenderer() returned NULL!");
		return 1;
	}

	nkctx_t* nkCtx = nk_sdl_init(window, renderer);
	if (nkCtx == NULL) {
		log_error("Unable to initialize Nuklear Context!");
		return 1;
	} else {
		log_info("Nuklear Context Initialized!");
	}

	float font_scale = 1;
	float scale_x, scale_y;

	{
		int render_w, render_h;
		int window_w, window_h;
		SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
		SDL_GetWindowSize(window, &window_w, &window_h);
		scale_x = (float)(render_w) / (float)(window_w);
		scale_y = (float)(render_h) / (float)(window_h);
		SDL_RenderSetScale(renderer, scale_x, scale_y);
		font_scale = scale_y;
	}

	// Load Default Font
	{
		struct nk_font_atlas *atlas;
		struct nk_font_config config = nk_font_config(0);
		struct nk_font *font;

		nk_sdl_font_stash_begin(&atlas);
		font = nk_font_atlas_add_default(atlas, 16 * font_scale, &config);
		nk_sdl_font_stash_end();
		font->handle.height /= font_scale;
		nk_style_set_font(nkCtx, &font->handle);
	}

	SDL_ShowWindow(window);

	if (_win)      *_win = window;
	if (_ren)      *_ren = renderer;
	if (_ctx)      *_ctx = nkCtx;
	if (scaleX)    *scaleX = scale_x;
	if (scaleY)    *scaleY = scale_y;
	if (fontScale) *fontScale = font_scale;

	return 0;
}

static unsigned int frameStart, frameTime;
static unsigned int frameDelay = 1000 / 50; // 50 fps

void Chip8Front_NewFrame() {
	frameStart = SDL_GetTicks();
}

void Chip8Front_RenderToScreen(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
	SDL_RenderClear(renderer);
	nk_sdl_render(NK_ANTI_ALIASING_ON);
	SDL_RenderPresent(renderer); // Swap Front & Back Buffers

	frameTime = SDL_GetTicks() - frameStart;
	if (frameDelay > frameTime) SDL_Delay(frameDelay - frameTime);
	frameStart = SDL_GetTicks();
}

void QuitChip8Front(SDL_Window* _win, SDL_Renderer* _ren, nkctx_t* _ctx) {
	nk_sdl_shutdown();
	SDL_DestroyRenderer(_ren);
	SDL_DestroyWindow(_win);
	SDL_Quit();
}

