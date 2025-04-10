#ifndef PHY_WORLD_H
#define PHY_WORLD_H

#include "raylib/rectangle.h"
#include "bodyIdReference.h"
#include <stdbool.h>

#define BAG_SIZE 50

typedef struct World *WorldHandle;

typedef enum BodyType {
    STATIC_PLATFORM,
	DYNAMIC_PLATFORM,
	WALL,
    CHARACTER
} BodyType;

typedef struct BodyRectReference {
	Rectangle *rectangle;
	float *rotation;
	int id; /// 0 when undefined (optional)
} BodyRectReference;

WorldHandle phy_createWorld(void);
void phy_free(WorldHandle handle);

/*
 * Advance physics world and update body references
 * Run update function for every body
 */
void phy_updateWorld(WorldHandle handle);

/*
 * Take rectangle and place it as STATIC_PLATFORM in the world
 */
void phy_addPlatform(WorldHandle world, Rectangle plat);

/*
 * Take rectangle and place it as DYNAMIC_PLATFORM in the world
 * Dynamic platforms have a ID to identify them further
 */
void phy_addDynamic(WorldHandle world, Rectangle plat, int id);

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
int phy_getBodyRectReferences(WorldHandle handle, BodyRectReference *bodyReferences, BodyType type);

/*
 * Get physics body reference from world
 * @return NULL when not found
 */
BodyIdReference phy_getCharacterBodyIdReference(WorldHandle handle);

/*
 * Add walls as WALL to the world (left and right side)
 */
void phy_addWalls(WorldHandle world, Rectangle boundary, int wallThickness);

/*
 * Destroy all physic objects above specific position
 * @param posY Objects above this position get destroyed
 */
void phy_destroyObjectsAbove(WorldHandle handle, float posY);

void phy_getVelocity(BodyIdReference body, float *velX, float *velY);
bool phy_isEnable(BodyIdReference body);
void phy_setPosition(BodyIdReference body, float posX, float posY);
#endif
