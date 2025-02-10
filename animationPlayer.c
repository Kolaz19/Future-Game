#include "include/animationPlayer.h"
#include "include/animation.h"
#include "include/slog.h"
#include <stdlib.h>

#define CHAR_SPRITESHEET "assets/player.png"
#define NEG_LIMIT -0.5f
#define POS_LIMIT 0.5f

typedef struct PlayerAnimations {
    Spritesheet sheet;
    Animation *curAnimation;
    Animation idle;
    Animation running;
    Animation falling;
    Animation dying;
    bool flip;
} PlayerAnimations;

PlAnimation panim_createAnimation(void) {
    PlAnimation plAnim = malloc(sizeof(PlayerAnimations));
    plAnim->sheet = anim_loadSpritesheet(CHAR_SPRITESHEET, 6, 5);
    plAnim->idle = anim_createAnimation(&(plAnim->sheet), 1, 4, 0.5f, LOOP);
    plAnim->running = anim_createAnimation(&(plAnim->sheet), 7, 10, 0.1f, LOOP);
    plAnim->falling = anim_createAnimation(&(plAnim->sheet), 13, 14, 0.1f, LOOP);
    plAnim->dying = anim_createAnimation(&(plAnim->sheet), 19, 24, 0.5f, LOOP);
    plAnim->curAnimation = &plAnim->idle;
    plAnim->flip = false;
    return plAnim;
}

void panim_update(PlAnimation plAnim, float velocityX, float velocityY) {
    Animation *prevAnimation = plAnim->curAnimation;
    bool prevFlip = plAnim->flip;
    // Set correct state
    if (velocityX > NEG_LIMIT && velocityX < POS_LIMIT && velocityY > NEG_LIMIT && velocityY < POS_LIMIT) {
        plAnim->curAnimation = &plAnim->idle;
        if (plAnim->curAnimation != prevAnimation) {
			slogd("Animation switched to IDLE");
        }
    } else if (velocityY > POS_LIMIT) {
        plAnim->curAnimation = &plAnim->falling;
        if (plAnim->curAnimation != prevAnimation) {
			slogd("Animation switched to FALLING");
        }
    } else if (velocityX != 0.0f) {
        plAnim->curAnimation = &plAnim->running;
        if (plAnim->curAnimation != prevAnimation) {
			slogd("Animation switched to RUNNING");
        }
    }

    // Set global flip state
    if (velocityX > POS_LIMIT) {
        plAnim->flip = false;
    } else if (velocityX < NEG_LIMIT) {
        plAnim->flip = true;
    }

	if (prevFlip != plAnim->flip) {
		slogd("Animation flipped to %d", plAnim->flip);
	}

    // Flip current animation
    if (plAnim->flip) {
        anim_flip(plAnim->curAnimation, FLIPX);
    } else {
        anim_flipReset(plAnim->curAnimation, FLIPX);
    }
    anim_advanceAnimation(plAnim->curAnimation);
}

void panim_draw(PlAnimation plAnim, int posX, int posY) {
    anim_drawAnimation(plAnim->curAnimation, &(Rectangle){(float)posX, (float)posY, 32.0f, 32.0f}, &(Vector2){8.0f, 0.0f}, 0.0f);
}

void panim_free(PlAnimation plAnim) {
    anim_unloadSpritesheet(&(plAnim->sheet));
    free(plAnim);
}
