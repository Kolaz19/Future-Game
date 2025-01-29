#ifndef ANIMATION_PLAYER_H
#define ANIMATION_PLAYER_H
typedef struct PlayerAnimations* PlAnimation;

PlAnimation panim_createAnimation(void);
void panim_update(PlAnimation plAnim, float velocityX, float velocityY);
void panim_draw(PlAnimation plAnim, int posX, int posY);
void panim_free(PlAnimation plAnim);

#endif
