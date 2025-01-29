#include "include/utest.h"
#include <assert.h>
#include <stdio.h>

#include "include/raylib/raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "include/animationPlayer.h"
#include "include/cameraControl.h"
#include "include/physicsPlayer.h"
#include "include/physicsWorld.h"
#include "include/tmxWrapper.h"

#define SCREEN_WIDTH (1920 * 0.8)
#define SCREEN_HEIGHT (1080 * 0.8)

// #define SHOW_COLLISION
#define UTEST_EXE

int main(void) {
#ifdef UTEST_EXE
    return utest_main(0, NULL);
#endif
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Future");
    SetTargetFPS(60);

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
    phy_addPlayer(worldHandle);
    phy_addWalls(worldHandle, (int)(map->width * map->tileWidth), (int)(map->height * map->tileHeight), 0, 0, 16);

    BodyIdReference ref = phy_getCharacterBodyReference(worldHandle);
    assert(ref != NULL);

    BodyRectReference platforms[BAG_SIZE];
    BodyRectReference playerBody;
    int amountPlatforms = phy_getBodyReferences(worldHandle, platforms, STATIC_PLATFORM);
    int amountPlayers = phy_getBodyReferences(worldHandle, &playerBody, CHARACTER);
    assert(amountPlayers == 1);

    Camera2D camera;
    cam_initializeCamera(&camera, SCREEN_WIDTH, SCREEN_HEIGHT, (int)(map->width * map->tileWidth), 330);

    PlAnimation plAnim = panim_createAnimation();
    Vector2 force;

    while (!WindowShouldClose()) {

        pl_update(ref);

        pl_getVelocity(ref, &force.x, &force.y);
        panim_update(plAnim, force.x, force.y);
        phy_updateWorld(worldHandle);
        cam_updateCamera(&camera, playerBody.rectangle->y);

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        DrawTMX(map, &camera, 0, 0, WHITE);

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

    UnloadTMX(map);
    phy_free(worldHandle);
    panim_free(plAnim);
    CloseWindow();

    return 0;
}
