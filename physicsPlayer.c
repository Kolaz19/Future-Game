#include "include/physicsPlayer.h"

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/raylib/raylib.h"
#include <stdio.h>

static void slowDown(BodyIdReference body) {
    b2Vec2 velocity = b2Body_GetLinearVelocity(*body);
    b2Body_SetLinearVelocity(*body, (b2Vec2){velocity.x * 0.7f, velocity.y});
}

void pl_update(BodyIdReference body) {
    b2Vec2 forceToApply = {0.0f, 0.0f};
    b2Vec2 velocity = b2Body_GetLinearVelocity(*body);

    if (IsKeyDown(KEY_A)) {
        if (velocity.x > 0) {
            slowDown(body);
        }
        if (velocity.x == 0) {
            forceToApply.x = -1000.0f;
        } else {
            forceToApply.x = -1000.0f;
        }
    } else if (IsKeyDown(KEY_D)) {
        if (velocity.x < 0) {
            slowDown(body);
        }
        if (velocity.x == 0) {
            forceToApply.x = 1000.0f;
        } else {
            forceToApply.x = 1000.0f;
        }
    } else {
        slowDown(body);
    }

    if (velocity.x < 8 && velocity.x > -8) {
        b2Body_ApplyForceToCenter(*body, forceToApply, true);
    } else {
        b2Body_ApplyForceToCenter(*body, (b2Vec2){forceToApply.x * -1, 0.0f}, true);
	}
}
