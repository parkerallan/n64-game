#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>
#include <math.h>
#include "startup.h"
#include "game.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define SCREEN_TIME_TICKS (2 * TICKS_PER_SECOND)

sprite_t *libdr_title;
sprite_t *tiny3D_title;
sprite_t *startscreen;
wav64_t gamestart;
wav64_t titlesong;

surface_t *depthBuffer;

startup_state_t startup_state;
bool isGameStarted;
bool isPaused;
u_uint32_t last_time;

struct player {
    T3DVec3 position;
    float rotation_y;  // Y-axis rotation for turning
    float move_speed;
    float turn_speed;
};

void handle_start_menu_input(struct main_menu *menu, joypad_buttons_t button) {

}

// Enhanced lighting for dungeon atmosphere
uint8_t colorAmbient[4] = {30, 30, 50, 0xFF};      // Dark blue ambient
uint8_t colorDir[4]     = {200, 180, 120, 0xFF};   // Warm directional light
uint8_t colorDir2[4]    = {50, 100, 150, 0xFF};    // Cool secondary light

T3DVec3 lightDirVec = {{0.3f, -0.8f, 0.5f}};       // Main light from above-forward
T3DVec3 lightDirVec2 = {{-0.5f, 0.2f, -0.3f}};     // Secondary light for shadows

void initialize() {
    debug_init_isviewer();
    debug_init_usblog();
    timer_init();
    console_init();
    joypad_init();
    audio_init(48000, 4);
    mixer_init(20);

	display_init(RESOLUTION_640x480, DEPTH_16_BPP, 3, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS_DEDITHER);
	dfs_init(DFS_DEFAULT_LOCATION);
    t3d_init((T3DInitParams){});
    rdpq_init();
    rdpq_debug_start();

    //libdr_title = sprite_load("rom:/libdragon.sprite");
    //tiny3D_title = sprite_load("rom:/tiny3d.sprite");
    //startscreen = sprite_load("rom:/startscreen.sprite");
    //wav64_open(&gamestart, "rom:/gamestart.wav64");
    //wav64_open(&titlesong, "rom:/pianoanime.wav64");
    
    // Initialize fonts for menu
    startup_init_fonts();
}

int main(void) {
    initialize();
    
    // Initialize lighting vectors
    t3d_vec3_norm(&lightDirVec);
    t3d_vec3_norm(&lightDirVec2);
    
    // Initialize tunnel scene
    tunnel_scene_init();
    
    depthBuffer = display_get_zbuf();

    // startup_state = STARTUP_LIBDRAGON_LOGO;
    // isGameStarted = false;
    // last_time = timer_ticks();

    isGameStarted = true; //debug disabling startup

	while (1) {
        surface_t* disp = display_get();

        joypad_poll();
        
        if (audio_can_write()) {
            // Select an audio buffer that we can write to
            short *buf = audio_write_begin();
            // Write to the audio buffer from the mixer
            mixer_poll(buf, audio_get_buffer_length());
            // Tell the audio system that the buffer has
            // been filled and is ready for playback
            audio_write_end();
	    }
	    
	    // Ensure audio playback coordination
	    mixer_try_play();

        joypad_buttons_t button = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        
        if (!isGameStarted) {
            isGameStarted = handle_startup_sequence(disp, button, &startup_state, &last_time);
        }

        if (isGameStarted) {
            // Get continuous button input for smooth movement
            joypad_buttons_t continuous_button = joypad_get_buttons(JOYPAD_PORT_1);
            joypad_inputs_t continuous_inputs = joypad_get_inputs(JOYPAD_PORT_1);
            
            // Update tunnel scene
            tunnel_scene_update(continuous_button, continuous_inputs);
            
            // Render tunnel scene
            tunnel_scene_render();
        }
        
        // Show the completed frame
        display_show(disp);
    }
}