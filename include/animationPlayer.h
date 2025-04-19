#ifndef ANIMATION_PLAYER_H
#define ANIMATION_PLAYER_H

#include <stdbool.h>
typedef struct PlayerAnimations* PlAnimation;

PlAnimation panim_createAnimation(void);

/*
 * Advance Animation and switch animation based on
 * force applied to body
 * @param velocityX Velocity X applied to player body before
 * @param velocityY Velocity Y applied to player body before
 */
void panim_update(PlAnimation plAnim, float velocityX, float velocityY, bool hasGroundContact);
void panim_draw(PlAnimation plAnim, float posX, float posY);
void panim_free(PlAnimation plAnim);
void panim_setDying(PlAnimation plAnim);
void panim_setAlive(PlAnimation plAnim);

#endif
