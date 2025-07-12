#ifndef DIAMOND_H
#define DIAMOND_H

#include "raylib/rectangle.h"

typedef enum DiamondStatus {
    INIT,
    ABSORBING_POSITIONING,
    ABSORBING,
	LIFTOFF,
    FREE,
} DStatus;

typedef struct DiamondData* Diamond;
Diamond dia_createDiamond(float startX, float startY);
void dia_free(Diamond diamond);
DStatus dia_update(Diamond diamond, Rectangle *player);
void dia_draw(Diamond diamond);
void dia_setPos(Diamond diamond, float posX, float posY);
int dia_particlePercentageFinished(Diamond diamond);
int dia_particlePercentageActive(Diamond diamond);
float* dia_getDiamondYCoordinate(Diamond diamond);

#endif
