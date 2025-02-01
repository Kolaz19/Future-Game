#include "include/tmxWrapper.h"
#include <string.h>
#include "include/slog.h"

int map_getRectanglesFromObjectLayer(const TmxMap *map, const char *layerName, Rectangle *rectangles) {
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
		rectangles[i].y = (float)matchingLayer->exact.objectGroup.objects[i].y;
		rectangles[i].width = (float)matchingLayer->exact.objectGroup.objects[i].width;
		rectangles[i].height = (float)matchingLayer->exact.objectGroup.objects[i].height;
	}

	return (int)matchingLayer->exact.objectGroup.objectsLength;
}
