#ifndef PHY_WORLD_H
#define PHY_WORLD_H

#include "raylib/rectangle.h"
#include "bodyIdReference.h"

#define BAG_SIZE 50

typedef struct World *WorldHandle;

typedef enum BodyType {
    STATIC_PLATFORM,
    CHARACTER
} BodyType;

typedef struct BodyRectReference {
	Rectangle *rectangle;
	float *rotation;
} BodyRectReference;

WorldHandle phy_createWorld(void);
void phy_free(WorldHandle handle);

/*
 * Advance physics world and update body references
 */
void phy_updateWorld(WorldHandle handle);

/*
 * Take rectangle and place it as STATIC_PLATFORM in the world
 */
void phy_addPlatform(WorldHandle world, Rectangle plat);

/*
 * Add player at pre-defined position as CHARACTER in the world
 */
void phy_addPlayer(WorldHandle world);

/*
 * Store rectangle and rotation references in array
 * @param bodyReference Array with size of BAG_SIZE to store the references in
 * @param type The type of the objects to store from the handle
 * @return Number of objects stored
 */
int phy_getBodyReferences(WorldHandle handle, BodyRectReference *bodyReferences, BodyType type);

/*
 * Get physics body reference from world
 * @return NULL when not found
 */
BodyIdReference phy_getCharacterBodyReference(WorldHandle handle);
#endif
