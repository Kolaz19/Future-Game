#ifndef CAMERA_CONTROL_H
#define CAMERA_CONTROL_H

#include "raylib/raylib.h"

void cam_initializeCamera(Camera2D *cam, int screenWidth, int screenHeight, int mapWidth, int startHeight);

/*
 * Close distance between camera and player Y position
 */
void cam_updateCamera(Camera2D *cam, float playerY);

#endif
