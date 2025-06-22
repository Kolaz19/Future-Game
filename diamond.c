#include "include/diamond.h"
#include "include/animation.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define FILE_NAME "assets/YellowDiamond.png"
#define START_X 140.f
#define START_Y 140.f
#define BASELINE_RADIUS 7.0f
#define UP_DOWN_SPEED 0.3f
#define UP_DOWN_MIN_SPEED 0.03f

typedef struct DiamondUpDownMovement {
    float basePosY;
    bool up;
} DiamondUpDownMovement;

struct DiamondData {
    Rectangle pos;
    Spritesheet sheet;
    Animation animation;
    bool wasTouched;
    DiamondUpDownMovement upDownMovement;
};

Diamond dia_createDiamond(float startX, float startY) {
    Diamond diamond = malloc(sizeof(struct DiamondData));
    diamond->sheet = anim_loadSpritesheet(FILE_NAME, 4, 1);
    diamond->animation = anim_createAnimation(&diamond->sheet, 1, 4, 0.2f, LOOP);
    diamond->pos.x = startX;
    diamond->pos.y = startY;
    diamond->upDownMovement.basePosY = startY;
    diamond->upDownMovement.up = true;
    diamond->pos.width = 64.0f * 2.0f;
    diamond->pos.height = 64.0f * 2.0f;
    diamond->wasTouched = false;
    return diamond;
}

void dia_setPos(Diamond diamond, float posX, float posY) {
    diamond->pos.x = posX;
    diamond->pos.y = posY;
}

void dia_free(Diamond diamond) {
    anim_unloadSpritesheet(&diamond->sheet);
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

void dia_update(Diamond diamond) {
    if (!diamond->wasTouched) {
        moveUpAndDown(&diamond->pos.y, &diamond->upDownMovement);
    }
    anim_advanceAnimation(&diamond->animation);
}

void dia_draw(Diamond diamond) {
    anim_drawAnimation(&diamond->animation, &diamond->pos, &(Vector2){0.0f, 0.0f}, 0.0f);
}
