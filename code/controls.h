#ifndef CONTROLS_H
#define CONTROLS_H

#include <libdragon.h>

// Input state structure for normalized control values
typedef struct {
    // Movement controls (-1.0 to 1.0)
    float move_forward;    // Forward/backward movement
    float move_right;      // Strafe left/right movement
    float turn_rate;       // Turning left/right
    
    // Action buttons (true/false)
    bool jump;
    bool action;           // Primary action (attack, interact, etc.)
    bool secondary_action; // Secondary action
    bool run;              // Run modifier
    
    // Camera controls
    float camera_x;        // Camera horizontal movement
    float camera_y;        // Camera vertical movement
    bool camera_reset;     // Reset camera behind player
    
    // Menu/UI controls
    bool pause;
    bool select;
    bool cancel;
} PlayerInput;

// Function declarations
PlayerInput controls_get_player_input(joypad_buttons_t buttons, joypad_inputs_t inputs);
void controls_init(void);

#endif // CONTROLS_H
