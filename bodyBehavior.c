#include "include/bodyBehavior.h"
#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/id.h"
#include "include/box2d/types.h"
#include "include/dynBodyDef.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"

#define RUNNING_FORCE 1000.0f
#define VELOCITY_LIMIT 8
#define MOVEMENT_BLOCK_THRESHOLD 0.01
#define JUMP_COOLDOWN_LIMIT 0.5f
#define JUMP_FORCE -220.0f
#define DYING_FALL_VELOCITY 23

#define STATUS_INIT UPDATE_STATUS_INIT
#define STATUS_DEAD UPDATE_STATUS_DEAD
#define STATUS_CONTACT 1
#define STATUS_WAS_CONTACT 2
#define STATUS_FREE_FALL 3
#define STATUS_UNSTABLE 4
#define STATUS_SMALL_STABLE 5
#define STATUS_LOCK_IN_PLACE 6
#define STATUS_JUMP 7

// typedef enum PlayerMovement {
// NONE,
// LEFT_MOVING,
// RIGHT_MOVING,
//}PlayerMovement;

static float previousPlayerVelocityY = 0.0f;
static float previousPlayerPosX = 0.0f;
static float previousPlayerPosY = 0.0f;
static bool previousMovement = false;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void noUpdate(UpdateData *updateData) { return; }
#pragma GCC diagnostic pop

/*
 * Slow down running speed
 */
static void slowDown(b2BodyId *body) {
    b2Vec2 velocity = b2Body_GetLinearVelocity(*body);
    b2Body_SetLinearVelocity(*body, (b2Vec2){velocity.x * 0.7f, velocity.y});
}

static void updateDynamicGroundContact(b2BodyId *body, int *amountGroundContact) {
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
}

static bool contactBegin(b2BodyId *body) {
    b2WorldId world = b2Body_GetWorld(*body);
    b2ShapeId shapes[MAX_SHAPES_ATTACHED_TO_BODY];
    int shapesStored = b2Body_GetShapes(*body, shapes, b2Body_GetShapeCount(*body));
    b2ContactEvents contactEvents = b2World_GetContactEvents(world);
    // Loop through shape events
    for (int i = 0; i < contactEvents.beginCount; i++) {
        b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
        // Loop through all shapes of body
        for (int k = 0; k < shapesStored; k++) {
            if (B2_ID_EQUALS(beginEvent->shapeIdA, shapes[k]) ||
                B2_ID_EQUALS(beginEvent->shapeIdB, shapes[k])) {
                return true;
            }
        }
    }
    return false;
}

/*
 * First contact makes platform unstable
 * It then moves a bit until it locks in place
 */
void shiftLittleUpdate(UpdateData *updateData) {
    if (updateData->status == STATUS_LOCK_IN_PLACE)
        return;

    const float unstableTime = 0.2f;
    if (updateData->status == STATUS_INIT && contactBegin(updateData->body)) {
        updateData->status = STATUS_CONTACT;
    }

    switch (updateData->status) {
    case STATUS_CONTACT:
        b2Body_SetType(*updateData->body, b2_dynamicBody);
        if (updateData->modifier == LEFT) {
            b2Body_ApplyTorque(*updateData->body, -30000.0f, true);
        }
        updateData->status = STATUS_UNSTABLE;
        break;
    case STATUS_UNSTABLE:
        if (updateData->timer < unstableTime) {
            updateData->timer += GetFrameTime();
        } else {
            b2Body_SetType(*updateData->body, b2_staticBody);
            updateData->status = STATUS_LOCK_IN_PLACE;
        }
        break;
    }
}

void fallFromStartUpdate(UpdateData *updateData) {
    switch (updateData->status) {
    case STATUS_INIT:
        b2Body_SetType(*updateData->body, b2_dynamicBody);
        updateData->status = STATUS_FREE_FALL;
        break;
    }
}

/*
 * Just break free when touched
 */
void justFallOnCollisionUpdate(UpdateData *updateData) {
    if (updateData->status == STATUS_FREE_FALL)
        return;

    if (contactBegin(updateData->body)) {
        b2Body_SetType(*updateData->body, b2_dynamicBody);
        updateData->status = STATUS_FREE_FALL;
    }
}

/*
 * First contact makes platform unstable
 * It then moves a bit until locking in place again
 * Then time goes by until it completely breaks free
 */
void unstableUpdate(UpdateData *updateData) {
    if (updateData->status == STATUS_FREE_FALL)
        return;

	//Wait a bit before contact checking
    if (updateData->modifier == WAIT) {
		if (updateData->timer < 1.0f) {
            updateData->timer += GetFrameTime();
		} else {
            updateData->timer = 0.0f;
			updateData->modifier = DEFAULT;
		}
		return;
    }

    const float unstableTime = 0.1f;
    const float smallStableTime = 1.4f;
    if (updateData->status == STATUS_INIT && contactBegin(updateData->body)) {
        updateData->status = STATUS_CONTACT;
    }

    switch (updateData->status) {
    case STATUS_CONTACT:
        b2Body_SetType(*updateData->body, b2_dynamicBody);
        updateData->status = STATUS_UNSTABLE;
        break;
    case STATUS_UNSTABLE:
        if (updateData->timer < unstableTime) {
            updateData->timer += GetFrameTime();
        } else {
            b2Body_SetType(*updateData->body, b2_staticBody);
            updateData->status = STATUS_SMALL_STABLE;
            updateData->timer = 0.0f;
        }
        break;
    case STATUS_SMALL_STABLE:
        if (updateData->timer < smallStableTime) {
            updateData->timer += GetFrameTime();
        } else {
            updateData->status = STATUS_FREE_FALL;
            b2Body_SetType(*updateData->body, b2_dynamicBody);
        }
        break;
    }
}

