#include "player.h"
#include "controls.h"
#include <malloc.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void player_init(Player* player) {
    // Initialize player position and rotation
    player->position = (T3DVec3){{0.0f, 0.0f, 0.0f}};
    player->rotation_y = M_PI/2;  // Face positive x-axis on start
    player->move_speed = PLAYER_SPEED;
    player->turn_speed = TURN_SPEED;
    
    // Load player model (textures are embedded in .t3dm file)
    player->model = t3d_model_load("rom:/player2.t3dm");
    player->texture = NULL;  // Not needed for T3D models
    
    // Create skeleton for skinned rendering like animation example
    player->skeleton = t3d_skeleton_create(player->model);
    t3d_skeleton_update(&player->skeleton);
    
    // Allocate uncached memory for player model matrix (required for RSP DMA)
    player->modelMat = malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(player->modelMat);
}

void player_update(Player* player, joypad_buttons_t buttons, joypad_inputs_t inputs) {
    // Get normalized input from controls system
    PlayerInput input = controls_get_player_input(buttons, inputs);
    
    // Calculate movement vector based on current rotation and input
    float moveX = 0.0f, moveZ = 0.0f;
    
    // Apply movement speed modifier if running
    float currentMoveSpeed = player->move_speed;
    if (input.run) {
        currentMoveSpeed *= 1.5f; // 50% faster when running
    }
    
    // Forward/Backward movement
    if (input.move_forward != 0.0f) {
        moveX = sinf(player->rotation_y) * input.move_forward * currentMoveSpeed;
        moveZ = -cosf(player->rotation_y) * input.move_forward * currentMoveSpeed;
    }
    
    // Strafe left/right movement
    if (input.move_right != 0.0f) {
        float strafeX = cosf(player->rotation_y) * input.move_right * currentMoveSpeed * 0.7f; // Slower strafe
        float strafeZ = sinf(player->rotation_y) * input.move_right * currentMoveSpeed * 0.7f;
        moveX += strafeX;
        moveZ += strafeZ;
    }
    
    // Turning
    if (input.turn_rate != 0.0f) {
        float currentTurnSpeed = player->turn_speed;
        if (input.run) {
            currentTurnSpeed *= 1.3f; // Faster turning when running
        }
        player->rotation_y += input.turn_rate * currentTurnSpeed;
    }
    
    // Apply movement
    player->position.x += moveX;
    player->position.z += moveZ;
    
    // Keep rotation in 0-2π range
    if (player->rotation_y < 0) player->rotation_y += 2 * M_PI;
    if (player->rotation_y >= 2 * M_PI) player->rotation_y -= 2 * M_PI;
    
    // Update player model matrix
    float modelOffsetX = sinf(player->rotation_y) * 20.0f;
    float modelOffsetZ = -cosf(player->rotation_y) * 20.0f;
    
    float playerModelX = player->position.x + modelOffsetX;
    float playerModelY = player->position.y;  // Same level as tunnel floor
    float playerModelZ = player->position.z + modelOffsetZ;
    
    // Update player model matrix using proper T3D transformation
    float scale[3] = {0.5f, 0.5f, 0.5f};
    float rotation[3] = {0.0f, player->rotation_y + M_PI, 0.0f};  // Add 180 degrees (π radians) to face correct direction
    float position[3] = {playerModelX, playerModelY, playerModelZ};
    
    t3d_mat4fp_from_srt_euler(player->modelMat, scale, rotation, position);
}

void player_render(Player* player) {
    // Draw the player using skinned model like animation example
    t3d_matrix_push(player->modelMat);
    rdpq_set_prim_color(RGBA32(255, 255, 255, 255)); // Set prim color like animation example
    t3d_model_draw_skinned(player->model, &player->skeleton);
    t3d_matrix_pop(1);
}

void player_cleanup(Player* player) {
    if (player->model) {
        t3d_model_free(player->model);
        player->model = NULL;
    }
    
    // Cleanup skeleton like animation example
    t3d_skeleton_destroy(&player->skeleton);
    
    // No need to free texture - T3D handles this internally
    
    if (player->modelMat) {
        free_uncached(player->modelMat);
        player->modelMat = NULL;
    }
}

void player_get_model_position(Player* player, float* x, float* y, float* z) {
    float modelOffsetX = sinf(player->rotation_y) * 20.0f;
    float modelOffsetZ = -cosf(player->rotation_y) * 20.0f;
    
    *x = player->position.x + modelOffsetX;
    *y = player->position.y;
    *z = player->position.z + modelOffsetZ;
}

T3DVec3 player_get_camera_position(Player* player, float camDistance, float camHeight) {
    float playerModelX, playerModelY, playerModelZ;
    player_get_model_position(player, &playerModelX, &playerModelY, &playerModelZ);
    
    float camOffsetX = sinf(player->rotation_y + M_PI) * camDistance;
    float camOffsetZ = -cosf(player->rotation_y + M_PI) * camDistance;
    
    T3DVec3 camPos;
    camPos.x = playerModelX + camOffsetX;
    camPos.y = playerModelY + camHeight;
    camPos.z = playerModelZ + camOffsetZ;
    
    return camPos;
}

T3DVec3 player_get_camera_target(Player* player, float lookAheadDistance, float lookUpOffset) {
    float playerModelX, playerModelY, playerModelZ;
    player_get_model_position(player, &playerModelX, &playerModelY, &playerModelZ);
    
    float targetAheadX = sinf(player->rotation_y) * lookAheadDistance;
    float targetAheadZ = -cosf(player->rotation_y) * lookAheadDistance;
    
    T3DVec3 camTarget;
    camTarget.x = playerModelX + targetAheadX;
    camTarget.y = playerModelY + lookUpOffset;
    camTarget.z = playerModelZ + targetAheadZ;
    
    return camTarget;
}