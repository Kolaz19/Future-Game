#ifndef BOX2DWRAPPER
#define BOX2DWRAPPER

#include "box2d/box2d.h"

#define BAG_SIZE 50

typedef enum BodyType {
	STATIC_PLATFORM
}BodyType;

typedef struct ObjectBag {
	b2BodyId bodies[BAG_SIZE];
	int currentEmptyIndex;
	BodyType type;
}ObjectBag;

typedef struct Platform {
    float x;
    float y;
    float width;
    float height;
} Platform;

b2WorldId phy_createWorld(void);
void phy_initBag(ObjectBag *bag, BodyType type);
void phy_addPlatform(b2WorldId *world, ObjectBag *bag, Platform plat);

#endif
