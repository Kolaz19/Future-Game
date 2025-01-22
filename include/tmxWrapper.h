#ifndef TMW_WRAPPER_H
#define TMW_WRAPPER_H

#include "raytmx/raytmx.h"

/*
 * Extract rectangles from specified object layer
 * @return Amount of rectangles
 */
int map_getRectanglesFromObjectLayer(const TmxMap *map, const char *layerName, Rectangle *rectangles);

#endif
