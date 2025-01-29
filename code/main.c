#include <libdragon.h>

#define PLAYER_SPEED 2.0f
#define SCREEN_TIME_TICKS (2 * TICKS_PER_SECOND)

sprite_t *libdr_title;
sprite_t *tiny3D_title;
sprite_t *startscreen;

int state;
bool isGameStarted = false;
u_uint32_t last_time;

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

void handle_start_menu_input(struct main_menu *menu, joypad_buttons_t button) {

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
    last_time = timer_ticks();

    // rdpq_init();
    // rdpq_debug_start();
    // rdpq_debug_end();

	while (1) {
        surface_t* disp = display_get();
		display_show(disp);

        joypad_poll();
        
        uint32_t current_time = timer_ticks();
        if (current_time - last_time > SCREEN_TIME_TICKS) {
            last_time = current_time; // Reset the timer
            state++; // Move to the next state
            if (state > 3) {
                state = 3; // Stay on the last state
            }
        }

        joypad_buttons_t button = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        
        if (state == 0) {
            graphics_draw_sprite_trans(disp, 0, 0, libdr_title);
            if (button.a || button.start) {
                state = 1;
                last_time = timer_ticks();
            }
        } else if (state == 1) {
            graphics_draw_sprite_trans(disp, 0, 0, tiny3D_title);
            if (button.a || button.start) {
                state = 2;
                last_time = timer_ticks();
            }
        } else if (state == 2) {
            graphics_draw_sprite_trans(disp, 0, 0, startscreen);
            if (button.a || button.start) {
                state = 3;
                last_time = timer_ticks();
            }
        } 
        
        if (state == 3) {
            for (int i = 0; i < menu.num_items; i++) {
                if (i == menu.pos) {
                    graphics_set_color(
                        graphics_make_color(0, 255, 0, 0),
                        graphics_make_color(0, 0, 0, 0)
                    );
                } else {
                    graphics_set_color(
                        graphics_make_color(0, 0, 0, 0),
                        graphics_make_color(0, 0, 0, 0)
                    );
                }
                graphics_draw_text(disp, 130, 200 + i * 20, menu.items[i]);
            }

            if (button.d_down) {
                menu.pos = (menu.pos + 1) % menu.num_items;
            } else if (button.d_up) {
                menu.pos = (menu.pos - 1 + menu.num_items) % menu.num_items;
            } else if (button.a || button.start) {
                if (menu.pos == 0) {
                    isGameStarted = true;
                    // Continue
                } else if (menu.pos == 1) {
                    isGameStarted = true;
                    // New Game
                }
            }
        }

        if (isGameStarted) {
            // 3d setup, model and game mechanics
        }
    }
}