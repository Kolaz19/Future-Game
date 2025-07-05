#ifndef DIAMOND_H
#define DIAMOND_H

#include "raylib/rectangle.h"

typedef enum DiamondStatus {
    INIT,
    ABSORBING,
    FREE,
} DStatus;

typedef struct DiamondData* Diamond;
Diamond dia_createDiamond(float startX, float startY);
void dia_free(Diamond diamond);
DStatus dia_update(Diamond diamond, Rectangle *player);
void dia_draw(Diamond diamond);
void dia_setPos(Diamond diamond, float posX, float posY);

#endif
