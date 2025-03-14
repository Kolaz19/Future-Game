#ifndef TMW_WRAPPER_H
#define TMW_WRAPPER_H

#include "raylib/raylib.h"

typedef struct Manager *MapManager;


/*
 * Load current map and try to load next map
 */
MapManager map_createMapManager(int startLevel);
void map_free(MapManager manager);

/*
 * Draw current and next map
 */
void map_draw(MapManager manager, Camera2D *cam);

/*
 * Load next map when player position is in the
 * middle of next map. Unload current map.
 * Swap next map with current map.
 * @return true if new map was loaded
 */
bool map_update(MapManager manager, float playerY);

/*
 * Extract rectangles from specified object layer
 * @return Amount of rectangles
 */
int map_getRectanglesFromCurrentMap(MapManager manager, const char *layerName, Rectangle *rectangles, int *ids);

/*
 * Extract rectangles from specified object layer
 * @return Amount of rectangles
 */
int map_getRectanglesFromNextMap(MapManager manager, const char *layerName, Rectangle *rectangles, int *ids);

/*
 * Get number of current map
 * @return Map level
 */
int map_getCurrentMapLevel(MapManager manager);

/*
 * Get max dimension of map
 */
Rectangle map_getBoundaryFromCurrentMap(MapManager manager);

/*
 * Get max dimension of map
 */
Rectangle map_getBoundaryFromNextMap(MapManager manager);

/*
 * Check if next map is loaded
 * It is not loaded when manager startet at last map
 * or has switched to last map
 */
bool map_hasNextMap(MapManager manager);
#endif
