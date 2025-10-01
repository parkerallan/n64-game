#include <libdragon.h>
#include "startup.h"

#define SCREEN_TIME_TICKS (2 * TICKS_PER_SECOND)

// Font ID for custom fonts
#define FONT_MENU 1
#define FONT_COPYRIGHT 2

// Menu definition
struct main_menu menu = {
    .pos = 0,
    .num_items = 1,
    .items = {
        "PRESS BUTTON TO START",
    },
};

// Audio playback flags to prevent repeated playback
static bool gamestart_played = false;

// Font variables
rdpq_font_t *menu_font = NULL;
rdpq_font_t *subtext_font = NULL;

void startup_init_fonts() {
    menu_font = rdpq_font_load("rom:/NeuropolX.font64");
    if (menu_font) {
        rdpq_font_style(menu_font, 0, &(rdpq_fontstyle_t){
            .color = RGBA32(0, 255, 0, 255),
        });
        rdpq_text_register_font(FONT_MENU, menu_font);
    }
    
    // Load smaller version of the same font for copyright text
    subtext_font = rdpq_font_load("rom:/NeuropolX-small.font64");
    if (subtext_font) {
        rdpq_font_style(subtext_font, 0, &(rdpq_fontstyle_t){
            .color = RGBA32(0, 255, 0, 255),
        });
        rdpq_text_register_font(FONT_COPYRIGHT, subtext_font);
    }
}

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
            if (!gamestart_played) {
                wav64_play(&gamestart, 0);
                gamestart_played = true;
            }
            graphics_draw_sprite_trans(disp, 0, 0, libdr_title);
            if (button.a || button.start) {
                sprite_free(libdr_title);
                rdpq_sync_pipe();
                rdpq_sync_load();
                *state = STARTUP_TINY3D_LOGO;
                *last_time = timer_ticks();
            }
            break;
            
        case STARTUP_TINY3D_LOGO:
            graphics_draw_sprite_trans(disp, 0, 0, tiny3D_title);
            if (button.a || button.start) {
                sprite_free(tiny3D_title);
                rdpq_sync_pipe();
                rdpq_sync_load();
                *state = STARTUP_TITLE_SCREEN;
                *last_time = timer_ticks();
            }
            break;
            
        case STARTUP_TITLE_SCREEN:
            //wav64_play(&titlesong, 1);
            graphics_draw_sprite_trans(disp, 0, 0, startscreen);
            
            // Add copyright text overlay on the startscreen
            rdpq_attach(disp, NULL);
            rdpq_textparms_t copyright_params = {
                .align = ALIGN_CENTER,
                .width = 640,
                .height = 480,
            };
            // Draw copyright at bottom edge of screen using smaller version of the same font
            rdpq_text_print(&copyright_params, FONT_COPYRIGHT, 0, 460, "2020 2025 NERIX LTD.,ALL RIGHTS RESERVED");
            rdpq_detach();
            
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
    // Calculate screen dimensions (assuming 640x480 resolution)
    int screen_width = 640;
    int screen_height = 480;
    
    // Use RDPQ text system for better rendering
    rdpq_attach(disp, NULL);
    
    // Set text parameters for main menu text (moved lower on screen)
    rdpq_textparms_t text_params = {
        .align = ALIGN_CENTER,
        .width = screen_width,
        .height = screen_height,
    };
    
    // If we have a custom font, use it, otherwise use default font
    int font_id = (menu_font != NULL) ? FONT_MENU : FONT_BUILTIN_DEBUG_MONO;
    
    // Draw the main menu text (positioned lower on screen)
    rdpq_text_print(&text_params, font_id, 0, 320, menu.items[0]);
    
    // Draw the subtext at bottom edge of screen
    rdpq_textparms_t subtext_params = {
        .align = ALIGN_CENTER,
        .width = screen_width,
        .height = screen_height,
    };
    rdpq_text_print(&subtext_params, FONT_COPYRIGHT, 0, 460, "2020 2025 NERIX LTD.,ALL RIGHTS RESERVED");
    
    rdpq_detach();

    // Handle menu navigation - for single item menu, any button starts the game
    if (button.a || button.start) {
        return true;
    }
    
    return false;
}
