#ifndef BOX2DWRAPPER_H
#define BOX2DWRAPPER_H

#include "raylib/rectangle.h"
#define BAG_SIZE 50

typedef enum BodyType {
    STATIC_PLATFORM,
    CHARACTER
} BodyType;

typedef struct World *WorldHandle;

WorldHandle phy_createWorld(void);
void updateWorld(WorldHandle handle);
void phy_free(WorldHandle handle);
void phy_addPlatform(WorldHandle world, Rectangle plat);
int phy_getRectangles(WorldHandle handle, Rectangle **rectangles, BodyType type);

#endif
