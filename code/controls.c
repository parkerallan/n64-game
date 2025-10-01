#include "controls.h"
#include <math.h>

void controls_init(void) {
    // Initialize any control-related settings here
    // Could be used for control remapping in the future
}

PlayerInput controls_get_player_input(joypad_buttons_t buttons, joypad_inputs_t inputs) {
    PlayerInput input = {0}; // Initialize all values to 0/false
    
    // D-pad movement (digital)
    if (buttons.d_up) {
        input.move_forward = 1.0f;
    } else if (buttons.d_down) {
        input.move_forward = -1.0f;
    }
    
    // D-pad turning
    if (buttons.d_left) {
        input.turn_rate = -1.0f;
    } else if (buttons.d_right) {
        input.turn_rate = 1.0f;
    }
    
    // Analog stick movement (if you want to add analog support later)
    // Normalize analog stick values (-128 to 127) to (-1.0 to 1.0)
    if (inputs.stick_x != 0) {
        input.move_right = inputs.stick_x / 127.0f;
        // Clamp to -1.0 to 1.0 range
        if (input.move_right > 1.0f) input.move_right = 1.0f;
        if (input.move_right < -1.0f) input.move_right = -1.0f;
    }
    
    if (inputs.stick_y != 0) {
        // Note: stick_y is inverted (up is negative)
        float stick_forward = -inputs.stick_y / 127.0f;
        if (stick_forward > 1.0f) stick_forward = 1.0f;
        if (stick_forward < -1.0f) stick_forward = -1.0f;
        
        // Use analog stick if it's stronger than D-pad input
        if (fabs(stick_forward) > fabs(input.move_forward)) {
            input.move_forward = stick_forward;
        }
    }
    
    // Action buttons
    input.jump = buttons.a;
    input.action = buttons.b;
    input.secondary_action = buttons.c_up;
    input.run = buttons.z;
    
    // Camera controls (C buttons)
    if (buttons.c_left) {
        input.camera_x = -1.0f;
    } else if (buttons.c_right) {
        input.camera_x = 1.0f;
    }
    
    if (buttons.c_up) {
        input.camera_y = 1.0f;
    } else if (buttons.c_down) {
        input.camera_y = -1.0f;
    }
    
    input.camera_reset = buttons.l;
    
    // Menu controls
    input.pause = buttons.start;
    input.select = buttons.a;
    input.cancel = buttons.b;
    
    return input;
}
