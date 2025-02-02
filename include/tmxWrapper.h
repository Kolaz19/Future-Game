#ifndef TMW_WRAPPER_H
#define TMW_WRAPPER_H

#include "raylib/raylib.h"

typedef struct Manager *MapManager;

/*
 * Extract rectangles from specified object layer
 * @return Amount of rectangles
 */

MapManager map_createMapManager(int startLevel);
void map_free(MapManager manager);
void map_draw(MapManager manager, Camera2D *cam);
bool map_update(MapManager manager, float playerY);
int map_getRectanglesFromCurrentMap(MapManager manager, const char *layerName, Rectangle *rectangles);
int map_getRectanglesFromNextMap(MapManager manager, const char *layerName, Rectangle *rectangles);
int map_getCurrentMapLevel(MapManager manager);
Rectangle map_getBoundaryFromCurrentMap(MapManager manager);
Rectangle map_getBoundaryFromNextMap(MapManager manager);
#endif
