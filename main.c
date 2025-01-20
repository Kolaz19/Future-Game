#include <assert.h>

#include "include/raylib/raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "include/box2dWrapper.h"
#include "include/cameraControl.h"
#include "include/raytmx/raytmx.h"
#include "include/tmxWrapper.h"
#include <stdio.h>

#define SCREEN_WIDTH (1920 * 0.8)
#define SCREEN_HEIGHT (1080 * 0.8)

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Future");

    const char *tmxFileName = "assets/map_part1.tmx";
    TmxMap *map = LoadTMX(tmxFileName);
    if (map == NULL) {
        TraceLog(LOG_ERROR, "Failed to load TMX");
    }

    Rectangle colRectangles[3];
    int amountOfRectangles = map_getRectanglesFromObjectLayer(map, "Platforms", colRectangles);
    WorldHandle worldHandle = phy_createWorld();

    for (int i = 0; i < amountOfRectangles; i++) {
        phy_addPlatform(worldHandle, colRectangles[i]);
    }

	Rectangle *platforms[BAG_SIZE];
	int amountPlatforms = phy_getRectangles(worldHandle, platforms, STATIC_PLATFORM);


    Camera2D camera;
    cam_initializeCamera(&camera, SCREEN_WIDTH, SCREEN_HEIGHT, map->width * map->tileWidth, 330);

    while (!WindowShouldClose()) {

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        DrawTMX(map, &camera, 0, 0, WHITE);

		for (int i = 0; i < amountPlatforms; i++) {
			DrawRectangle(platforms[i]->x, platforms[i]->y, platforms[i]->width, platforms[i]->height, GREEN);
		}

        EndMode2D();
        EndDrawing();
    }

    UnloadTMX(map);
    phy_free(worldHandle);
    CloseWindow();

    return 0;
}
