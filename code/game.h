#ifndef GAME_H
#define GAME_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    T3DVec3 position;
    float rotation_y;  // Y-axis rotation for turning
    float move_speed;
    float turn_speed;
    T3DMat4FP *modelMat;  // Matrix for player model transformation
} Player;

typedef struct {
    T3DModel *tunnel_model;
    T3DModel *player_model;
    T3DViewport viewport;
    Player player;
    
    // Third-person camera
    T3DVec3 camPos;
    T3DVec3 camTarget;
    float camDistance;
    float camHeight;
    
    // Lighting
    uint8_t colorAmbient[4];
    uint8_t colorDir[4];
    uint8_t colorDir2[4];
    T3DVec3 lightDirVec;
    T3DVec3 lightDirVec2;
} TunnelScene;

// External variables
extern TunnelScene tunnel_scene;

// Function declarations
void tunnel_scene_init();
void tunnel_scene_update(joypad_buttons_t button);
void tunnel_scene_render();
void tunnel_scene_cleanup();

#endif
