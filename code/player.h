#ifndef PLAYER_H
#define PLAYER_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <t3d/t3dskeleton.h>

#define PLAYER_SPEED 6.5f
#define TURN_SPEED 0.08f

typedef struct {
    T3DVec3 position;
    float rotation_y;
    float move_speed;
    float turn_speed;
    T3DMat4FP* modelMat;
    T3DModel* model;
    T3DSkeleton skeleton;  // Add skeleton for skinned rendering
    sprite_t* texture;
} Player;

// Player management functions
void player_init(Player* player);
void player_update(Player* player, joypad_buttons_t buttons, joypad_inputs_t inputs);
void player_render(Player* player);
void player_cleanup(Player* player);

// Player utility functions
void player_get_model_position(Player* player, float* x, float* y, float* z);
T3DVec3 player_get_camera_position(Player* player, float camDistance, float camHeight);
T3DVec3 player_get_camera_target(Player* player, float lookAheadDistance, float lookUpOffset);

#endif