#include "include/raylib/raylib.h"
#define RAYTMX_IMPLEMENTATION
#include "include/raytmx/raytmx.h"
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


    Camera2D camera;
    camera.zoom = SCREEN_WIDTH / (map->width * map->tileWidth);
    camera.target.x = (float)(map->width * map->tileWidth) / 2.0f;
    camera.target.y = (float)(map->height * map->tileHeight) / 2.0f;
    camera.offset.x = (float)SCREEN_WIDTH / 2.0f;
    camera.offset.y = (float)SCREEN_HEIGHT / 2.0f;
    camera.rotation = 0.0f;


    float panSpeed = 150.0f;

    while (!WindowShouldClose()) {

        if (IsKeyDown(KEY_RIGHT))
            camera.target.x += panSpeed * GetFrameTime();
        if (IsKeyDown(KEY_LEFT))
            camera.target.x -= panSpeed * GetFrameTime();
        if (IsKeyDown(KEY_DOWN))
            camera.target.y += panSpeed * GetFrameTime();
        if (IsKeyDown(KEY_UP))
            camera.target.y -= panSpeed * GetFrameTime();
        if (IsKeyDown(KEY_KP_ADD))
            camera.zoom += camera.zoom < 10.0f ? 0.25f : 0.0f;
        if (IsKeyDown(KEY_KP_SUBTRACT))
            camera.zoom -= camera.zoom > 1.0f ? 0.25f : 0.0f;

        BeginDrawing();
        ClearBackground(BLACK);
        BeginMode2D(camera);
        DrawTMX(map, &camera, 0, 0, WHITE);
        EndMode2D();
        EndDrawing();
    }

	UnloadTMX(map);
    CloseWindow();

    return 0;
}
