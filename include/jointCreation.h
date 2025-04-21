#ifndef JOINT_CREATION_H
#define JOINT_CREATION_H
#include "box2d/box2d.h"

#define MAX_NUMBER_NEW_BODIES 2

/*
 * @original Already created body
 * @new All bodies created by the function
 * @width Width of original body
 * @height Height of original body
 */
typedef struct JointCreationContextData {
	b2BodyId *original;
	b2BodyId new[MAX_NUMBER_NEW_BODIES];
	float width;
	float height;
}JointCreationContext;

/*
 * Will set the creation function of new joints for a specific ID
 * Because joints are destroyed when body is destroyed,
 * we don't have to return them and don't have to keep track of them
 * @return If creation function should be executed
 */
bool setJointCreationFunction(int id, int (**create)(JointCreationContext*));

#endif
