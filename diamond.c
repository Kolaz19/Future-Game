#include "include/diamond.h"
#include "include/animation.h"
#include "include/diamondParticles.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define FILE_NAME "assets/YellowDiamond.png"
#define START_X 140.f
#define START_Y 140.f
#define BASELINE_RADIUS 7.0f
#define UP_DOWN_SPEED 0.1f
#define UP_DOWN_MIN_SPEED 0.03f
#define DESTINATION_WIDTH (64.0 * 2.0f)
#define DESTINATION_HEIGHT (64.0 * 2.0f)
#define TRAVEL_DISTANCE 100.0f
#define TRAVEL_SPEED 0.4f

typedef struct DiamondUpDownMovement {
    float basePosY;
    bool up;
} DiamondUpDownMovement;

struct DiamondData {
    // Real presentation of diamond core
    Rectangle rectangle;
    float absorbingPosXShift;
    Spritesheet sheet;
    Animation animation;
    DiamondUpDownMovement upDownMovement;
    DStatus status;
    ParticleHandler particles;
    float particlesLifetime;
};

Diamond dia_createDiamond(float startX, float startY) {
    Diamond diamond = malloc(sizeof(struct DiamondData));
    diamond->sheet = anim_loadSpritesheet(FILE_NAME, 4, 1);
    diamond->animation = anim_createAnimation(&diamond->sheet, 1, 4, 0.2f, LOOP);
    diamond->rectangle.x = startX;
    diamond->rectangle.y = startY;
    diamond->upDownMovement.basePosY = startY;
    diamond->upDownMovement.up = true;
    diamond->rectangle.width = 10.0f * 2.0f;
    diamond->rectangle.height = 22.0 * 2.0f;
    diamond->status = INIT;
    diamond->absorbingPosXShift = diamond->rectangle.x - TRAVEL_DISTANCE;
    diamond->particles = NULL;
    diamond->particlesLifetime = 0.0f;
    return diamond;
}

void dia_setPos(Diamond diamond, float posX, float posY) {
    diamond->rectangle.x = posX;
    diamond->rectangle.y = posY;
}

void dia_free(Diamond diamond) {
    anim_unloadSpritesheet(&diamond->sheet);
	if (diamond->particles != NULL) {
		diap_free(diamond->particles);
	}
    free(diamond);
}

static void moveUpAndDown(float *posY, DiamondUpDownMovement *movement) {
    // Get distance
    float distanceToTravel = *posY - movement->basePosY;
    if (distanceToTravel < 0) distanceToTravel = distanceToTravel * -1;
    // Get percentage (Closer to base = higher)
    distanceToTravel = (BASELINE_RADIUS - distanceToTravel) * 100 / BASELINE_RADIUS;
    // Calculate real value
    distanceToTravel = distanceToTravel * UP_DOWN_SPEED * GetFrameTime();
    // Never go below this value
    if (distanceToTravel < UP_DOWN_MIN_SPEED) distanceToTravel = UP_DOWN_MIN_SPEED;

    if (movement->up) {
        *posY -= distanceToTravel;
        if (*posY < movement->basePosY - BASELINE_RADIUS) movement->up = false;
    } else {
        *posY += distanceToTravel;
        if (*posY > movement->basePosY + BASELINE_RADIUS) movement->up = true;
    }
}

DStatus dia_update(Diamond diamond, Rectangle *player) {
    switch (diamond->status) {
    case INIT:
        moveUpAndDown(&diamond->rectangle.y, &diamond->upDownMovement);
        if (CheckCollisionRecs(diamond->rectangle, *player)) diamond->status = ABSORBING_POSITIONING;
        break;
    case ABSORBING_POSITIONING:
        moveUpAndDown(&diamond->rectangle.y, &diamond->upDownMovement);
        if (diamond->rectangle.x > diamond->absorbingPosXShift) {
            diamond->rectangle.x -= TRAVEL_SPEED;
        } else {
            diamond->status = ABSORBING;
            diamond->particles = diap_init((int)player->x, (int)player->y, (int)(player->y + player->height));
        }
        break;
    case ABSORBING:
        moveUpAndDown(&diamond->rectangle.y, &diamond->upDownMovement);
        diap_update(diamond->particles,
                    diamond->particlesLifetime,
                    (int)diamond->rectangle.x + 10,
                    (int)diamond->rectangle.y + 22);
        diamond->particlesLifetime += GetFrameTime();
        break;
    case FREE:
        diamond->rectangle.y -= 0.5f;
        break;
    }

    anim_advanceAnimation(&diamond->animation);
    return diamond->status;
}

void dia_draw(Diamond diamond) {
    anim_drawAnimation(&diamond->animation,
                       &(Rectangle){diamond->rectangle.x, diamond->rectangle.y, DESTINATION_WIDTH, DESTINATION_HEIGHT},
                       &(Vector2){56.0f, 44.0f}, 0.0f);
    if (diamond->status == ABSORBING) {
        diap_drawParticles(diamond->particles);
    }
    // DrawRectangle((int)diamond->rectangle.x, (int)diamond->rectangle.y, 10*2, 22*2, BLUE);
}

DStatus dia_getStatus(Diamond diamond) {
    return diamond->status;
}
