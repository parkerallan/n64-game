#ifndef STARTUP_H
#define STARTUP_H

#include <libdragon.h>

typedef enum {
    STARTUP_LIBDRAGON_LOGO = 0,
    STARTUP_TINY3D_LOGO = 1,
    STARTUP_TITLE_SCREEN = 2,
    STARTUP_MAIN_MENU = 3,
    STARTUP_COMPLETE = 4
} startup_state_t;

extern sprite_t *libdr_title;
extern sprite_t *tiny3D_title;
extern sprite_t *startscreen;
extern wav64_t gamestart;
extern wav64_t titlesong;

struct main_menu {
    int pos;
    int num_items;
    char *items[];
};

extern struct main_menu menu;

// Function declarations
bool handle_startup_sequence(surface_t* disp, joypad_buttons_t button, startup_state_t* state, u_uint32_t* last_time);
bool handle_main_menu(surface_t* disp, joypad_buttons_t button);

#endif
