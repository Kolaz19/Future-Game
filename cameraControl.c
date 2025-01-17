#include "include/cameraControl.h"

void cam_initializeCamera(Camera2D *cam, int screenWidth, int screenHeight, int mapWidth, int startHeight) {
    cam->zoom = (float)screenWidth / mapWidth;
    cam->target.x = mapWidth / 2.0f;
    cam->target.y = startHeight;
    cam->offset.x = screenWidth / 2.0f;
    cam->offset.y = screenHeight / 2.0f;
    cam->rotation = 0.0f;
}
