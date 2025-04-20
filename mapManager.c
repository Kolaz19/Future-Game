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

#define MAPNAME_MAXLEN 27
#define OBJ_MAX_NAMELEN 20
#define LASTLEVEL 4

static const char *mapNamePrefix = "assets/maps/map_part";
static const char *mapNameSuffix = ".tmx";

static void setMapFileName(int level, char *mapName) {
    char *curChar = mapName;
    size_t prefixLen = strlen(mapNamePrefix);
    strncpy(curChar, mapNamePrefix, prefixLen);
    curChar += prefixLen;

    if (level > 9) {
        int nextIntChar = level / 10;
        *curChar = '0' + (char)nextIntChar;
        curChar++;
        nextIntChar = level % 10;
        *curChar = '0' + (char)nextIntChar;
        curChar++;
    } else {
        *curChar = '0' + (char)level;
        curChar++;
    }

    size_t suffixLen = strlen(mapNameSuffix) + 1;
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
                    (int)manager->curMap->height *
                        (int)manager->curMap->tileHeight,
                WHITE);
    }
}

bool map_update(MapManager manager, float playerY) {
    if (manager->nextMap == NULL) {
        return false;
    }

    // Check if player position is over halfway point of next map
    if (playerY > (float)manager->startYCurMap +
                      (float)(manager->curMap->height * manager->curMap->tileHeight) +
                      ((float)manager->nextMap->height * (float)manager->curMap->tileHeight / 2.0f)) {
        slogi("Map switch detected at player Y position [%f]", playerY);
        // Set starting point for current map after current map
        // because next map takes it's place
        manager->startYCurMap = manager->startYCurMap +
                                (int)(manager->curMap->height * manager->curMap->tileHeight);
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

    if (startLevel == 1) {
        loadMap(&(manager->curMap), startLevel);
        loadMap(&(manager->nextMap), startLevel + 1);
        manager->curMapLevel = startLevel;
    } else {
        manager->curMapLevel = startLevel - 1;
        loadMap(&(manager->curMap), startLevel - 1);
        loadMap(&(manager->nextMap), startLevel);
    }

    return manager;
}

void map_free(MapManager manager) {
    free(manager);
}

static int getRectanglesFromObjectLayer(const TmxMap *map, int mapStartY, const char *layerName, Rectangle *rectangles, int *ids) {
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
        rectangles[i].y = (float)mapStartY + (float)matchingLayer->exact.objectGroup.objects[i].y;
        rectangles[i].width = (float)matchingLayer->exact.objectGroup.objects[i].width;
        rectangles[i].height = (float)matchingLayer->exact.objectGroup.objects[i].height;
        if (ids != NULL) {
            if (strlen(matchingLayer->exact.objectGroup.objects[i].name) > OBJ_MAX_NAMELEN) {
                sloge("Object name '%s' length exceeds %d characters", matchingLayer->exact.objectGroup.objects[i].name, OBJ_MAX_NAMELEN);
                return 0;
            }
            char *endPtr;
            ids[i] = (int)strtol(matchingLayer->exact.objectGroup.objects[i].name, &endPtr, 10);
            if (ids[i] == 0) {
                sloge("Object name '%s' could not be converted into integer value", matchingLayer->exact.objectGroup.objects[i].name);
                return 0;
            }
        }
    }

    return (int)matchingLayer->exact.objectGroup.objectsLength;
}

int map_getRectanglesFromCurrentMap(MapManager manager, const char *layerName, Rectangle *rectangles, int *ids) {
    if (manager->curMap == NULL) {
        sloge("Attempt to get rectangles from NULL (current) map");
        return 0;
    }
    return getRectanglesFromObjectLayer(manager->curMap, manager->startYCurMap, layerName, rectangles, ids);
}

int map_getRectanglesFromNextMap(MapManager manager, const char *layerName, Rectangle *rectangles, int *ids) {
    if (manager->nextMap == NULL) {
        sloge("Attempt to get rectangles from NULL (next) map");
        return 0;
    }
    return getRectanglesFromObjectLayer(manager->nextMap,
                                        manager->startYCurMap + (int)manager->curMap->tileHeight * (int)manager->curMap->height,
                                        layerName, rectangles, ids);
}

int map_getCurrentMapLevel(MapManager manager) {
    return manager->curMapLevel;
}

int map_getNextMapLevel(MapManager manager) {
    return manager->curMapLevel + 1;
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
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};
    }
    Rectangle boundary = {
        .x = 0.0f,
        .y = (float)manager->startYCurMap,
        .width = (float)(manager->curMap->tileWidth * manager->curMap->width),
        .height = (float)(manager->curMap->tileHeight * manager->curMap->height),
    };
    return boundary;
}

Rectangle map_getBoundaryFromNextMap(MapManager manager) {
    if (manager->nextMap == NULL) {
        sloge("Attempt to get rectangle from NULL (next) map");
        return (Rectangle){0.0f, 0.0f, 0.0f, 0.0f};
    }
    Rectangle boundary = {
        .x = 0.0f,
        .y = (float)((uint32_t)manager->startYCurMap + manager->curMap->tileHeight * manager->curMap->height),
        .width = (float)(manager->nextMap->tileWidth * manager->nextMap->width),
        .height = (float)(manager->nextMap->tileHeight * manager->nextMap->height),
    };
    return boundary;
}