void playerUpdate(UpdateData *updateData) {
    b2Vec2 forceToApply = {0.0f, 0.0f};
    b2Vec2 velocity = b2Body_GetLinearVelocity(*updateData->body);
    b2Vec2 pos = b2Body_GetPosition(*updateData->body);
    bool enableMovement = true;
    slogt("Velocity of player: X:%f Y:%f", velocity.x, velocity.y);

    // Check for death by falling
    if (updateData->status == STATUS_DEAD) {
        return;
    }

    if (IsKeyDown(KEY_R)) {
        previousPlayerVelocityY = 0.0f;
    }

    if (velocity.y < (DYING_FALL_VELOCITY / 2.0f) && previousPlayerVelocityY > DYING_FALL_VELOCITY) {
        updateData->status = STATUS_DEAD;
        updateData->timer = 0.0f;
        previousPlayerVelocityY = 0.0f;

        // Collision box has to be changed when player dies
        b2ShapeId shapeId[2];
        b2Body_GetShapes(*updateData->body, shapeId, 2);
        for (int i = 0; i < 2; i++) {
            if (!b2Shape_IsSensor(shapeId[i])) {
                b2Polygon playerBox = b2MakeOffsetBox(0.5f, 0.15f, (b2Vec2){0.0f, 0.65f}, (b2Rot){1.0f, 0.0f});
                b2Shape_SetPolygon(shapeId[i], &playerBox);
            }
        }
        return;
    }

    // Track if player is jumping
    updateDynamicGroundContact(updateData->body, &(updateData->counter));
    if (updateData->counter > 0) {
        updateData->status = STATUS_INIT;
    } else {
        updateData->status = STATUS_JUMP;
    }

    // Should player be able to move when on slope?
    if (previousPlayerPosX > pos.x - MOVEMENT_BLOCK_THRESHOLD && previousPlayerPosX < pos.x + MOVEMENT_BLOCK_THRESHOLD &&
        previousPlayerPosY > pos.y - MOVEMENT_BLOCK_THRESHOLD && previousPlayerPosY < pos.y + MOVEMENT_BLOCK_THRESHOLD &&
        previousMovement && updateData->counter == 0) {
        enableMovement = false;
    }
    previousPlayerVelocityY = velocity.y;

    // Only advance cooldown when player is on ground
    // or when player is stuck in jumping animation (between two objects jumping)
    if (updateData->status == STATUS_INIT || (previousPlayerPosX == pos.x && previousPlayerPosY == pos.y)) {
        updateData->timer += GetFrameTime();
    } else {
        updateData->timer = 0.0f;
    }

    previousPlayerPosX = pos.x;
    previousPlayerPosY = pos.y;

    if (IsKeyDown(KEY_W) && updateData->timer > JUMP_COOLDOWN_LIMIT) {
        b2Body_ApplyLinearImpulse(*updateData->body, (b2Vec2){0.0f, JUMP_FORCE}, (b2Vec2){0.0f, 0.0f}, true);
        updateData->timer = 0.0f;
    }

    // Slow down when starting running in opposite direction
    // or when coming to halt
    if (IsKeyDown(KEY_A) && enableMovement) {
        if (velocity.x > 0) {
            slowDown(updateData->body);
        }
        previousMovement = true;
        forceToApply.x = RUNNING_FORCE * -1;
    } else if (IsKeyDown(KEY_D) && enableMovement) {
        if (velocity.x < 0) {
            slowDown(updateData->body);
        }
        previousMovement = true;
        forceToApply.x = RUNNING_FORCE;
    } else {
        previousMovement = false;
        slowDown(updateData->body);
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
        previousMovement = true;
    }

    if (velocity.x < VELOCITY_LIMIT && velocity.x > (VELOCITY_LIMIT * -1)) {
        b2Body_ApplyForceToCenter(*updateData->body, forceToApply, true);
    } else {
        b2Body_ApplyForceToCenter(*updateData->body, (b2Vec2){forceToApply.x * -1, 0.0f}, true);
    }
}

DynBodyUpdateModifier setUpdateFunction(int id, void (**update)(UpdateData *updateData)) {
    *update = &noUpdate;
    switch (id) {
    case UNDEFINED:
        *update = &noUpdate;
        break;
    case BASIC_96X16:
        *update = &shiftLittleUpdate;
        return LEFT;
        break;
    case THINNER_BROKEN_112X16:
    case SLIDER_128X16:
        *update = &justFallOnCollisionUpdate;
        break;
    case CIRCLES_BROKEN_32X32:
    case CIRCLES_32X16:
    case THIN_NO_END_80X16:
    case THIN_END_112X16:
    case BIG_UPPER_BLOCK_64X112:
    case THIN_END_144X16:
    case JOINT_ONLY_RIGHT_208X16:
    case ANKERED_144x16:
        *update = &unstableUpdate;
        break;
    case ANKERED_160x16:
        *update = &unstableUpdate;
        return WAIT;
        break;
    case ENERGY_BOXC_20x20:
    case STANDING_CAGE_16x176:
        *update = &fallFromStartUpdate;
        break;
    case PLAYER:
        *update = &playerUpdate;
        break;
    }
    return DEFAULT;
}
