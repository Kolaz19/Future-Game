#include "include/bodyBehavior.h"
#include "include/box2d/box2d.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"

#define RUNNING_FORCE 1000.0f
#define VELOCITY_LIMIT 8
#define JUMP_COOLDOWN_LIMIT 0.5f
#define JUMP_FORCE -200.0f
#define DYING_FALL_VELOCITY 25

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

void contactUpdate(UpdateData *updateData) {
    if (updateData->status == 0) {
        b2WorldId world = b2Body_GetWorld(*updateData->body);
        b2ContactEvents contactEvents = b2World_GetContactEvents(world);
        for (int i = 0; i < contactEvents.beginCount; ++i) {
            b2ContactBeginTouchEvent *beginEvent = contactEvents.beginEvents + i;
            if (beginEvent->shapeIdA.index1 == updateData->body->index1 ||
                beginEvent->shapeIdB.index1 == updateData->body->index1) {
				slogi("CONTACT");
            }
        }
    }
}

void playerUpdate(UpdateData *updateData) {
    b2Vec2 forceToApply = {0.0f, 0.0f};
    b2Vec2 velocity = b2Body_GetLinearVelocity(*updateData->body);
    slogt("Velocity of player: X:%f Y:%f", velocity.x, velocity.y);

    // Check for death by falling
    if (!b2Body_IsEnabled(*updateData->body)) {
        return;
    }
    if (velocity.y < 0.01f && previousPlayerVelocityY > DYING_FALL_VELOCITY) {
        b2Body_Disable(*updateData->body);
    } else {
        previousPlayerVelocityY = velocity.y;
    }

    // Only advance cooldown when player is on ground
    if (velocity.y > -0.1f && velocity.y < 0.1f) {
        updateData->timer += GetFrameTime();
    } else {
        updateData->timer = 0.0f;
    }

    // Jumping
    if (IsKeyDown(KEY_W) && velocity.y < 0.01f && velocity.y > -0.01f && updateData->timer > JUMP_COOLDOWN_LIMIT) {
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

void setUpdateFunction(int id, void (**update)(UpdateData *updateData)) {
    *update = &noUpdate;
    switch (id) {
    case 0:
        *update = &noUpdate;
        break;
    case 1:
        *update = &contactUpdate;
        break;
    case 99:
        *update = &playerUpdate;
        break;
    }
}
