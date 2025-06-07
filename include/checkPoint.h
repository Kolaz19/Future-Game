#ifndef CHECKPOINT_H
#define CHECKPOINT_H

#include <stdbool.h>
#include "raylib/rectangle.h"

typedef struct Checkpoint_internal *Checkpoint;

Checkpoint check_createCheckpoint(void);
int check_getCurrentLevel(Checkpoint cp);
void check_setNextCheckpoint(Checkpoint cp, Rectangle *rect, int level);
void check_free(Checkpoint cp);

/**
 * Overwrite current checkpoint with next checkpoint
 * when player is under next checkpoint and stays at same
 * height for a specific amount of time
 * @return If checkpoint was updated
 */
bool check_update(Checkpoint cp, float playerPosY);
void check_setCurrentCheckpoint(Checkpoint cp,Rectangle *rect, int level);
float check_getX(Checkpoint cp);
float check_getY(Checkpoint cp);

#endif
