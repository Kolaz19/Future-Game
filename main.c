#include "include/customLogging.h"
#include "include/slog.h"
#include "include/utest.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "include/animationPlayer.h"
#include "include/cameraControl.h"
#include "include/physicsPlayer.h"
#include "include/physicsWorld.h"
#include "include/raylib/raylib.h"
#include "include/mapManager.h"

#define SCREEN_WIDTH (1920 * 0.8)
#define SCREEN_HEIGHT (1080 * 0.8)

// #define SHOW_COLLISION
// #define UTEST_EXE

int main(int argc, char *argv[]) {
#ifdef UTEST_EXE
    return utest_main(0, NULL);
#endif
    initLogger(argc, argv);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Future");
    SetTargetFPS(60);

    const char *tmxFileName = "assets/map_part1.tmx";
    MapManager manager = map_createMapManager(1);

    WorldHandle worldHandle = phy_createWorld();

    Rectangle colRectangles[3];
    int amountOfRectangles = map_getRectanglesFromCurrentMap(manager, "Platforms", colRectangles);
    for (int i = 0; i < amountOfRectangles; i++) {
        phy_addPlatform(worldHandle, colRectangles[i]);
    }
    amountOfRectangles = map_getRectanglesFromNextMap(manager, "Platforms", colRectangles);
    for (int i = 0; i < amountOfRectangles; i++) {
        phy_addPlatform(worldHandle, colRectangles[i]);
    }

    phy_addPlayer(worldHandle);
    phy_addWalls(worldHandle, map_getBoundaryFromCurrentMap(manager), 16);
    phy_addWalls(worldHandle, map_getBoundaryFromNextMap(manager), 16);

    BodyIdReference ref = phy_getCharacterBodyReference(worldHandle);
    assert(ref != NULL);

    BodyRectReference platforms[BAG_SIZE];
    BodyRectReference playerBody;
    int amountPlatforms = phy_getBodyReferences(worldHandle, platforms, STATIC_PLATFORM);
    int amountPlayers = phy_getBodyReferences(worldHandle, &playerBody, CHARACTER);
    assert(amountPlayers == 1);

    Camera2D camera;
    cam_initializeCamera(&camera, SCREEN_WIDTH, SCREEN_HEIGHT, map_getBoundaryFromNextMap(manager).width, 330);

    PlAnimation plAnim = panim_createAnimation();
    Vector2 force;

    while (!WindowShouldClose()) {

        pl_update(ref);

        pl_getVelocity(ref, &force.x, &force.y);
        panim_update(plAnim, force.x, force.y);
        phy_updateWorld(worldHandle);
        cam_updateCamera(&camera, playerBody.rectangle->y);
        if (map_update(manager, playerBody.rectangle->y)) {
            amountOfRectangles = map_getRectanglesFromNextMap(manager, "Platforms", colRectangles);
            for (int i = 0; i < amountOfRectangles; i++) {
                phy_addPlatform(worldHandle, colRectangles[i]);
            }
            phy_addWalls(worldHandle, map_getBoundaryFromNextMap(manager), 16);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        map_draw(manager, &camera);

#ifdef SHOW_COLLISION
        for (int i = 0; i < amountPlatforms; i++) {
            DrawRectangle((int)platforms[i].rectangle->x, (int)platforms[i].rectangle->y, (int)platforms[i].rectangle->width, (int)platforms[i].rectangle->height, GREEN);
        }
        DrawRectangle((int)playerBody.rectangle->x, (int)playerBody.rectangle->y, (int)playerBody.rectangle->width, (int)playerBody.rectangle->height, BLUE);
#endif
        panim_draw(plAnim, (int)playerBody.rectangle->x, (int)playerBody.rectangle->y);

        EndMode2D();
        DrawFPS(10, 10);
        EndDrawing();
    }

    map_free(manager);
    phy_free(worldHandle);
    panim_free(plAnim);
    CloseWindow();
    slog_destroy();
    return 0;
}
