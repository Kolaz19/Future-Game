#include "include/bodyBehavior.h"
#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/types.h"
#include "include/dynBodyDef.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"

#define RUNNING_FORCE 1000.0f
#define VELOCITY_LIMIT 8
#define JUMP_COOLDOWN_LIMIT 0.5f
#define JUMP_FORCE -200.0f
#define DYING_FALL_VELOCITY 22

#define STATUS_INIT UPDATE_STATUS_INIT
#define STATUS_DEAD UPDATE_STATUS_DEAD
#define STATUS_CONTACT 1
#define STATUS_WAS_CONTACT 2
#define STATUS_FREE_FALL 3
#define STATUS_UNSTABLE 4
#define STATUS_SMALL_STABLE 5
#define STATUS_LOCK_IN_PLACE 6
#define STATUS_JUMP 7

static float previousPlayerVelocityY = 0.0f;

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

static bool contactBegin(b2BodyId *body) {
    b2WorldId world = b2Body_GetWorld(*body);
    b2ContactEvents contactEvents = b2World_GetContactEvents(world);
    for (int i = 0; i < contactEvents.beginCount; ++i) {
        b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
        if (beginEvent->shapeIdA.index1 == body->index1 ||
            beginEvent->shapeIdB.index1 == body->index1) {
            return true;
        }
    }
    return false;
}

static bool contactEnd(b2BodyId *body) {
    b2WorldId world = b2Body_GetWorld(*body);
    b2ContactEvents contactEvents = b2World_GetContactEvents(world);
    for (int i = 0; i < contactEvents.endCount; ++i) {
        b2ContactEndTouchEvent *endEvent = contactEvents.endEvents + i;
        if (endEvent->shapeIdA.index1 == body->index1 ||
            endEvent->shapeIdB.index1 == body->index1) {
            return true;
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

/*
 * Just break free when touched
 */
void justFallOnCollisionUpdate(UpdateData *updateData) {
	if (updateData->status == STATUS_FREE_FALL) return;

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
    slogt("Velocity of player: X:%f Y:%f", velocity.x, velocity.y);

    // Check for death by falling
    if (updateData->status == STATUS_DEAD) {
        return;
    }
    if (velocity.y < 0.01f && previousPlayerVelocityY > DYING_FALL_VELOCITY) {
        updateData->status = STATUS_DEAD;
        updateData->timer = 0.0f;
        previousPlayerVelocityY = 0.0f;

		//Collision box has to be changed when player dies
        b2ShapeId shapeId;
        b2Body_GetShapes(*updateData->body, &shapeId, 1);
        b2Polygon playerBox = b2MakeOffsetBox(0.5f, 0.15f, (b2Vec2){0.0f, 0.65f}, (b2Rot){1.0f, 0.0f});
        b2Shape_SetPolygon(shapeId, &playerBox);
        return;
    } else {
        previousPlayerVelocityY = velocity.y;
    }

    // Track if player is jumping
    if (updateData->status == STATUS_JUMP && contactBegin(updateData->body)) {
        updateData->status = STATUS_INIT;
    } else if (contactEnd(updateData->body)) {
        updateData->status = STATUS_JUMP;
    }

    // Only advance cooldown when player is on ground
    if ((velocity.y > -0.1f && velocity.y < 0.1f) || updateData->status == STATUS_INIT) {
        updateData->timer += GetFrameTime();
    } else {
        updateData->timer = 0.0f;
    }

    // Jumping - either static or dynamic platform
    bool groundContact = (velocity.y < 0.01f && velocity.y > -0.01f) || updateData->status == STATUS_INIT;
    if (IsKeyDown(KEY_W) && groundContact && updateData->timer > JUMP_COOLDOWN_LIMIT) {
        b2Body_ApplyLinearImpulse(*updateData->body, (b2Vec2){0.0f, JUMP_FORCE}, (b2Vec2){0.0f, 0.0f}, true);
        updateData->timer = 0.0f;
    }

    // Slow down when starting running in opposite direction
    // or when coming to halt
    if (IsKeyDown(KEY_A)) {
        if (velocity.x > 0) {
            slowDown(updateData->body);
        }
        forceToApply.x = RUNNING_FORCE * -1;
    } else if (IsKeyDown(KEY_D)) {
        if (velocity.x < 0) {
            slowDown(updateData->body);
        }
        forceToApply.x = RUNNING_FORCE;
    } else {
        slowDown(updateData->body);
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
	case JOINT_ONLY_RIGHT_208X16:
	case THINNER_BROKEN_112X16:
		*update = &justFallOnCollisionUpdate;
		break;
    case CIRCLES_BROKEN_32X32:
    case CIRCLES_32X16:
	case THIN_NO_END_80X16:
	case THIN_END_112X16:
	case THIN_END_144X16:
		*update = &unstableUpdate;
		break;
    case PLAYER:
        *update = &playerUpdate;
        break;
    }
    return DEFAULT;
}
