#include "include/checkPoint.h"
#include "include/raylib/raylib.h"
#include "include/slog.h"
#include <stdlib.h>

#define CHECKPOINT_WAITTIME 0.2f

typedef struct {
    int level;
    float posX;
    float posY;
} CheckpointData;

/**
 * Holds the currently active/reached checkpoint
 * This will be used to load the game
 * The next checkpoint will get the current checkpoint
 * with check_update
 */
typedef struct Checkpoint_internal {
    CheckpointData current;
    CheckpointData next;
    /// Time starts counting when player is under height of next checkpoint
    /// and height of player is same as height last frame
    float timer;
    int height;
} Checkpoint_internal;

Checkpoint check_createCheckpoint(void) {
    Checkpoint cp = malloc(sizeof(Checkpoint_internal));
    cp->timer = 0.0f;
    cp->height = 0;
    cp->current.level = 6;
    cp->next.level = 6;
    return cp;
}

int check_getCurrentLevel(Checkpoint cp) {
    return cp->current.level;
}

void check_setCurrentCheckpoint(Checkpoint cp, Rectangle *rect, int level) {
    cp->current.posX = rect->x;
    cp->current.posY = rect->y;
    cp->current.level = level;
    slogi("Current checkpoint manually set at level %d at pos(X:%f|Y:%f)",
          level, rect->x, rect->y);
}

void check_setNextCheckpoint(Checkpoint cp, Rectangle *rect, int level) {
    cp->next.posX = rect->x;
    cp->next.posY = rect->y;
    cp->next.level = level;
    slogi("Next checkpoint manually set at level %d at pos(X:%f|Y:%f)",
          level, rect->x, rect->y);
}

void check_free(Checkpoint cp) {
    free(cp);
}

float check_getX(Checkpoint cp) {
    return cp->current.posX;
}

float check_getY(Checkpoint cp) {
    return cp->current.posY;
}

void check_update(Checkpoint cp, float playerPosY) {
    // Return when next checkpoint was not set
    //(Levels can have no checkpoints)
    if (cp->current.level == cp->next.level)
        return;

    if (playerPosY > cp->next.posY) {
        cp->timer += GetFrameTime();
        if (cp->height != (int)playerPosY) {
            cp->height = (int)playerPosY;
            cp->timer = 0.0f;
        }
    }

    if (cp->timer > CHECKPOINT_WAITTIME) {
        cp->height = (int)playerPosY;
        cp->timer = 0.0f;
        cp->height = 0;
        cp->current.level = cp->next.level;
        cp->current.posX = cp->next.posX;
        cp->current.posY = cp->next.posY;
        slogi("Checkpoint automatically set at level %d at pos(X:%f|Y:%f)",
              cp->current.level, cp->current.posX, cp->current.posY);
    }
}
