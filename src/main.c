#include <stdio.h>
#include "minimal_window.h"

int main(void) {
    int width = 800, height = 600;
    minimal_window_create_fixed_size_window(width, height);
    while (minimal_window_process_events()) {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                minimal_window_draw_pixel(i, j, 255, 50, 255);
            }
        }
    }

    return 0;
}

