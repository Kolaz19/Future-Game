#ifndef DIAMOND_H
#define DIAMOND_H

typedef struct DiamondData* Diamond;
Diamond dia_createDiamond(float startX, float startY);
void dia_free(Diamond diamond);
void dia_update(Diamond diamond, float playerX, float playerY);
void dia_draw(Diamond diamond);
void dia_setPos(Diamond diamond, float posX, float posY);

#endif
