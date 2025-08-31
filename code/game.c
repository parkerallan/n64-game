#include "game.h"

#define PLAYER_SPEED 1.5f
#define TURN_SPEED 0.08f

TunnelScene tunnel_scene;

void tunnel_scene_init() {
    // Initialize lighting vectors
    tunnel_scene.lightDirVec = (T3DVec3){{0.3f, -0.8f, 0.5f}};
    tunnel_scene.lightDirVec2 = (T3DVec3){{-0.5f, 0.2f, -0.3f}};
    t3d_vec3_norm(&tunnel_scene.lightDirVec);
    t3d_vec3_norm(&tunnel_scene.lightDirVec2);
    
    // Load models
    tunnel_scene.tunnel_model = t3d_model_load("rom:/tunnel.t3dm");
    tunnel_scene.player_model = t3d_model_load("rom:/block.t3dm");
    
    // Initialize player
    tunnel_scene.player.position = (T3DVec3){{0.0f, 0.0f, 0.0f}};
    tunnel_scene.player.rotation_y = M_PI/2;  // Face positive x-axis on start
    tunnel_scene.player.move_speed = PLAYER_SPEED;
    tunnel_scene.player.turn_speed = TURN_SPEED;
    
    // Allocate uncached memory for player model matrix (required for RSP DMA)
    tunnel_scene.player.modelMat = malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(tunnel_scene.player.modelMat);
    
    // Initialize third-person camera settings - Better distances for visibility
    tunnel_scene.camDistance = 20.0f;  // Further back for better view
    tunnel_scene.camHeight = 20.0f;    // Higher for over-shoulder view
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
    
    // Create viewport
    tunnel_scene.viewport = t3d_viewport_create();
}

void tunnel_scene_update(joypad_buttons_t button) {
    // Calculate movement vector based on current rotation
    float moveX = 0.0f, moveZ = 0.0f;
    
    // Forward/Backward movement (D-pad up/down)
    if(button.d_up) {
        moveX = sinf(tunnel_scene.player.rotation_y) * tunnel_scene.player.move_speed;
        moveZ = -cosf(tunnel_scene.player.rotation_y) * tunnel_scene.player.move_speed;
    } else if(button.d_down) {
        moveX = -sinf(tunnel_scene.player.rotation_y) * tunnel_scene.player.move_speed;
        moveZ = cosf(tunnel_scene.player.rotation_y) * tunnel_scene.player.move_speed;
    }
    
    // Turning (D-pad left/right)
    if(button.d_left) {
        tunnel_scene.player.rotation_y -= tunnel_scene.player.turn_speed;
    } else if(button.d_right) {
        tunnel_scene.player.rotation_y += tunnel_scene.player.turn_speed;
    }
    
    // Apply movement - NO CONSTRAINTS
    tunnel_scene.player.position.x += moveX;
    tunnel_scene.player.position.z += moveZ;
    
    // Keep rotation in 0-2π range
    if (tunnel_scene.player.rotation_y < 0) tunnel_scene.player.rotation_y += 2 * M_PI;
    if (tunnel_scene.player.rotation_y >= 2 * M_PI) tunnel_scene.player.rotation_y -= 2 * M_PI;
    
    // Position player model with offset so it's visible - THIS IS the player's actual position
    float modelOffsetX = sinf(tunnel_scene.player.rotation_y) * 2.0f; // Smaller offset for better positioning
    float modelOffsetZ = -cosf(tunnel_scene.player.rotation_y) * 2.0f;

    // The player model position IS the player's logical position
    float playerModelX = tunnel_scene.player.position.x + modelOffsetX;
    float playerModelY = tunnel_scene.player.position.y - 40.0f; // Much lower - closer to ground
    float playerModelZ = tunnel_scene.player.position.z + modelOffsetZ;

    // Update player model matrix using proper T3D transformation
    float scale[3] = {0.1f, 0.1f, 0.1f};
    float rotation[3] = {0.0f, tunnel_scene.player.rotation_y, 0.0f};
    float position[3] = {playerModelX, playerModelY, playerModelZ};

    t3d_mat4fp_from_srt_euler(tunnel_scene.player.modelMat, scale, rotation, position);

    // Calculate third-person camera position - orbits around the PLAYER MODEL position
    // Camera directly behind the player for straight view
    float camOffsetX = sinf(tunnel_scene.player.rotation_y + M_PI) * tunnel_scene.camDistance;
    float camOffsetZ = -cosf(tunnel_scene.player.rotation_y + M_PI) * tunnel_scene.camDistance;

    tunnel_scene.camPos.x = playerModelX + camOffsetX;
    tunnel_scene.camPos.y = playerModelY + tunnel_scene.camHeight;
    tunnel_scene.camPos.z = playerModelZ + camOffsetZ;

    // Camera target looks ahead of the player for better forward view
    float lookAheadDistance = 5.0f;
    float targetAheadX = sinf(tunnel_scene.player.rotation_y) * lookAheadDistance;
    float targetAheadZ = -cosf(tunnel_scene.player.rotation_y) * lookAheadDistance;
    
    tunnel_scene.camTarget.x = playerModelX + targetAheadX;
    tunnel_scene.camTarget.y = playerModelY + 10.0f; // Look slightly above model for better view
    tunnel_scene.camTarget.z = playerModelZ + targetAheadZ;    // Debug: Print player position (uncomment to debug)
    // debugf("Player pos: X=%.2f, Y=%.2f, Z=%.2f, Rot=%.2f\n", 
    //        tunnel_scene.player.position.x, tunnel_scene.player.position.y, 
    //        tunnel_scene.player.position.z, tunnel_scene.player.rotation_y);
}

void tunnel_scene_render() {
    t3d_viewport_set_projection(&tunnel_scene.viewport, T3D_DEG_TO_RAD(75.0f), 0.1f, 200.0f);
    t3d_viewport_look_at(&tunnel_scene.viewport, &tunnel_scene.camPos, &tunnel_scene.camTarget, &(T3DVec3){{0,1,0}});

    rdpq_attach(display_get(), display_get_zbuf());

    t3d_frame_start();
    t3d_viewport_attach(&tunnel_scene.viewport);

    // Dark atmosphere for dungeon
    t3d_screen_clear_color(RGBA32(10, 10, 20, 0xFF));
    t3d_screen_clear_depth();

    // Set up atmospheric lighting
    t3d_light_set_ambient(tunnel_scene.colorAmbient);
    t3d_light_set_directional(0, tunnel_scene.colorDir, &tunnel_scene.lightDirVec);
    t3d_light_set_directional(1, tunnel_scene.colorDir2, &tunnel_scene.lightDirVec2);
    t3d_light_set_count(2);

    // Draw the tunnel
    t3d_model_draw(tunnel_scene.tunnel_model);
    
    // Draw the player model using the proper T3D matrix system
    t3d_matrix_push(tunnel_scene.player.modelMat);
    t3d_model_draw(tunnel_scene.player_model);
    t3d_matrix_pop(1);

    rdpq_detach_show();
}

void tunnel_scene_cleanup() {
    if (tunnel_scene.tunnel_model) {
        t3d_model_free(tunnel_scene.tunnel_model);
        tunnel_scene.tunnel_model = NULL;
    }
    
    if (tunnel_scene.player_model) {
        t3d_model_free(tunnel_scene.player_model);
        tunnel_scene.player_model = NULL;
    }
    
    if (tunnel_scene.player.modelMat) {
        free_uncached(tunnel_scene.player.modelMat);
        tunnel_scene.player.modelMat = NULL;
    }
}
