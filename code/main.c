#include <libdragon.h>
#include <stdlib.h>

sprite_t *libdr_title;

static char *title_sprites[] = {
    "rom:/libdragon.sprite",
    "rom:/tiny3d.sprite",
    "rom:/startscreen.sprite",
};

int main(void) {
    debug_init_isviewer();
    debug_init_usblog();

	display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
	dfs_init(DFS_DEFAULT_LOCATION);

    joypad_init();
    timer_init();

    // rdpq_init();
    // rdpq_debug_start();

	libdr_title = sprite_load(title_sprites[0]);

	while (1) {
        surface_t* disp = display_get();
		display_show(disp);
		graphics_draw_sprite_trans(disp, 0, 0, libdr_title);
    }
}