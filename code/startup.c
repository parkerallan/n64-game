#include <libdragon.h>
#include "startup.h"

#define SCREEN_TIME_TICKS (2 * TICKS_PER_SECOND)

// Menu definition
struct main_menu menu = {
    .pos = 0,
    .num_items = 2,
    .items = {
        "Continue",
        "New Game",
    },
};

bool handle_startup_sequence(surface_t* disp, joypad_buttons_t button, startup_state_t* state, u_uint32_t* last_time) {
    uint32_t current_time = timer_ticks();
    
    // Auto-advance timer
    if (current_time - *last_time > SCREEN_TIME_TICKS) {
        *last_time = current_time;
        (*state)++;
        if (*state > STARTUP_MAIN_MENU) {
            *state = STARTUP_MAIN_MENU;
        }
    }
    
    switch (*state) {
        case STARTUP_LIBDRAGON_LOGO:
            wav64_play(&gamestart, 0);
            graphics_draw_sprite_trans(disp, 160, 120, libdr_title);
            if (button.a || button.start) {
                sprite_free(libdr_title);
                *state = STARTUP_TINY3D_LOGO;
                *last_time = timer_ticks();
            }
            break;
            
        case STARTUP_TINY3D_LOGO:
            graphics_draw_sprite_trans(disp, 160, 120, tiny3D_title);
            if (button.a || button.start) {
                sprite_free(tiny3D_title);
                *state = STARTUP_TITLE_SCREEN;
                *last_time = timer_ticks();
            }
            break;
            
        case STARTUP_TITLE_SCREEN:
            //wav64_play(&titlesong, 1);
            graphics_draw_sprite_trans(disp, 0, 0, startscreen);
            if (button.a || button.start) {
                *state = STARTUP_MAIN_MENU;
                *last_time = timer_ticks();
            }
            break;
            
        case STARTUP_MAIN_MENU:
            return handle_main_menu(disp, button);
            
        default:
            break;
    }
    
    return false; // Still in startup sequence
}

bool handle_main_menu(surface_t* disp, joypad_buttons_t button) {
    // Calculate screen center (assuming 640x480 resolution)
    int screen_width = 640;
    int screen_height = 480;
    int menu_start_y = (screen_height / 2) - (menu.num_items * 10); // Center vertically
    
    // Draw menu items centered
    for (int i = 0; i < menu.num_items; i++) {
        if (i == menu.pos) {
            graphics_set_color(
                graphics_make_color(0, 255, 0, 0),
                graphics_make_color(0, 0, 0, 0)
            );
        } else {
            graphics_set_color(
                graphics_make_color(255, 255, 255, 0),
                graphics_make_color(0, 0, 0, 0)
            );
        }
        // Center horizontally (approximate text width calculation)
        int text_width = strlen(menu.items[i]) * 8; // Rough character width
        int x_pos = (screen_width / 2) - (text_width / 2);
        graphics_draw_text(disp, x_pos, menu_start_y + i * 30, menu.items[i]);
    }

    // Handle menu navigation
    if (button.d_down) {
        menu.pos = (menu.pos + 1) % menu.num_items;
    } else if (button.d_up) {
        menu.pos = (menu.pos - 1 + menu.num_items) % menu.num_items;
    } else if (button.a || button.start) {
        if (menu.pos == 0) {
            // Continue
            //wav64_close(&titlesong);
            return true; // Start game
        } else if (menu.pos == 1) {
            // New Game
            //wav64_close(&titlesong);
            return true; // Start game
        }
    }
    
    return false; // Stay in menu
}
