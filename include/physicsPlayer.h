#ifndef PHY_CHAR_H
#define PHY_CHAR_H

#include "bodyIdReference.h"

/*
 * Catch button pressed and apply force to player
 */
void plphy_update(BodyIdReference body, float *jumpCooldown);

/*
 * Get velocity of player body
 * @param velX Velocity of body on x axis
 * @param velY Velocity of body on y axis
 */
void plphy_getVelocity(BodyIdReference body,float *velX, float *velY);

#endif
