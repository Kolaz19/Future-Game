#define RAYTMX_IMPLEMENTATION
#include "include/mapManager.h"
#include "include/raytmx/raytmx.h"
#include "include/slog.h"
#include <assert.h>
#include <string.h>

typedef struct Manager {
    TmxMap *curMap;
    TmxMap *nextMap;
    int startYCurMap;
    int curMapLevel;
} Manager;

#define MAPNAME_MAXLEN 22
#define LASTLEVEL 3

static const char *mapNamePrefix = "assets/map_part";
static const char *mapNameSuffix = ".tmx";

static void setMapFileName(int level, char *mapName) {
    char *curChar = mapName;
    int prefixLen = strlen(mapNamePrefix);
    strncpy(curChar, mapNamePrefix, prefixLen);
    curChar += prefixLen;

    if (level > 9) {
        int nextIntChar = level / 10;
        *curChar = '0' + nextIntChar;
        curChar++;
        nextIntChar = level % 10;
        *curChar = '0' + nextIntChar;
        curChar++;
    } else {
        *curChar = '0' + level;
        curChar++;
    }

    int suffixLen = strlen(mapNameSuffix) + 1;
    strncpy(curChar, mapNameSuffix, suffixLen);
}

static bool loadMap(TmxMap **map, int level) {
    if (level > LASTLEVEL) {
        slogw("Attempted to load last map (Number:%d)", level);
        return false;
    }
    char mapFileName[MAPNAME_MAXLEN];
    setMapFileName(level, mapFileName);
    *map = LoadTMX(mapFileName);
    return true;
}

void map_draw(MapManager manager, Camera2D *cam) {
    DrawTMX(manager->curMap, cam, 0, manager->startYCurMap, WHITE);
    if (manager->nextMap != NULL) {
        // Draw next map under current map
        DrawTMX(manager->nextMap, cam, 0,
                manager->startYCurMap +
                    manager->curMap->height *
                        manager->curMap->tileHeight,
                WHITE);
    }
}

bool map_update(MapManager manager, float playerY) {
    if (manager->nextMap == NULL) {
        return false;
    }

    // Check if player position is over halfway point of next map
    if (playerY > manager->startYCurMap +
                      (manager->curMap->height * manager->curMap->tileHeight) +
                      (manager->nextMap->height * manager->curMap->tileHeight / 2.0f)) {
        slogi("Map switch detected at player Y position [%f]", playerY);
        // Set starting point for current map after current map
        // because next map takes it's place
        manager->startYCurMap = manager->startYCurMap +
                                (manager->curMap->height * manager->curMap->tileHeight);
        UnloadTMX(manager->curMap);
        manager->curMap = manager->nextMap;
        manager->curMapLevel++;
        manager->nextMap = NULL;
        if (loadMap(&(manager->nextMap), manager->curMapLevel + 1)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

MapManager map_createMapManager(int startLevel) {
    MapManager manager = malloc(sizeof(Manager));
    manager->startYCurMap = 0;
    manager->curMap = NULL;
    manager->nextMap = NULL;
    manager->curMapLevel = startLevel;

    loadMap(&(manager->curMap), startLevel);
    loadMap(&(manager->nextMap), startLevel + 1);

    return manager;
}

void map_free(MapManager manager) {
    free(manager);
}

static int getRectanglesFromObjectLayer(const TmxMap *map, int mapStartY, const char *layerName, Rectangle *rectangles) {
    TmxLayer *matchingLayer = NULL;

    for (uint32_t i = 0; i < map->layersLength; i++) {
        if (strcmp(map->layers[i].name, layerName) == 0) {
            matchingLayer = map->layers + i;
        }
    }

    if (matchingLayer == NULL) {
        sloge("No layer with name %s found", layerName);
        return 0;
    }

    if (matchingLayer->type != LAYER_TYPE_OBJECT_GROUP) {
        sloge("Layer %s not object group layer, but %d", layerName, matchingLayer->type);
        return 0;
    }

    for (uint32_t i = 0; i < matchingLayer->exact.objectGroup.objectsLength; i++) {
        rectangles[i].x = (float)matchingLayer->exact.objectGroup.objects[i].x;
        rectangles[i].y = mapStartY + (float)matchingLayer->exact.objectGroup.objects[i].y;
        rectangles[i].width = (float)matchingLayer->exact.objectGroup.objects[i].width;
        rectangles[i].height = (float)matchingLayer->exact.objectGroup.objects[i].height;
    }

    return (int)matchingLayer->exact.objectGroup.objectsLength;
}

int map_getRectanglesFromCurrentMap(MapManager manager, const char *layerName, Rectangle *rectangles) {
    if (manager->curMap == NULL) {
        sloge("Attempt to get rectangles from NULL (current) map");
        return 0;
    }
    return getRectanglesFromObjectLayer(manager->curMap, manager->startYCurMap, layerName, rectangles);
}

int map_getRectanglesFromNextMap(MapManager manager, const char *layerName, Rectangle *rectangles) {
    if (manager->nextMap == NULL) {
        sloge("Attempt to get rectangles from NULL (next) map");
        return 0;
    }
    return getRectanglesFromObjectLayer(manager->nextMap,
                                        manager->startYCurMap + manager->curMap->tileHeight * manager->curMap->height,
                                        layerName, rectangles);
}

int map_getCurrentMapLevel(MapManager manager) {
    return manager->curMapLevel;
}

bool map_hasNextMap(MapManager manager) {
    if (manager->nextMap != NULL) {
        return true;
    }
    return false;
}

Rectangle map_getBoundaryFromCurrentMap(MapManager manager) {
    if (manager->curMap == NULL) {
        sloge("Attempt to get rectangle from NULL (current) map");
        return (Rectangle){0.0f,0.0f,0.0f,0.0f};
    }
    Rectangle boundary = {
        .x = 0.0f,
        .y = (float)manager->startYCurMap,
        .width = manager->curMap->tileWidth * manager->curMap->width,
        .height = manager->curMap->tileHeight * manager->curMap->height,
    };
    return boundary;
}

Rectangle map_getBoundaryFromNextMap(MapManager manager) {
    if (manager->nextMap == NULL) {
        sloge("Attempt to get rectangle from NULL (next) map");
        return (Rectangle){0.0f,0.0f,0.0f,0.0f};
    }
    Rectangle boundary = {
        .x = 0.0f,
        .y = (float)manager->startYCurMap + manager->curMap->tileHeight * manager->curMap->height,
        .width = manager->nextMap->tileWidth * manager->nextMap->width,
        .height = manager->nextMap->tileHeight * manager->nextMap->height,
    };
    return boundary;
}
