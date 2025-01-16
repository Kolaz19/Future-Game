#include "include/tmxWrapper.h"
#include <string.h>

/**
 * Convert all objects on specific object layer to rectangles
 */
int GetRectanglesFromObjectLayer(const TmxMap *map, const char *layerName, Rectangle *rectangles) {
    TmxLayer *matchingLayer = NULL;

    for (int i = 0; i < map->layersLength; i++) {
        if (strcmp(map->layers[i].name, layerName)) {
            matchingLayer = map->layers + i;
        }
    }

    if (matchingLayer == NULL) {
        TraceLog(LOG_ERROR, "No layer with name %s found", layerName);
		return 0;
    }

    if (matchingLayer->type != LAYER_TYPE_OBJECT_GROUP) {
        TraceLog(LOG_ERROR, "Layer %s not object group layer", layerName);
		return 0;
    }

	for (int i = 0; i < matchingLayer->exact.objectGroup.objectsLength; i++) {
		rectangles[i].x = matchingLayer->exact.objectGroup.objects[i].x;
		rectangles[i].y = matchingLayer->exact.objectGroup.objects[i].y;
		rectangles[i].width = matchingLayer->exact.objectGroup.objects[i].width;
		rectangles[i].height = matchingLayer->exact.objectGroup.objects[i].height;
	}

	return matchingLayer->exact.objectGroup.objectsLength;
}
