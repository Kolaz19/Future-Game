#include "include/physicsWorld.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/bodyBehavior.h"
#include "include/raylib/pi.h"
#include "include/slog.h"

#define CONV_VAL 20
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)

/// Update function is performed every frame
typedef struct {
    int id; /// Further specification of BodyType, can be 0
    float status; /// For tracking time or other status saves
    void (*update)(b2BodyId *, float *);
} UpdateEntity;

typedef struct Body {
    b2BodyId body;
    Rectangle rectangle; /// Is always in pixel dimension
    BodyType type;
    float rotation;
    UpdateEntity uEntity;
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
 * @param name ID of body, relevant for dynamic bodies
 */
static void addToBag(Body **bodies, b2BodyId *entry, BodyType type, float width, float height, int id) {
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

            // Initialize update function
            bodies[i]->uEntity.id = id;
            bodies[i]->uEntity.status = 0.0f;
            setUpdateFunction(id, &bodies[i]->uEntity.update);

            // Initially set position and rotation
            updateRectangle(bodies[i]);
            slogd("Body in bag at index [%d] added (TYPE:%d ID:%d)", i, type, id);
            return;
        }
    }
}

void phy_destroyObjectsAbove(WorldHandle handle, float posY) {
    BodyType type;
    for (int i = 0; i < BAG_SIZE; i++) {
        if (handle->bag[i] != NULL && handle->bag[i]->rectangle.y < posY) {
            b2DestroyBody(handle->bag[i]->body);
            type = handle->bag[i]->type;
            free(handle->bag[i]);
            handle->bag[i] = NULL;
            slogd("Body in bag at index [%d] freed (TYPE:%d)", i, type);
        }
    }
}

void phy_updateWorld(WorldHandle handle) {
    b2World_Step(handle->world, 1.0f / 60.0f, 4);
    for (int i = 0; i < BAG_SIZE; i++) {
        if (handle->bag[i] != NULL) {
            updateRectangle(handle->bag[i]);
            handle->bag[i]->uEntity.update(
                &(handle->bag[i]->body),
                &(handle->bag[i]->uEntity.status));
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
            slogd("Body in bag at index [%d] freed (TYPE:%d)", i, type);
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

    addToBag(world->bag, &groundId, STATIC_PLATFORM, plat.width, plat.height, 0);
}

void phy_addDynamic(WorldHandle world, Rectangle plat, int id) {
    b2BodyDef dynamicBodyDef = b2DefaultBodyDef();
    dynamicBodyDef.position = (b2Vec2){TOWORLD(plat.x + plat.width / 2), TOWORLD(plat.y + plat.height / 2)};

    b2BodyId dynamicId = b2CreateBody(world->world, &dynamicBodyDef);
    b2Polygon dynamicBox = b2MakeBox(TOWORLD(plat.width / 2), TOWORLD(plat.height / 2));

    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.friction = 0.3f;
    b2CreatePolygonShape(dynamicId, &dynamicShapeDef, &dynamicBox);

    addToBag(world->bag, &dynamicId, DYNAMIC_PLATFORM, plat.width, plat.height, id);
}

void phy_addWalls(WorldHandle world, Rectangle boundary, int wallThickness) {
    b2BodyDef wallLeftBodyDef = b2DefaultBodyDef();
    wallLeftBodyDef.position = (b2Vec2){TOWORLD((float)boundary.x + ((float)wallThickness / 2)), TOWORLD((float)boundary.y + ((float)boundary.height / 2))};

    b2BodyDef wallRightBodyDef = b2DefaultBodyDef();
    wallRightBodyDef.position = (b2Vec2){TOWORLD((float)boundary.x + (float)boundary.width - ((float)wallThickness / 2)), TOWORLD((float)boundary.y + ((float)boundary.height / 2))};

    b2BodyId wallLeftId = b2CreateBody(world->world, &wallLeftBodyDef);
    b2BodyId wallRightId = b2CreateBody(world->world, &wallRightBodyDef);
    // TODO Clean transition between two walls
    b2Polygon wallBox = b2MakeBox(TOWORLD((float)wallThickness / 2), TOWORLD((float)boundary.height / 2));

    b2ShapeDef wallShapeDef = b2DefaultShapeDef();
    wallShapeDef.friction = 0.0f;
    b2CreatePolygonShape(wallLeftId, &wallShapeDef, &wallBox);
    b2CreatePolygonShape(wallRightId, &wallShapeDef, &wallBox);

    addToBag(world->bag, &wallLeftId, WALL, (float)wallThickness, (float)boundary.height, 0);
    addToBag(world->bag, &wallRightId, WALL, (float)wallThickness, (float)boundary.height, 0);
}

void phy_addPlayer(WorldHandle world) {
    b2BodyDef playerBodyDef = b2DefaultBodyDef();
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.fixedRotation = true;
    playerBodyDef.position = (b2Vec2){TOWORLD(40.0f), TOWORLD(150.0f)};

    b2BodyId playerId = b2CreateBody(world->world, &playerBodyDef);
    b2Polygon playerBox = b2MakeBox(TOWORLD(8.0f), TOWORLD(16.0f));

    b2ShapeDef playerShapeDef = b2DefaultShapeDef();
    playerShapeDef.density = 20.0f;
    playerShapeDef.friction = 0.1f;
    b2CreatePolygonShape(playerId, &playerShapeDef, &playerBox);

    addToBag(world->bag, &playerId, CHARACTER, 16.0f, 32.0f, 0);
}

int phy_getBodyRectReferences(WorldHandle handle, BodyRectReference *bodyReferences, BodyType type) {
    Body **bodies = handle->bag;
    int amount = 0;
    // Loop through all objects in bag and assign the ones with needed type
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] != NULL && bodies[i]->type == type) {
            bodyReferences[amount].rectangle = &(bodies[i]->rectangle);
            bodyReferences[amount].rotation = &(bodies[i]->rotation);
            bodyReferences[amount].id = (bodies[i]->uEntity.id);
        }
    }
    return amount;
}

BodyIdReference phy_getCharacterBodyIdReference(WorldHandle handle) {
    Body **bodies = handle->bag;
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] != NULL && bodies[i]->type == CHARACTER) {
            return &(bodies[i]->body);
        }
    }
    return NULL;
}
