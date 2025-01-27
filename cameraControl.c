#include "include/cameraControl.h"

#define MAX_DISTANCE 100
#define MAX_DISTANCE_STEP 0.1f

void cam_initializeCamera(Camera2D *cam, int screenWidth, int screenHeight, int mapWidth, int startHeight) {
    cam->zoom = (float)screenWidth / (float)mapWidth;
    cam->target.x = (float)mapWidth / 2.0f;
    cam->target.y = (float)startHeight;
    cam->offset.x = (float)screenWidth / 2.0f;
    cam->offset.y = (float)screenHeight / 2.0f;
    cam->rotation = 0.0f;
}

void cam_updateCamera(Camera2D *cam, float playerY) {
    float *camY = &(cam->target.y);

	if (*camY == playerY)
		return;

	//Don't go over max distance
    if (playerY - *camY > MAX_DISTANCE) {
        *camY = playerY - MAX_DISTANCE;
    } else if (*camY - playerY > MAX_DISTANCE) {
        *camY = playerY + MAX_DISTANCE;
    }

	//Get current distance between cam and player
    float camDist = playerY - *camY;
    if (camDist < 0)
        camDist *= -1;

	//Get close to player with cam faster the greater the distance
    float curStepDistance = (camDist * 100 / MAX_DISTANCE) * MAX_DISTANCE_STEP;

	if (*camY < playerY) {
		*camY += curStepDistance;
		if (*camY > playerY)
			*camY = playerY;
	} else if (*camY > playerY) {
		*camY -= curStepDistance;
		if (*camY < playerY)
			*camY = playerY;
	}
}
