#ifndef DYN_PLAT_BEHAVIOR_H
#define DYN_PLAT_BEHAVIOR_H

#include "box2d/box2d.h"

typedef struct PersistentUpdateData {
	b2BodyId* body;
	int status;
	float timer;
}UpdateData;

#define UPDATE_STATUS_INIT 0

/*
 * Set update function based on ID
 * ID 0 always gets empty update function
 * ID 99 is player
 */
void setUpdateFunction(int id, void (**update)(UpdateData *updateData));

#endif
