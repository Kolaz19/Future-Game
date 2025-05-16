#include "include/animationPlayer.h"
#include "include/animation.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"
#include <stdlib.h>

#define CHAR_SPRITESHEET "assets/player.png"
#define NEG_LIMIT_RUN -0.1f
#define POS_LIMIT_RUN 0.1f
#define NEG_LIMIT_JUMP -0.00000001f
#define POS_LIMIT_JUMP 0.00000001f
#define LIMIT_FALLING 10.0f

typedef struct PlayerAnimations {
    Spritesheet sheet;
    Animation *curAnimation;
    Animation idle;
    Animation running;
    Animation falling;
    Animation jumping;
    Animation dying;
    bool flip;
} PlayerAnimations;

PlAnimation panim_createAnimation(void) {
    PlAnimation plAnim = malloc(sizeof(PlayerAnimations));
    plAnim->sheet = anim_loadSpritesheet(CHAR_SPRITESHEET, 6, 6);
    plAnim->idle = anim_createAnimation(&(plAnim->sheet), 1, 4, 0.5f, LOOP);
    plAnim->running = anim_createAnimation(&(plAnim->sheet), 7, 10, 0.1f, LOOP);
    plAnim->falling = anim_createAnimation(&(plAnim->sheet), 13, 14, 0.1f, LOOP);
    plAnim->jumping = anim_createAnimation(&(plAnim->sheet), 19, 20, 0.15f, LOOP);
    plAnim->dying = anim_createAnimation(&(plAnim->sheet), 25, 30, 0.1f, PLAY_ONCE);
    anim_startAnimation(&plAnim->dying);
    plAnim->curAnimation = &plAnim->idle;
    plAnim->flip = false;
    return plAnim;
}

void panim_update(PlAnimation plAnim, float velocityX, float velocityY, bool hasGroundContact) {
    Animation *prevAnimation = plAnim->curAnimation;
    bool prevFlip = plAnim->flip;

    // Check death state set in setDying()
    if (plAnim->curAnimation == &plAnim->dying) {
        if (!(anim_getCurrentFrame(&plAnim->dying) == 6)) {
            anim_advanceAnimation(plAnim->curAnimation);
        }
        return;
    }

    if (velocityY > LIMIT_FALLING) {
        plAnim->curAnimation = &plAnim->falling;
        if (plAnim->curAnimation != prevAnimation) {
            slogd("Animation switched to FALLING");
        }
    } else if (hasGroundContact) {
        if //((IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) &&
            (velocityX > POS_LIMIT_RUN || velocityX < NEG_LIMIT_RUN) {
            plAnim->curAnimation = &plAnim->running;
            if (plAnim->curAnimation != prevAnimation) {
                slogd("Animation switched to RUNNING");
            }
        } else {
            plAnim->curAnimation = &plAnim->idle;
            if (plAnim->curAnimation != prevAnimation) {
                slogd("Animation switched to IDLE");
            }
        }
    } else {
        plAnim->curAnimation = &plAnim->jumping;
        if (plAnim->curAnimation != prevAnimation) {
            slogd("Animation switched to JUMPING");
        }
    }

    // Set global flip state
    if (velocityX > POS_LIMIT_RUN) {
        plAnim->flip = false;
    } else if (velocityX < NEG_LIMIT_RUN) {
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

void panim_setDying(PlAnimation plAnim) {
    if (plAnim->curAnimation == &plAnim->dying)
        return;
    bool prevAnimationFlip = plAnim->curAnimation->flipX;
    plAnim->curAnimation = &plAnim->dying;
    anim_startAnimation(plAnim->curAnimation);
    plAnim->curAnimation->flipX = prevAnimationFlip;
    slogd("Animation switched to DYING");
}

void panim_setAlive(PlAnimation plAnim) {
    plAnim->curAnimation = &plAnim->idle;
    slogd("Animation switched to ALIVE/IDLE");
}

void panim_draw(PlAnimation plAnim, float posX, float posY) {
    anim_drawAnimation(plAnim->curAnimation, &(Rectangle){posX, posY, 32.0f, 32.0f}, &(Vector2){8.0f, 0.0f}, 0.0f);
}

void panim_free(PlAnimation plAnim) {
    anim_unloadSpritesheet(&(plAnim->sheet));
    free(plAnim);
}
