#include "include/cameraControl.h"

void cam_initializeCamera(Camera2D *cam, int screenWidth, int screenHeight, int mapWidth, int startHeight) {
    cam->zoom = (float)screenWidth / (float)mapWidth;
    cam->target.x = (float)mapWidth / 2.0f;
    cam->target.y = (float)startHeight;
    cam->offset.x = (float)screenWidth / 2.0f;
    cam->offset.y = (float)screenHeight / 2.0f;
    cam->rotation = 0.0f;
}
