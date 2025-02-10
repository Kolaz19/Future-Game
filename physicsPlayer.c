#include "include/physicsPlayer.h"

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"

#define RUNNING_FORCE 1000.0f
#define VELOCITY_LIMIT 8
#define JUMP_FORCE -250.0f

/*
 * Slow down running speed
 */
static void slowDown(BodyIdReference body) {
    b2Vec2 velocity = b2Body_GetLinearVelocity(*body);
    b2Body_SetLinearVelocity(*body, (b2Vec2){velocity.x * 0.7f, velocity.y});
}

void plphy_update(BodyIdReference body) {
    b2Vec2 forceToApply = {0.0f, 0.0f};
    b2Vec2 velocity = b2Body_GetLinearVelocity(*body);
	slogt("Velocity of player: X:%f Y:%f", velocity.x, velocity.y);

	if (IsKeyDown(KEY_W) && velocity.y < 0.01f && velocity.y > -0.01f) {
		b2Body_ApplyLinearImpulse(*body, (b2Vec2){0.0f, JUMP_FORCE}, (b2Vec2){0.0f, 0.0f}, true);
	}

	//Slow down when starting running in opposite direction
	//or when coming to halt
    if (IsKeyDown(KEY_A)) {
        if (velocity.x > 0) {
            slowDown(body);
        }
        forceToApply.x = RUNNING_FORCE * -1;
    } else if (IsKeyDown(KEY_D)) {
        if (velocity.x < 0) {
            slowDown(body);
        }
        forceToApply.x = RUNNING_FORCE;
    } else {
        slowDown(body);
    }

    if (velocity.x < VELOCITY_LIMIT && velocity.x > (VELOCITY_LIMIT * -1)) {
        b2Body_ApplyForceToCenter(*body, forceToApply, true);
    } else {
        b2Body_ApplyForceToCenter(*body, (b2Vec2){forceToApply.x * -1, 0.0f}, true);
    }
}

void plphy_getVelocity(BodyIdReference body, float *velX, float *velY) {
    b2Vec2 vec = b2Body_GetLinearVelocity(*body);
    *velX = vec.x;
    *velY = vec.y;
}
