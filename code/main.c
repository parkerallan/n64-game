#include <libdragon.h>
#include <stdio.h>

int main(void) {
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    console_init();
    console_set_render_mode(RENDER_AUTOMATIC);

    console_clear();
    printf("Hello, World!\n");

    console_render();
}