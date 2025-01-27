#include "include/physicsPlayer.h"

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"
#include "include/raylib/raylib.h"

void pl_update(BodyIdReference body) {
    b2Vec2 vec = {0.0f, 0.0f};

    if (IsKeyDown(KEY_A)) {
        vec.x = -300.0f;
    } else if (IsKeyDown(KEY_D)) {
        vec.x = 300.0f;
    }

    b2Body_ApplyForceToCenter(*body, vec, true);
}
