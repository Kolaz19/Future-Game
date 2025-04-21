#include "include/jointCreation.h"
#include "include/dynBodyDef.h"

#define CONV_VAL 20
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int noCreation(JointCreationContext*) { return 0; }
#pragma GCC diagnostic pop


int anchorOnRightSide(JointCreationContext* context) {
    b2WorldId world = b2Body_GetWorld(*context->original);
    b2Vec2 pos = b2Body_GetPosition(*context->original);

    b2BodyDef dynamicBodyDef2 = b2DefaultBodyDef();
    dynamicBodyDef2.position = (b2Vec2){pos.x + TOWORLD(context->width) / 2 - TOWORLD(16.0f), pos.y + TOWORLD(8.0f) };
    dynamicBodyDef2.type = b2_staticBody;
    context->new[0] = b2CreateBody(world, &dynamicBodyDef2);
    b2Polygon dynamicBox2 = b2MakeBox(TOWORLD(8.0f), TOWORLD(8.0f));
    b2ShapeDef dynamicShapeDef2 = b2DefaultShapeDef();
    b2CreatePolygonShape(context->new[0], &dynamicShapeDef2, &dynamicBox2);

    b2RevoluteJointDef jointDef2 = b2DefaultRevoluteJointDef();
    jointDef2.bodyIdA = *context->original;
    jointDef2.bodyIdB = context->new[0];
    jointDef2.localAnchorA = (b2Vec2){0.0f, 0.0f}; // b2Body_GetLocalPoint(dynamicId2, worldPoint);
    jointDef2.localAnchorB = (b2Vec2){0.0f, 5.0f}; // b2Body_GetLocalPoint(dynamicId, worldPoint);
    b2CreateRevoluteJoint(world, &jointDef2);
	return 1;
}

bool setJointCreationFunction(int id, int (**create)(JointCreationContext*)) {
    *create = &noCreation;
    switch (id) {
    case UNDEFINED:
        *create = &noCreation;
        break;
		/*
    case THIN_END_144X16:
		*create = &anchorOnRightSide;
		return true;
		break;
		*/
	}
	return false;
}
