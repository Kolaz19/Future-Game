#include "include/diamond.h"
#include "include/animation.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define FILE_NAME "assets/YellowDiamond.png"
#define START_X 140.f
#define START_Y 140.f

struct DiamondData{
	Rectangle pos;
	Spritesheet sheet;
	Animation animation;
};

Diamond dia_createDiamond(float startX, float startY) {
	Diamond diamond = malloc(sizeof(struct DiamondData));
	diamond->sheet = anim_loadSpritesheet(FILE_NAME, 4, 1);
	diamond->animation = anim_createAnimation(&diamond->sheet, 1, 4, 0.2f, LOOP);
	diamond->pos.x = startX;
	diamond->pos.y = startY;
	diamond->pos.width = 64.0f * 2.0f;
	diamond->pos.height = 64.0f * 2.0f;
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

void dia_update(Diamond diamond) {
	anim_advanceAnimation(&diamond->animation);
}

void dia_draw(Diamond diamond) {
	anim_drawAnimation(&diamond->animation, &diamond->pos, &(Vector2){0.0f, 0.0f}, 0.0f);
}
