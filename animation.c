#include "include/animation.h"

static void setOrigin(Rectangle *origin, int *currentFrame,
                      Spritesheet *spriteSheet);

Spritesheet anim_loadSpritesheet(const char *fileName, int amountFramesX,
                                 int amountFramesY) {
    Spritesheet spriteSheet;

    Texture2D texture = LoadTexture(fileName);
    spriteSheet.texture = texture;

    spriteSheet.amountFramesX = amountFramesX;
    spriteSheet.amountFramesY = amountFramesY;

    return spriteSheet;
}

void anim_unloadSpritesheet(Spritesheet *spriteSheet) {
    UnloadTexture((spriteSheet->texture));
}

void anim_setShiftValues(Animation *animation, float shiftValueX,
                         float shiftValueY) {
    animation->flipShiftX = shiftValueX;
    animation->flipShiftY = shiftValueY;
}

Animation anim_createAnimation(Spritesheet *spriteSheet, int startFrame,
                               int endFrame, float frameDuration,
                               AnimationType type) {
    Animation animation;
    animation.spriteSheet = spriteSheet;
    animation.startFrame = startFrame;
    animation.endFrame = endFrame;
    animation.currentFrame = startFrame;
    animation.frameDuration = frameDuration;
    animation.advancedTime = 0;
    animation.type = type;
    animation.isPlaying = type == LOOP ? true : false;

    animation.flipShiftX = 0;
    animation.flipShiftY = 0;
    animation.flipX = false;
    animation.flipY = false;
    setOrigin(&animation.origin, &animation.currentFrame, animation.spriteSheet);

    return animation;
}

void anim_startAnimation(Animation *animation) {
    animation->isPlaying = true;
    animation->currentFrame = animation->startFrame;
    animation->advancedTime = 0;
    setOrigin(&animation->origin, &animation->currentFrame,
              animation->spriteSheet);
}

void anim_stopAnimation(Animation *animation) {
    animation->isPlaying = false;
    animation->currentFrame = animation->startFrame;
    animation->advancedTime = 0;
    setOrigin(&animation->origin, &animation->currentFrame,
              animation->spriteSheet);
}

void anim_enableAnimation(Animation *animation) {
    animation->isPlaying = true;
}

void anim_disableAnimation(Animation *animation) {
    animation->isPlaying = false;
}

int anim_getCurrentFrame(Animation *animation) {
    return animation->currentFrame - animation->startFrame + 1;
}

/*
 * Return true if last call of advanceAnimation moved
 * origin to next frame
 */
bool anim_startOfAnimation(Animation *animation) {
    return animation->advancedTime == 0;
}

void anim_flip(Animation *animation, FlipAxis axis) {
    if (axis == FLIPX) {
        animation->flipX = true;
    } else {
        animation->flipY = true;
    }
}

void anim_flipReset(Animation *animation, FlipAxis axis) {
    if (axis == FLIPX) {
        animation->flipX = false;
    } else {
        animation->flipY = false;
    }
}

/*
 * Set a new origin rectangle after a specific amount of time.
 */
void anim_advanceAnimation(Animation *animation) {
    if (!animation->isPlaying) {
        return;
    }

    animation->advancedTime += GetFrameTime();
    if (animation->advancedTime >= animation->frameDuration) {
        // After switching to a new frame, leftover advanced time is cut off
        animation->advancedTime = 0;
        animation->currentFrame++;
    }

    // Loop back to start frame
    if (animation->currentFrame > animation->endFrame) {
        animation->currentFrame = animation->startFrame;
        if (animation->type == PLAY_ONCE) {
            animation->isPlaying = false;
        }
    }
    // Set new origin when current frame has changed
    if (animation->advancedTime == 0) {
        setOrigin(&animation->origin, &animation->currentFrame,
                  animation->spriteSheet);
    }
}

/*
 * Draw current frame at destination.
 * Calculate flipped frame dynamically in DrawTexturePro parameters.
 * In this case, destination frame has to be shifted when the texture is not
 * centered.
 */
void anim_drawAnimation(Animation *animation, Rectangle *destination,
                        Vector2 *origin, float rotation) {
    if (!animation->isPlaying) {
        return;
    }

    DrawTexturePro(
        animation->spriteSheet->texture,
        (Rectangle){animation->origin.x, animation->origin.y,
                    animation->flipX == true ? animation->origin.width * -1
                                             : animation->origin.width,
                    animation->flipY == true ? animation->origin.height * -1
                                             : animation->origin.height},
        (Rectangle){
            animation->flipX == true ? destination->x + animation->flipShiftX
                                     : destination->x,
            animation->flipY == true ? destination->y + animation->flipShiftY
                                     : destination->y,
            destination->width, destination->height},
        *origin, rotation, WHITE);
}

/*
 * Calculate the origin rectangle based on the width/height and the
 * amount of frames on the x-asix on the original texture.
 */
static void setOrigin(Rectangle *origin, int *currentFrame,
                      Spritesheet *spriteSheet) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
   origin->width =
        (int)(spriteSheet->texture.width / spriteSheet->amountFramesX);
    origin->height =
        (int)(spriteSheet->texture.height / spriteSheet->amountFramesY);
    origin->y =
        (int)((*currentFrame - 1) / spriteSheet->amountFramesX) * origin->height;
    origin->x = (int)(*currentFrame - 1 - ((*currentFrame / spriteSheet->amountFramesX)
					  * spriteSheet->amountFramesX)) * origin->width;
#pragma GCC diagnostic pop
}
