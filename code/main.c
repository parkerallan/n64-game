#include <libdragon.h>

#define PLAYER_SPEED 2.0f

sprite_t *libdr_title;
sprite_t *tiny3D_title;
sprite_t *startscreen;

int state;
bool isGameStarted = false;

struct main_menu {
    int pos;
    int num_items;
    char *items[];
};

struct main_menu menu = {
    .pos = 0,
    .num_items = 2,
    .items = {
        "Continue",
        "New Game",
    },
};

struct player {
    int pos_x;
    int pos_y;
    int width;
    int height;
    int length;
};

static char *title_sprites[] = {
    "rom:/libdragon.sprite",
    "rom:/tiny3d.sprite",
    "rom:/startscreen.sprite",
};

void display_start_menu(surface_t* disp, struct main_menu *menu) {
    for (int i = 0; i < menu->num_items; i++) {
        if (i == menu->pos) {
            graphics_draw_text(disp, 10, 10 + i * 20, ">");
        }
        graphics_draw_text(disp, 20, 10 + i * 20, menu->items[i]);
    }
}

void handle_start_menu_input(struct main_menu *menu, joypad_buttons_t button) {
    if (button.d_down) {
        menu->pos = (menu->pos + 1) % menu->num_items;
    } else if (button.d_up) {
        menu->pos = (menu->pos - 1 + menu->num_items) % menu->num_items;
    } else if (button.a || button.start) {
        if (menu->pos == 0) {
            isGameStarted = true;
            // Continue
        } else if (menu->pos == 1) {
            isGameStarted = true;
            // New Game
        }
    }
}

int main(void) {

    debug_init_isviewer();
    debug_init_usblog();
    joypad_init();
    timer_init();
    console_init();

	display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
	dfs_init(DFS_DEFAULT_LOCATION);

	libdr_title = sprite_load(title_sprites[0]);
    tiny3D_title = sprite_load(title_sprites[1]);
    startscreen = sprite_load(title_sprites[2]);

    state = 0;

    // rdpq_init();
    // rdpq_debug_start();
    // rdpq_debug_end();

	while (1) {
        surface_t* disp = display_get();
		display_show(disp);

        joypad_poll();
        
        if (joypad_is_connected(JOYPAD_PORT_1)) {
            joypad_buttons_t button = joypad_get_buttons_pressed(JOYPAD_PORT_1);
            
            if (state == 0) {
                graphics_draw_sprite_trans(disp, 0, 0, libdr_title);
                if (button.a || button.start) {
                    state = 1;
                }
            } else if (state == 1) {
                graphics_draw_sprite_trans(disp, 0, 0, tiny3D_title);
                if (button.a || button.start) {
                    state = 2;
                }
            } else if (state == 2) {
                graphics_draw_sprite_trans(disp, 0, 0, startscreen);
                if (button.a || button.start) {
                    state = 3;
                }
            } else if (state == 3) {
                if (button.start) {
                    // display_start_menu(disp, &menu);
                    // handle_start_menu_input(&menu, button);
                }
            }
        }
    }
}