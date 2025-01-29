#ifndef RAYLIB_ANIMATION
#define RAYLIB_ANIMATION

#include "raylib/raylib.h"

typedef struct Spritesheet Spritesheet;
typedef struct Animation Animation;

struct Spritesheet {
    Texture2D texture;
    int amountFramesX;
    int amountFramesY;
};

typedef enum AnimationType {
	/// Reset to first frame after last frame was finished.
	/// Animation is enabled by default
    LOOP,
	/// Stop at last frame.
	/// Is disabled by default
    PLAY_ONCE
} AnimationType;

typedef enum FlipAxis {
    FLIPX,
    FLIPY,
} FlipAxis;

struct Animation {
    Spritesheet *spriteSheet;
    int startFrame;
    int endFrame;
    int currentFrame;
    float flipShiftX;
    float flipShiftY;
    bool flipX;
    bool flipY;
    FlipAxis flipMode;
    Rectangle origin;
    float frameDuration;
    float advancedTime;
    AnimationType type;
    bool isPlaying;
};

Spritesheet anim_loadSpritesheet(const char *fileName, int amountFramesX,
                                 int amountFramesY);
void anim_unloadSpritesheet(Spritesheet *spriteSheet);

/*
 * From left to right, row to row, set start and end frame of the animation
 * Set frame duration in seconds
 * @param startFrame From left to right on sprite sheet
 * @param endFrame From top to bottom
 */
Animation anim_createAnimation(Spritesheet *spriteSheet, int startFrame,
                               int endFrame, float frameDuration,
                               AnimationType type);
void anim_drawAnimation(Animation *animation, Rectangle *destination,
                        Vector2 *origin, float rotation);

/// Update animation (based on frameDuration)
void anim_advanceAnimation(Animation *animation);
/// Reset advanced time, reset to startFrame and enable animation
void anim_startAnimation(Animation *animation);
/// Reset advanced time, reset to startFrame and disable animation
void anim_stopAnimation(Animation *animation);
/// Enable function of advanceAnimation and drawAnimation
void anim_enableAnimation(Animation *animation);
/// Disable function of advanceAnimation and drawAnimation
void anim_disableAnimation(Animation *animation);
int anim_getCurrentFrame(Animation *animation);
/// Return true if animation was switched to start of frame by advanceAnimation
bool anim_startOfAnimation(Animation *animation);
/// Mandatory for flipped frames when texture is not centered on frame
void anim_setShiftValues(Animation *animation, float shiftValueX, float shiftValueY);
/// Toggle flipped drawing in drawAnimation
void anim_flip(Animation *animation, FlipAxis axis);
/// Toggle un-flipped drawing in drawAnimation
void anim_flipReset(Animation *animation, FlipAxis axis);
#endif
