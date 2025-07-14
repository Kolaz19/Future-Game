#include "include/physicsWorld.h"
#include "include/box2d/id.h"
#include "include/dynBodyDef.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "include/bodyBehavior.h"
#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/jointCreation.h"
#include "include/raylib/pi.h"
#include "include/slog.h"
#include "include/sounds.h"

#define CONV_VAL 20
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)

typedef struct {
    float x;
    float y;
} Vec2;

typedef struct Body {
    b2BodyId body;
    int id;                /// Further specification of BodyType, can be 0
    Rectangle rectangle;   /// Is always in pixel dimension
    Vec2 previousPosition; /// Needed to check if body was moved
    BodyType type;
    float rotation;
    UpdateData updateData; /// Is passed to update function every frame
    void (*update)(UpdateData *);
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

static void updatePlatformSound(WorldHandle handle) {

    b2ContactEvents contactEvents = b2World_GetContactEvents(handle->world);
    for (int i = 0; i < contactEvents.beginCount; i++) {
        b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
        b2BodyId bodyA = b2Shape_GetBody(beginEvent->shapeIdA);
        b2BodyId bodyB = b2Shape_GetBody(beginEvent->shapeIdB);
        // Sound flag can be set in bodyBehavior = No Sound
        bool *noSoundFlag = b2Body_GetUserData(bodyA);
        if (noSoundFlag == NULL) {
            noSoundFlag = b2Body_GetUserData(bodyB);
        }
        // Check if no player
        if (b2Body_GetShapeCount(bodyA) == 1 && b2Body_GetShapeCount(bodyB) == 1) {
            if (noSoundFlag == NULL) {
                sound_platforms();
            }
        }
    }

	// If we need constant contact in future
    /*
for (int i = 0; i < BAG_SIZE; i++) {
    if (handle->bag[i] != NULL && handle->bag[i]->type == DYNAMIC_PLATFORM) {
        b2ShapeId shape;
        b2Body_GetShapes(handle->bag[i]->body, &shape, 1);
        int cap = b2Shape_GetContactCapacity(shape);
        if (cap != 0 && ((handle->bag[i]->previousPosition.x != handle->bag[i]->rectangle.x) ||
                         (handle->bag[i]->previousPosition.y != handle->bag[i]->rectangle.y))) {
            sound_platformsMoving();
        }
    }
}
    */
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
            bodies[i]->id = id;

            // Initialize update function
            bodies[i]->updateData.status = UPDATE_STATUS_INIT;
            bodies[i]->updateData.timer = 0.0f;
            bodies[i]->updateData.counter = 0;
            bodies[i]->updateData.body = &bodies[i]->body;
            bodies[i]->updateData.modifier = setUpdateFunction(id, &bodies[i]->update);

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

static void setPreviousPosition(WorldHandle handle) {
    for (int i = 0; i < BAG_SIZE; i++) {
        if (handle->bag[i] != NULL) {
            handle->bag[i]->previousPosition.x = handle->bag[i]->rectangle.x;
            handle->bag[i]->previousPosition.y = handle->bag[i]->rectangle.y;
        }
    }
}

void phy_updateWorld(WorldHandle handle) {
    b2World_Step(handle->world, 1.0f / 60.0f, 4);
    for (int i = 0; i < BAG_SIZE; i++) {
        if (handle->bag[i] != NULL) {
            updateRectangle(handle->bag[i]);
            handle->bag[i]->update(
                &(handle->bag[i]->updateData));
        }
    }
    updatePlatformSound(handle);
    setPreviousPosition(handle);
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
    groundShapeDef.friction = 0.4f;
    b2ShapeId shapeId = b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
    b2Shape_SetRestitution(shapeId, 0.0f);
    b2Shape_EnableContactEvents(shapeId, true);

    addToBag(world->bag, &groundId, STATIC_PLATFORM, plat.width, plat.height, UNDEFINED);
}

void phy_addDynamic(WorldHandle world, Rectangle plat, int id) {
    b2BodyDef dynamicBodyDef = b2DefaultBodyDef();
    dynamicBodyDef.position = (b2Vec2){TOWORLD(plat.x + plat.width / 2), TOWORLD(plat.y + plat.height / 2)};
    dynamicBodyDef.type = b2_staticBody;
	dynamicBodyDef.linearDamping = 0.1f;

    b2BodyId dynamicId = b2CreateBody(world->world, &dynamicBodyDef);
    b2Polygon dynamicBox = b2MakeBox(TOWORLD(plat.width / 2), TOWORLD(plat.height / 2));

    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.friction = 0.5f;
    if (plat.width > 15.0f && plat.height > 15.0f) {
        dynamicShapeDef.density = 35.0f;
    } else {
        dynamicShapeDef.density = 65.0f;
    }
    b2ShapeId shapeId = b2CreatePolygonShape(dynamicId, &dynamicShapeDef, &dynamicBox);
    b2Shape_SetRestitution(shapeId, 0.0f);
    b2Shape_EnableContactEvents(shapeId, true);

    // Optional joint creation
    int (*create)(JointCreationContext *);
    if (setJointCreationFunction(id, &create)) {
        JointCreationContext jointContext = {
            .original = &dynamicId,
            .width = plat.width,
            .height = plat.height};
        int numberOfNewBodies = (*create)(&jointContext);
        // Add it to bag so it can be cleaned up
        // Values are not that important
        for (int i = 0; i < numberOfNewBodies; i++) {
            addToBag(world->bag, jointContext.new + i, STATIC_PLATFORM, 1, 1, UNDEFINED);
        }
    }

    addToBag(world->bag, &dynamicId, DYNAMIC_PLATFORM, plat.width, plat.height, id);
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
    b2Shape_EnableContactEvents(b2CreatePolygonShape(wallRightId, &wallShapeDef, &wallBox), true);
    b2Shape_EnableContactEvents(b2CreatePolygonShape(wallLeftId, &wallShapeDef, &wallBox), true);

    addToBag(world->bag, &wallLeftId, WALL, (float)wallThickness, (float)boundary.height, UNDEFINED);
    addToBag(world->bag, &wallRightId, WALL, (float)wallThickness, (float)boundary.height, UNDEFINED);
}

void phy_addPlayer(WorldHandle world, float posX, float posY) {
    b2BodyDef playerBodyDef = b2DefaultBodyDef();
    playerBodyDef.type = b2_dynamicBody;
    playerBodyDef.fixedRotation = true;
    playerBodyDef.position = (b2Vec2){TOWORLD(posX), TOWORLD(posY)};

    b2BodyId playerId = b2CreateBody(world->world, &playerBodyDef);

    b2ShapeDef playerShapeDef = b2DefaultShapeDef();
    playerShapeDef.density = 25.0f;
    playerShapeDef.friction = 0.1f;
    b2Capsule capsuleId;
    capsuleId.radius = TOWORLD(8.0f);
    capsuleId.center1.x = 0.0f;
    capsuleId.center1.y = TOWORLD(-8.0f);
    capsuleId.center2.x = 0.0f;
    capsuleId.center2.y = TOWORLD(8.0f);
    b2ShapeId shapeId = b2CreateCapsuleShape(playerId, &playerShapeDef, &capsuleId);

    b2Polygon footBox = b2MakeOffsetBox(TOWORLD(3.0f), TOWORLD(1.0f), (b2Vec2){0.0f, TOWORLD(16.0f)}, (b2Rot){1.0f, 0.0f});
    playerShapeDef.density = 0.0f;
    playerShapeDef.isSensor = true;
    b2ShapeId footShapeId = b2CreatePolygonShape(playerId, &playerShapeDef, &footBox);

    // Enable contact events for player
    b2Shape_EnableContactEvents(shapeId, true);
    b2Shape_EnableSensorEvents(footShapeId, true);
    b2Shape_SetRestitution(shapeId, 0.0f);

    addToBag(world->bag, &playerId, CHARACTER, 16.0f, 32.0f, PLAYER);
}

int phy_getBodyRectReferences(WorldHandle handle, BodyRectReference *bodyReferences, BodyType type) {
    Body **bodies = handle->bag;
    int amount = 0;
    // Loop through all objects in bag and assign the ones with needed type
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] != NULL && bodies[i]->type == type) {
            bodyReferences[amount].rectangle = &(bodies[i]->rectangle);
            bodyReferences[amount].rotation = &(bodies[i]->rotation);
            bodyReferences[amount++].id = (bodies[i]->id);
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
    sloge("No player body in bag");
    return NULL;
}

void phy_getVelocity(BodyIdReference body, float *velX, float *velY) {
    b2Vec2 vec = b2Body_GetLinearVelocity(*body);
    *velX = vec.x;
    *velY = vec.y;
}

void phy_setPosition(BodyIdReference body, float posX, float posY) {
    b2Body_SetTransform(*body, (b2Vec2){TOWORLD(posX), TOWORLD(posY)}, (b2Rot){1.0f, 0.0f});
    b2Body_SetLinearVelocity(*body, (b2Vec2){0.0f, 0.0f});
}

/*
 * Platforms and dynamic platforms have enabled contact events
 * Not walls
 */
void phy_updateDynamicGroundContact(BodyIdReference body, int *amountGroundContact) {
    b2WorldId world = b2Body_GetWorld(*body);
    b2ShapeId shapes[MAX_SHAPES_ATTACHED_TO_BODY];
    b2ShapeId *sensorShape = NULL;
    // Get foot sensor of character
    int shapesStored = b2Body_GetShapes(*body, shapes, b2Body_GetShapeCount(*body));
    for (int i = 0; i < shapesStored; i++) {
        if (b2Shape_IsSensor(shapes[i])) {
            sensorShape = shapes + i;
        }
    }
    assert(sensorShape != NULL);

    b2SensorEvents sensorEvents = b2World_GetSensorEvents(world);

    for (int i = 0; i < sensorEvents.endCount; i++) {
        b2SensorEndTouchEvent *endEvent = sensorEvents.endEvents + i;
        if (B2_ID_EQUALS(endEvent->sensorShapeId, (*sensorShape))) {
            (*amountGroundContact)--;
            slogt("Player dynamic body contact amount updated: %d", *amountGroundContact);
        }
    }

    for (int i = 0; i < sensorEvents.beginCount; i++) {
        b2SensorBeginTouchEvent *beginEvent = sensorEvents.beginEvents + i;
        if (B2_ID_EQUALS(beginEvent->sensorShapeId, (*sensorShape))) {
            (*amountGroundContact)++;
            slogt("Player dynamic body contact amount updated: %d", *amountGroundContact);
        }
    }
    assert((*amountGroundContact) >= 0);
}

void phy_disablePlayer(WorldHandle handle) {
    Body **bodies = handle->bag;
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] != NULL && bodies[i]->type == CHARACTER) {
            bodies[i]->updateData.status = UPDATE_STATUS_DISABLE;
        }
    }
}

bool phy_isPlayerDead(WorldHandle handle) {
    Body **bodies = handle->bag;
    for (int i = 0; i < BAG_SIZE; i++) {
        if (bodies[i] != NULL && bodies[i]->type == CHARACTER) {
            if (bodies[i]->updateData.status == UPDATE_STATUS_DEAD) {
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}
