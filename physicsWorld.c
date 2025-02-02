#include "include/physicsWorld.h"
#include <assert.h>
#include <stdlib.h>


#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/raylib/pi.h"
#include "include/slog.h"

#define CONV_VAL 20
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)


typedef struct Body {
    b2BodyId body;
    Rectangle rectangle; /// Is always in pixel dimension
    BodyType type;
    float rotation;
} Body;

struct World {
    b2WorldId world;
    Body *bag[BAG_SIZE];
};

/*
 * Update the rectangle attached to the physics body in Body
 * Convert world dimension to pixel dimension for rectangle
 */
static void updateRectangle(Body *body) {
    assert(body != NULL);
    b2Vec2 vec = b2Body_GetPosition(body->body);
    body->rectangle.x = TOPIXEL(vec.x) - (body->rectangle.width / 2);
    body->rectangle.y = TOPIXEL(vec.y) - (body->rectangle.height / 2);
    body->rotation = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(body->body));
}

/*
 * Copy box2d body and rectangle to bag
 * The references in the bag get updated later in updateWorld()
 * @param bodies Bag
 * @param entry physics body
 * @param width Width of the rectangle
 * @param height Height of the rectangle
 */
static void addToBag(Body **bodies, b2BodyId *entry, BodyType type, float width, float height) {
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] == NULL) {
            bodies[i] = malloc(sizeof(Body));
            bodies[i]->body.world0 = entry->world0;
            bodies[i]->body.revision = entry->revision;
            bodies[i]->body.index1 = entry->index1;

			// Width and height can not be retrieved later from physics object
			// So we have to store it here
            bodies[i]->rectangle.width = width;
            bodies[i]->rectangle.height = height;
			bodies[i]->type = type;
			// Initially set position and rotation
            updateRectangle(bodies[i]);
			slogd("Body in bag at index [%d] added (TYPE:%d)", i, type);
			return;
        }
    }
}

void phy_updateWorld(WorldHandle handle) {
    b2World_Step(handle->world, 1.0f / 60.0f, 4);
    for (int i = 0; i < BAG_SIZE; i++) {
        if (handle->bag[i] != NULL) {
            updateRectangle(handle->bag[i]);
        }
    }
}

WorldHandle phy_createWorld(void) {
    WorldHandle world = malloc(sizeof(struct World));
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 30.0f};

    b2WorldId worldId = b2CreateWorld(&worldDef);
    world->world.index1 = worldId.index1;
    world->world.revision = worldId.revision;

    for (int i = 0; i < BAG_SIZE; i++) {
        world->bag[i] = NULL;
    }
    return world;
}

void phy_free(WorldHandle handle) {
    Body **bodies = handle->bag;

    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] != NULL) {
			BodyType type = bodies[i]->type;
            free(bodies[i]);
			slogd("Body in bag at index [%d] freed (TYPE:%d)",i, type);
        }
    }
    b2DestroyWorld(handle->world);
    free(handle);
}

void phy_addPlatform(WorldHandle world, Rectangle plat) {
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){TOWORLD(plat.x + plat.width / 2), TOWORLD(plat.y + plat.height / 2)};

    b2BodyId groundId = b2CreateBody(world->world, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(TOWORLD(plat.width / 2), TOWORLD(plat.height / 2));

    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
	groundShapeDef.friction = 0.3f;
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    addToBag(world->bag, &groundId, STATIC_PLATFORM, plat.width, plat.height);
}

void phy_addWalls(WorldHandle world, Rectangle boundary, int wallThickness) {
    b2BodyDef wallLeftBodyDef = b2DefaultBodyDef();
    wallLeftBodyDef.position = (b2Vec2){TOWORLD((float)boundary.x + ((float)wallThickness / 2)), TOWORLD((float)boundary.y + ((float)boundary.height / 2))};

    b2BodyDef wallRightBodyDef = b2DefaultBodyDef();
    wallRightBodyDef.position = (b2Vec2){TOWORLD((float)boundary.x + (float)boundary.width - ((float)wallThickness / 2)), TOWORLD((float)boundary.y + ((float)boundary.height / 2))};

    b2BodyId wallLeftId = b2CreateBody(world->world, &wallLeftBodyDef);
    b2BodyId wallRightId = b2CreateBody(world->world, &wallRightBodyDef);
    b2Polygon wallBox = b2MakeBox(TOWORLD((float)wallThickness / 2), TOWORLD((float)boundary.height / 2));

    b2ShapeDef wallShapeDef = b2DefaultShapeDef();
	wallShapeDef.friction = 0.0f;
    b2CreatePolygonShape(wallLeftId, &wallShapeDef, &wallBox);
    b2CreatePolygonShape(wallRightId, &wallShapeDef, &wallBox);

    addToBag(world->bag, &wallLeftId, WALL, (float)wallThickness, (float)boundary.height);
    addToBag(world->bag, &wallRightId, WALL, (float)wallThickness, (float)boundary.height);
}

void phy_addPlayer(WorldHandle world) {
    b2BodyDef playerBodyDef = b2DefaultBodyDef();
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.fixedRotation = true;
    playerBodyDef.position = (b2Vec2){TOWORLD(40.0f), TOWORLD(5.0f)};

    b2BodyId playerId = b2CreateBody(world->world, &playerBodyDef);
    b2Polygon playerBox = b2MakeBox(TOWORLD(8.0f), TOWORLD(16.0f));

    b2ShapeDef playerShapeDef = b2DefaultShapeDef();
	playerShapeDef.density = 20.0f;
	playerShapeDef.friction = 0.1f;
    b2CreatePolygonShape(playerId, &playerShapeDef, &playerBox);

    addToBag(world->bag, &playerId, CHARACTER, 16.0f, 32.0f);
}

int phy_getBodyReferences(WorldHandle handle, BodyRectReference *bodyReferences, BodyType type) {
	Body **bodies = handle->bag;
	int amount = 0;
	// Loop through all objects in bag and assign the ones with needed type
	for (int i = 0; i < BAG_SIZE; i++) {
		if (bodies[i] != NULL && bodies[i]->type == type) {
			bodyReferences[amount].rectangle = &(bodies[i]->rectangle);
			bodyReferences[amount++].rotation = &(bodies[i]->rotation);
		}
	}
	return amount;
}

BodyIdReference phy_getCharacterBodyReference(WorldHandle handle) {
	Body **bodies = handle->bag;
	for (int i = 0; i < BAG_SIZE; i++) {
		if (bodies[i] != NULL && bodies[i]->type == CHARACTER) {
			return &(bodies[i]->body);
		}
	}
	return NULL;
}
