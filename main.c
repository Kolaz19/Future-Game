#include "include/box2d/box2d.h"
#include "include/customLogging.h"
#include "include/slog.h"
#include "include/utest.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "include/animationPlayer.h"
#include "include/cameraControl.h"
#include "include/checkPoint.h"
#include "include/drawDynamicPlatform.h"
#include "include/mapManager.h"
#include "include/physicsWorld.h"
#include "include/raylib/raylib.h"
#include "include/textDraw.h"
#include "include/sounds.h"

#define SCREEN_WIDTH ((int)(1920 * 0.5))
#define SCREEN_HEIGHT ((int)(1080 * 0.5))

// #define UTEST_EXE

void addPlatforms(WorldHandle handle, MapManager manager, bool initial);
void addLongWalls(WorldHandle worldHandle, MapManager mapManager);
void resetCheckpoint(Checkpoint checkpoint, MapManager mapManager);

int main(int argc, char *argv[]) {
#ifdef UTEST_EXE
    return utest_main(0, NULL);
#endif
    initLogger(argc, argv);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Future");
    SetWindowSize((int)((float)GetMonitorWidth(GetCurrentMonitor()) * 0.8f),
                  (int)((float)GetMonitorHeight(GetCurrentMonitor()) * 0.8f));
    SetWindowPosition(10, 10);
    SetTargetFPS(60);

    Checkpoint checkpoint = check_createCheckpoint();
    // Load savestate here
    MapManager mapManager = map_createMapManager(check_getCurrentLevel(checkpoint));
    resetCheckpoint(checkpoint, mapManager);

    WorldHandle worldHandle = phy_createWorld();

    phy_addPlayer(worldHandle, check_getX(checkpoint), check_getY(checkpoint));
    BodyIdReference playerBody = phy_getCharacterBodyIdReference(worldHandle);
    int amountDynamicGroundContact = 0;

    addLongWalls(worldHandle, mapManager);
    addPlatforms(worldHandle, mapManager, true);

    BodyRectReference playerRectangle;
    BodyRectReference dynamicRectangles[BAG_SIZE];
    phy_getBodyRectReferences(worldHandle, &playerRectangle, CHARACTER);
    int amountDynamicRecs = phy_getBodyRectReferences(worldHandle, dynamicRectangles, DYNAMIC_PLATFORM);
    PlatformTextureHandle platTextHandle = platTex_createPlatformTextureHandle();

    Camera2D camera;
    cam_initializeCamera(&camera, (int)(map_getBoundaryFromCurrentMap(mapManager).width), 330);

    PlAnimation plAnim = panim_createAnimation();
    Vector2 forceOfCharacter;

    TextHandle textHandle = text_init();
    text_activateLevelText(textHandle, check_getCurrentLevel(checkpoint));

	sound_init();
	sound_resetSound();

    while (!WindowShouldClose()) {

        // Player body was disabled - player is dead
        if (phy_isPlayerDead(worldHandle)) {
            panim_setDying(plAnim);
        } else {
            phy_getVelocity(playerBody, &forceOfCharacter.x, &forceOfCharacter.y);
            if (check_update(checkpoint, playerRectangle.rectangle->y)) {
                text_activateLevelText(textHandle, check_getCurrentLevel(checkpoint));
            }
        }

        phy_updateDynamicGroundContact(playerBody, &amountDynamicGroundContact);
        panim_update(plAnim, forceOfCharacter.x, forceOfCharacter.y, amountDynamicGroundContact > 0);
        phy_updateWorld(worldHandle);
        cam_updateCamera(&camera, playerRectangle.rectangle->y, (int)(map_getBoundaryFromCurrentMap(mapManager).width));
        text_update(textHandle);

        if (IsKeyPressed(KEY_R)) {
            slogi("Player reset the level with checkpoint level %d", check_getCurrentLevel(checkpoint));
            // Reset game and load at current checkpoint
            amountDynamicGroundContact = 0;
            map_free(mapManager);
            phy_free(worldHandle);
            mapManager = map_createMapManager(check_getCurrentLevel(checkpoint));
            resetCheckpoint(checkpoint, mapManager);

            worldHandle = phy_createWorld();
            phy_addPlayer(worldHandle, check_getX(checkpoint), check_getY(checkpoint));
            playerBody = phy_getCharacterBodyIdReference(worldHandle);
            addLongWalls(worldHandle, mapManager);
            addPlatforms(worldHandle, mapManager, true);
            phy_getBodyRectReferences(worldHandle, &playerRectangle, CHARACTER);
            amountDynamicRecs = phy_getBodyRectReferences(worldHandle, dynamicRectangles, DYNAMIC_PLATFORM);
            panim_setAlive(plAnim);
			sound_resetSound();
        }

        if (map_update(mapManager, playerRectangle.rectangle->y)) {
            addLongWalls(worldHandle, mapManager);
            phy_destroyObjectsAbove(worldHandle, map_getBoundaryFromCurrentMap(mapManager).y - 10.0f);
            addPlatforms(worldHandle, mapManager, false);
            amountDynamicRecs = phy_getBodyRectReferences(worldHandle, dynamicRectangles, DYNAMIC_PLATFORM);
			sound_resetSound();
            Rectangle newCheckpoint;
            if (map_getRectanglesFromNextMap(mapManager, "Checkpoints", &newCheckpoint, NULL) == 1) {
                check_setNextCheckpoint(checkpoint, &newCheckpoint, map_getNextMapLevel(mapManager));
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        map_draw(mapManager, &camera);
        panim_draw(plAnim, playerRectangle.rectangle->x, playerRectangle.rectangle->y);
        for (int i = 0; i < amountDynamicRecs; i++) {
            platTex_drawPlatform(platTextHandle, dynamicRectangles[i].id, dynamicRectangles[i].rectangle, *dynamicRectangles[i].rotation);
        }
        EndMode2D();
        DrawFPS(10, 10);
        text_draw(textHandle);
        // DrawText(TextFormat("Contact %d",amountDynamicGroundContact), 200, 10, 40, RAYWHITE);
        EndDrawing();
    }

    map_free(mapManager);
    phy_free(worldHandle);
    check_free(checkpoint);
    panim_free(plAnim);
	text_free(textHandle);
	platTex_free(platTextHandle);
	sound_free();
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

void resetCheckpoint(Checkpoint checkpoint, MapManager mapManager) {
    Rectangle newCheckpoint;

    if (check_getCurrentLevel(checkpoint) == 1) {
        // First level
        // Set current checkpoint to first level
        if (map_getRectanglesFromCurrentMap(mapManager, "Checkpoints", &newCheckpoint, NULL) == 1) {
            check_setCurrentCheckpoint(checkpoint, &newCheckpoint, map_getCurrentMapLevel(mapManager));
        } else {
            sloge("No checkpoint in first map");
        }
        // Next checkpoint CAN have a checkpoint
        if (map_getRectanglesFromNextMap(mapManager, "Checkpoints", &newCheckpoint, NULL) == 1) {
            check_setNextCheckpoint(checkpoint, &newCheckpoint, map_getNextMapLevel(mapManager));
        } else {
            check_setNextCheckpoint(checkpoint, &newCheckpoint, map_getCurrentMapLevel(mapManager));
        }
    } else {
        // Second to n level -> Player spawns at top of nextMapLevel and not at currentMapLevel
        if (map_getRectanglesFromNextMap(mapManager, "Checkpoints", &newCheckpoint, NULL) == 1) {
            check_setNextCheckpoint(checkpoint, &newCheckpoint, map_getNextMapLevel(mapManager));
            check_setCurrentCheckpoint(checkpoint, &newCheckpoint, map_getNextMapLevel(mapManager));
        } else {
            sloge("No checkpoint on initial load");
        }
    }
}
