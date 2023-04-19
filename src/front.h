#ifndef CHIP_8_FRONTEND_H_INCLUDED_
#define CHIP_8_FRONTEND_H_INCLUDED_ 1

/*
	front.h - abstraction for initializing/de-initialized & handling SDL2 & Nuklear UI
*/

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "third_party/nuklear.h"
#include "third_party/nuklear_sdl_renderer.h"

typedef struct nk_context nkctx_t;

// Initialize SDL2 & Nuklear UI, takes pointers to variables to hold various values you will need, any or all of them can be NULL if not required.
int  InitChip8Front(SDL_Window** _win, SDL_Renderer** _ren, float* scaleX, float* scaleY, float* fontScale, nkctx_t** _ctx);

// To be called before every new frame.
void Chip8Front_NewFrame();

// To be called to draw everything to screen.
void Chip8Front_RenderToScreen(SDL_Renderer* renderer);

// De-Initialize SDL2 & Nuklear UI.
void QuitChip8Front(SDL_Window* _win, SDL_Renderer* _ren, nkctx_t* _ctx);

#endif // CHIP_8_FRONTEND_H_INCLUDED_


