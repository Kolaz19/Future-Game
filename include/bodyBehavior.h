#ifndef DYN_PLAT_BEHAVIOR_H
#define DYN_PLAT_BEHAVIOR_H

#include "box2d/box2d.h"

/*
 * Two bodies with the same update function
 * can have different behavior based on
 * this modifier
 */
typedef enum DynBodyUpdateModifier {
    DEFAULT,
    LEFT,
    RIGHT,
	WAIT
}DynBodyUpdateModifier;

typedef struct PersistentUpdateData {
    b2BodyId *body;
    int status;
    float timer;
	int counter;
    DynBodyUpdateModifier modifier;
} UpdateData;

#define MAX_SHAPES_ATTACHED_TO_BODY 2

#define UPDATE_STATUS_INIT 0
#define UPDATE_STATUS_DEAD 9
#define UPDATE_STATUS_DISABLE 60

/*
 * Set update function for a dynamic body
 * Return update modifier for additional behavior
 * in update function
 */
enum DynBodyUpdateModifier setUpdateFunction(int id, void (**update)(UpdateData *updateData));

#endif
