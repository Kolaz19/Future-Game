#include "include/box2dWrapper.h"
#include <assert.h>
#include <stdlib.h>

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/raylib/pi.h"

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
			return;
        }
    }
}

void updateWorld(WorldHandle handle) {
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
    worldDef.gravity = (b2Vec2){0.0f, 50.0f};

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
            free(bodies[i]);
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
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    addToBag(world->bag, &groundId, STATIC_PLATFORM, plat.width, plat.height);
}

int phy_getBodyReferences(WorldHandle handle, BodyReference *bodyReferences, BodyType type) {
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
