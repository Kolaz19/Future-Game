#ifndef DYN_PLAT_BEHAVIOR_H
#define DYN_PLAT_BEHAVIOR_H

#include "box2d/box2d.h"

/*
 * Set update function based on ID
 * ID 0 always gets empty update function
 */
void setUpdateFunction(int id, void (**update)(b2BodyId*, float*));

#endif
