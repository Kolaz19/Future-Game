#include "include/box2dWrapper.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"

#define CONV_VAL 20
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)

#ifndef PI
#define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
#define DEG2RAD (PI / 180.0f)
#endif
#ifndef RAD2DEG
#define RAD2DEG (180.0f / PI)
#endif


typedef struct Body {
    b2BodyId body;
    Rectangle rectangle;
    BodyType type;
    float rotation;
} Body;

struct World {
    b2WorldId world;
    Body *bodies[BAG_SIZE];
};

static void updateRectangle(Body *body) {
    assert(body != NULL);
    b2Vec2 vec = b2Body_GetPosition(body->body);
    body->rectangle.x = TOPIXEL(vec.x) - (body->rectangle.width / 2);
    body->rectangle.y = TOPIXEL(vec.y) - (body->rectangle.height / 2);
    body->rotation = RAD2DEG * b2Rot_GetAngle(b2Body_GetRotation(body->body));
}

static void addToBag(Body **bodies, b2BodyId *entry, BodyType type, float width, float height) {
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] == NULL) {
            bodies[i] = malloc(sizeof(Body));
            bodies[i]->body.world0 = entry->world0;
            bodies[i]->body.revision = entry->revision;
            bodies[i]->body.index1 = entry->index1;

            bodies[i]->rectangle.width = width;
            bodies[i]->rectangle.height = height;
			bodies[i]->type = type;
            updateRectangle(bodies[i]);
			return;
        }
    }
}

void updateWorld(WorldHandle handle) {
    b2World_Step(handle->world, 1.0f / 60.0f, 4);
    for (int i = 0; i < BAG_SIZE; i++) {
        if (handle->bodies[i] != NULL) {
            updateRectangle(handle->bodies[i]);
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
        world->bodies[i] = NULL;
    }
    return world;
}

void phy_free(WorldHandle handle) {
    Body **bodies = handle->bodies;

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

    addToBag(world->bodies, &groundId, STATIC_PLATFORM, plat.width, plat.height);
}

int phy_getRectangles(WorldHandle handle, Rectangle **rectangles, BodyType type) {
	Body **bodies = handle->bodies;
	int amount = 0;
	for (int i = 0; i < BAG_SIZE; i++) {
		if (bodies[i] != NULL && bodies[i]->type == type) {
			rectangles[amount++] = &(bodies[i]->rectangle);
		}
	}
	return amount;
}
