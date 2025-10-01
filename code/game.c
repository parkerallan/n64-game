#include "game.h"
#include <rdpq_tex.h>
#include <malloc.h>
#include <t3d/t3dskeleton.h>

TunnelScene tunnel_scene;

sprite_t* tunnelTexture = NULL;

void tunnel_scene_init() {
    // Initialize lighting vectors
    tunnel_scene.lightDirVec = (T3DVec3){{0.3f, -0.8f, 0.5f}};
    tunnel_scene.lightDirVec2 = (T3DVec3){{-0.5f, 0.2f, -0.3f}};
    t3d_vec3_norm(&tunnel_scene.lightDirVec);
    t3d_vec3_norm(&tunnel_scene.lightDirVec2);
    
    // Load tunnel model
    tunnel_scene.tunnel_model = t3d_model_load("rom:/tunnel1.t3dm");
    tunnelTexture = NULL;  // Not needed for T3D models
    
    // Create display list for tunnel
    rspq_block_begin();
    t3d_model_draw(tunnel_scene.tunnel_model);
    tunnel_scene.tunnelDpl = rspq_block_end();
    
    // Initialize player
    player_init(&tunnel_scene.player);
    
    // Initialize third-person camera settings - Better distances for visibility
    tunnel_scene.camDistance = 200.0f;  // Further back for better view
    tunnel_scene.camHeight = 150.0f;    // Higher over-shoulder view of player
    tunnel_scene.camPos = (T3DVec3){{0.0f, tunnel_scene.camHeight, tunnel_scene.camDistance}};
    tunnel_scene.camTarget = (T3DVec3){{0.0f, 0.0f, 0.0f}};
    
    // Initialize lighting colors
    tunnel_scene.colorAmbient[0] = 30;   // R
    tunnel_scene.colorAmbient[1] = 30;   // G  
    tunnel_scene.colorAmbient[2] = 50;   // B
    tunnel_scene.colorAmbient[3] = 0xFF; // A
    
    tunnel_scene.colorDir[0] = 200;      // R
    tunnel_scene.colorDir[1] = 180;      // G
    tunnel_scene.colorDir[2] = 120;      // B
    tunnel_scene.colorDir[3] = 0xFF;     // A
    
    tunnel_scene.colorDir2[0] = 50;      // R
    tunnel_scene.colorDir2[1] = 100;     // G
    tunnel_scene.colorDir2[2] = 150;     // B
    tunnel_scene.colorDir2[3] = 0xFF;    // A
    
    // Create viewport like T3D examples
    tunnel_scene.viewport = malloc(sizeof(T3DViewport));
    *tunnel_scene.viewport = t3d_viewport_create();
}

void tunnel_scene_update(joypad_buttons_t button, joypad_inputs_t inputs) {
    // Update player (handles movement, rotation, and model matrix)
    player_update(&tunnel_scene.player, button, inputs);
    
    // Update camera to follow player
    tunnel_scene.camPos = player_get_camera_position(&tunnel_scene.player, tunnel_scene.camDistance, tunnel_scene.camHeight);
    tunnel_scene.camTarget = player_get_camera_target(&tunnel_scene.player, 100.0f, 0.0f);
}

void tunnel_scene_render() {
    t3d_viewport_set_projection(tunnel_scene.viewport, T3D_DEG_TO_RAD(75.0f), 1.0f, 1000.0f);
    t3d_viewport_look_at(tunnel_scene.viewport, &tunnel_scene.camPos, &tunnel_scene.camTarget, &(T3DVec3){{0,1,0}});

    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    t3d_viewport_attach(tunnel_scene.viewport);

    // Dark atmosphere for dungeon
    t3d_screen_clear_color(RGBA32(10, 10, 20, 0xFF));
    t3d_screen_clear_depth();
    
    // Set render state flags like T3D examples
    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_TEXTURED | T3D_FLAG_DEPTH);
    
    // Set up atmospheric lighting (KEEP YOUR LIGHTING)
    t3d_light_set_ambient(tunnel_scene.colorAmbient);
    t3d_light_set_directional(0, tunnel_scene.colorDir, &tunnel_scene.lightDirVec);
    t3d_light_set_directional(1, tunnel_scene.colorDir2, &tunnel_scene.lightDirVec2);
    t3d_light_set_count(2);

    // Draw the tunnel using display list
    rspq_block_run(tunnel_scene.tunnelDpl);
    
    // Draw the player using skinned rendering
    player_render(&tunnel_scene.player);

    rdpq_detach_show();
}

void tunnel_scene_cleanup() {
    if (tunnel_scene.tunnel_model) {
        t3d_model_free(tunnel_scene.tunnel_model);
        tunnel_scene.tunnel_model = NULL;
    }
    

    
    // No need to free textures - T3D handles this internally
    
    // Cleanup player
    player_cleanup(&tunnel_scene.player);
    
    if (tunnel_scene.viewport) {
        free(tunnel_scene.viewport);
        tunnel_scene.viewport = NULL;
    }
}
