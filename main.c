#include "include/customLogging.h"
#include "include/slog.h"
#include "include/utest.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "include/animationPlayer.h"
#include "include/cameraControl.h"
#include "include/mapManager.h"
#include "include/physicsWorld.h"
#include "include/raylib/raylib.h"

#define SCREEN_WIDTH ((int)(1920 * 0.8))
#define SCREEN_HEIGHT ((int)(1080 * 0.8))

// #define UTEST_EXE

void addPlatforms(WorldHandle handle, MapManager manager, bool initial);
void addLongWalls(WorldHandle worldHandle, MapManager mapManager);

int main(int argc, char *argv[]) {
#ifdef UTEST_EXE
    return utest_main(0, NULL);
#endif
    initLogger(argc, argv);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Future");
    SetTargetFPS(60);

    MapManager mapManager = map_createMapManager(1);
    WorldHandle worldHandle = phy_createWorld();

    phy_addPlayer(worldHandle);
    BodyIdReference playerBody = phy_getCharacterBodyIdReference(worldHandle);

    addLongWalls(worldHandle, mapManager);
    addPlatforms(worldHandle, mapManager, true);

    BodyRectReference playerRectangle;
    phy_getBodyRectReferences(worldHandle, &playerRectangle, CHARACTER);

    Camera2D camera;
    cam_initializeCamera(&camera, SCREEN_WIDTH, SCREEN_HEIGHT, (int)(map_getBoundaryFromCurrentMap(mapManager).width), 330);

    PlAnimation plAnim = panim_createAnimation();
    Vector2 forceOfCharacter;

    while (!WindowShouldClose()) {

		// Player body was disabled - player is dead
        if (!phy_isEnable(playerBody)) {
            panim_setDying(plAnim);
        } else {
            phy_getVelocity(playerBody, &forceOfCharacter.x, &forceOfCharacter.y);
        }

        panim_update(plAnim, forceOfCharacter.x, forceOfCharacter.y);
        phy_updateWorld(worldHandle);
        cam_updateCamera(&camera, playerRectangle.rectangle->y);

        if (map_update(mapManager, playerRectangle.rectangle->y)) {
            addLongWalls(worldHandle, mapManager);
            phy_destroyObjectsAbove(worldHandle, map_getBoundaryFromCurrentMap(mapManager).y - 10.0f);
            addPlatforms(worldHandle, mapManager, false);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        map_draw(mapManager, &camera);
        panim_draw(plAnim, playerRectangle.rectangle->x, playerRectangle.rectangle->y);

        EndMode2D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    map_free(mapManager);
    phy_free(worldHandle);
    panim_free(plAnim);
    CloseWindow();
    slog_destroy();
    return 0;
}

void addPlatforms(WorldHandle handle, MapManager manager, bool initial) {
    Rectangle platforms[50];
    int dynamicPlatformsIds[50];
    int amountOfRectangles = 0;

    if (initial) {
        amountOfRectangles = map_getRectanglesFromCurrentMap(manager, "Platforms", platforms, NULL);
        for (int i = 0; i < amountOfRectangles; i++) {
            phy_addPlatform(handle, platforms[i]);
        }
        amountOfRectangles = map_getRectanglesFromCurrentMap(manager, "Dynamic", platforms, dynamicPlatformsIds);
        for (int i = 0; i < amountOfRectangles; i++) {
            phy_addDynamic(handle, platforms[i], dynamicPlatformsIds[i]);
        }
    }

    amountOfRectangles = map_getRectanglesFromNextMap(manager, "Platforms", platforms, NULL);
    for (int i = 0; i < amountOfRectangles; i++) {
        phy_addPlatform(handle, platforms[i]);
    }
    amountOfRectangles = map_getRectanglesFromNextMap(manager, "Dynamic", platforms, dynamicPlatformsIds);
    for (int i = 0; i < amountOfRectangles; i++) {
        phy_addDynamic(handle, platforms[i], dynamicPlatformsIds[i]);
    }
}

void addLongWalls(WorldHandle worldHandle, MapManager mapManager) {
    Rectangle targetMapBoundary = map_getBoundaryFromCurrentMap(mapManager);
    if (map_hasNextMap(mapManager)) {
        Rectangle nextMapBoundary = map_getBoundaryFromNextMap(mapManager);
        targetMapBoundary.height += nextMapBoundary.height;
    }
    phy_addWalls(worldHandle, targetMapBoundary, 16);
}
