#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>
#include <t3d/t3danim.h>
#include <t3d/t3ddebug.h>

#define PLAYER_SPEED 2.0f
#define SCREEN_TIME_TICKS (2 * TICKS_PER_SECOND)

sprite_t *libdr_title;
sprite_t *tiny3D_title;
sprite_t *startscreen;
wav64_t gamestart;
wav64_t titlesong;

surface_t *depthBuffer;
T3DModel *block;
T3DViewport viewport;

int state;
bool isGameStarted;
bool isPaused;
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
    int pos_z;
    int width;
    int height;
    int length;
};

void handle_start_menu_input(struct main_menu *menu, joypad_buttons_t button) {

}

T3DVec3 camPos = {{20.0f, -20.0f, 20.0f}};
T3DVec3 camTarget = {{0.0f, 0.0f, 0.0f}};

uint8_t colorAmbient[4] = {80, 80, 100, 0xFF};
uint8_t colorDir[4]     = {0xEE, 0xAA, 0xAA, 0xFF};

T3DVec3 lightDirVec = {{1.0f, 1.0f, 1.0f}};

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

    libdr_title = sprite_load("rom:/libdragon.sprite");
    tiny3D_title = sprite_load("rom:/tiny3d.sprite");
    startscreen = sprite_load("rom:/startscreen.sprite");
    wav64_open(&gamestart, "rom:/gamestart.wav64");
    wav64_open(&titlesong, "rom:/pianoanime.wav64");
}

int main(void) {
    initialize();
    
    t3d_vec3_norm(&lightDirVec);
    block = t3d_model_load("rom:/block.t3dm");
    depthBuffer = display_get_zbuf();
    viewport = t3d_viewport_create();

    state = 0;
    isGameStarted = false;
    last_time = timer_ticks();

	while (1) {
        surface_t* disp = display_get();
		display_show(disp);

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

        uint32_t current_time = timer_ticks();
        if (current_time - last_time > SCREEN_TIME_TICKS) {
            last_time = current_time; // Reset the timer
            state++; // Move to the next state
            if (state > 3) {
                state = 3; // Stay on the last state
            }
        }

        joypad_buttons_t button = joypad_get_buttons_pressed(JOYPAD_PORT_1);
        
        if (state == 0 && !isGameStarted) {
            wav64_play(&gamestart, 0);
            graphics_draw_sprite_trans(disp, 160, 120, libdr_title);
            if (button.a || button.start) {
                sprite_free(libdr_title);
                state = 1;
                last_time = timer_ticks();
            }
        } else if (state == 1 && !isGameStarted) {
            graphics_draw_sprite_trans(disp, 160, 120, tiny3D_title);
            if (button.a || button.start) {
                sprite_free(tiny3D_title);
                state = 2;
                last_time = timer_ticks();
            }
        } else if (state == 2 && !isGameStarted) {
            wav64_play(&titlesong, 1);
            graphics_draw_sprite_trans(disp, 0, 0, startscreen);
            if (button.a || button.start) {
                state = 3;
                last_time = timer_ticks();
            }
        } 
        
        if (state == 3 && !isGameStarted) {
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
                    wav64_close(&titlesong);
                } else if (menu.pos == 1) {
                    isGameStarted = true;
                    // New Game
                    wav64_close(&titlesong);
                }
            }
        }

        if (isGameStarted) {

            t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(90.0f), 20.0f, 160.0f);
            t3d_viewport_look_at(&viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

            rdpq_attach(display_get(), display_get_zbuf());

            t3d_frame_start();
            t3d_viewport_attach(&viewport);

            t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
            t3d_screen_clear_depth();

            t3d_light_set_ambient(colorAmbient);
            t3d_light_set_directional(0, colorDir, &lightDirVec);
            t3d_light_set_count(1);

            t3d_model_draw(block);
            rdpq_detach_show();

            joypad_buttons_t button = joypad_get_buttons(JOYPAD_PORT_1);
            
            if(button.c_right) {
                camPos.x += PLAYER_SPEED;
            } else if(button.c_left) {
                camPos.x -= PLAYER_SPEED;
            } else if(button.c_up) {
                camPos.y += PLAYER_SPEED;
            } else if(button.c_down) {
                camPos.y -= PLAYER_SPEED;
            }

            // if (button.start) {
            //     isGameStarted = false;
            //     isPaused = true;
            // }
        }
    }
}