#include "include/jointCreation.h"
#include "include/dynBodyDef.h"

#define CONV_VAL 20
#define SPRITE_SIZE 16.0f
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int noCreation(JointCreationContext *context) { return 0; }
#pragma GCC diagnostic pop

int anchorOnLeftSide(JointCreationContext *context) {
    b2WorldId world = b2Body_GetWorld(*context->original);
    b2Vec2 pos = b2Body_GetPosition(*context->original);

    b2BodyDef dynamicBodyDef = b2DefaultBodyDef();
    // At first sprite of body
    dynamicBodyDef.position = (b2Vec2){pos.x - TOWORLD(context->width / 2 - SPRITE_SIZE / 2), pos.y};
    dynamicBodyDef.type = b2_staticBody;
    context->new[0] = b2CreateBody(world, &dynamicBodyDef);
    b2Polygon dynamicBox = b2MakeBox(TOWORLD(SPRITE_SIZE / 2.0f), TOWORLD(SPRITE_SIZE / 2.0f));
    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.isSensor = true;
    b2CreatePolygonShape(context->new[0], &dynamicShapeDef, &dynamicBox);

    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    jointDef.bodyIdA = *context->original;
    jointDef.bodyIdB = context->new[0];
    jointDef.localAnchorA = (b2Vec2){-1.0f * TOWORLD(context->width / 2.0f - SPRITE_SIZE / 2), 0.0f};
    jointDef.localAnchorB = (b2Vec2){0.0f, 0.0f};
    b2CreateRevoluteJoint(world, &jointDef);
    return 1;
}

int anchorOnRightSide(JointCreationContext *context) {
    b2WorldId world = b2Body_GetWorld(*context->original);
    b2Vec2 pos = b2Body_GetPosition(*context->original);

    b2BodyDef dynamicBodyDef = b2DefaultBodyDef();
    // At last sprite of body
    dynamicBodyDef.position = (b2Vec2){pos.x + TOWORLD(context->width / 2 - SPRITE_SIZE / 2), pos.y};
    dynamicBodyDef.type = b2_staticBody;
    context->new[0] = b2CreateBody(world, &dynamicBodyDef);
    b2Polygon dynamicBox = b2MakeBox(TOWORLD(SPRITE_SIZE / 2.0f), TOWORLD(SPRITE_SIZE / 2.0f));
    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.isSensor = true;
    b2CreatePolygonShape(context->new[0], &dynamicShapeDef, &dynamicBox);

    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    jointDef.bodyIdA = *context->original;
    jointDef.bodyIdB = context->new[0];
    jointDef.localAnchorA = (b2Vec2){1.0f * TOWORLD(context->width / 2.0f - SPRITE_SIZE / 2), 0.0f};
    jointDef.localAnchorB = (b2Vec2){0.0f, 0.0f};
    b2CreateRevoluteJoint(world, &jointDef);
    return 1;
}

int anchorLeftDown(JointCreationContext *context) {
    b2WorldId world = b2Body_GetWorld(*context->original);
    b2Vec2 pos = b2Body_GetPosition(*context->original);

    b2BodyDef dynamicBodyDef = b2DefaultBodyDef();
    // At down left sprite of body
    dynamicBodyDef.position = (b2Vec2){pos.x - TOWORLD(context->width / 2 - SPRITE_SIZE / 2),
                                       pos.y + TOWORLD(context->height / 2 - SPRITE_SIZE / 2)};
    dynamicBodyDef.type = b2_staticBody;
    context->new[0] = b2CreateBody(world, &dynamicBodyDef);
    b2Polygon dynamicBox = b2MakeBox(TOWORLD(SPRITE_SIZE / 2.0f), TOWORLD(SPRITE_SIZE / 2.0f));
    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.isSensor = true;
    b2CreatePolygonShape(context->new[0], &dynamicShapeDef, &dynamicBox);

    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    jointDef.bodyIdA = *context->original;
    jointDef.bodyIdB = context->new[0];
    jointDef.localAnchorA = (b2Vec2){-1.0f * TOWORLD(context->width / 2.0f - SPRITE_SIZE / 2),
                                     1.0f * TOWORLD(context->height / 2.0f - SPRITE_SIZE / 2)};
    jointDef.localAnchorB = (b2Vec2){0.0f, 0.0f};
    b2CreateRevoluteJoint(world, &jointDef);
    return 1;
}

int anchorPlatformLeft(JointCreationContext *context) {
    b2WorldId world = b2Body_GetWorld(*context->original);
    b2Vec2 pos = b2Body_GetPosition(*context->original);

    b2BodyDef dynamicBodyDef = b2DefaultBodyDef();
    // At second box from left
    dynamicBodyDef.position = (b2Vec2){pos.x - TOWORLD(context->width / 2 - SPRITE_SIZE / 2 - SPRITE_SIZE),
                                       pos.y + TOWORLD(SPRITE_SIZE / 2)};
    dynamicBodyDef.type = b2_staticBody;
    context->new[0] = b2CreateBody(world, &dynamicBodyDef);
    b2Polygon dynamicBox = b2MakeBox(TOWORLD(SPRITE_SIZE / 2.0f), TOWORLD(SPRITE_SIZE / 2.0f));
    b2ShapeDef dynamicShapeDef = b2DefaultShapeDef();
    dynamicShapeDef.isSensor = true;
    b2CreatePolygonShape(context->new[0], &dynamicShapeDef, &dynamicBox);

    b2RevoluteJointDef jointDef = b2DefaultRevoluteJointDef();
    jointDef.bodyIdA = *context->original;
    jointDef.bodyIdB = context->new[0];
    jointDef.localAnchorA = (b2Vec2){-1.0f * TOWORLD(context->width / 2.0f - SPRITE_SIZE / 2 - SPRITE_SIZE),
                                     1.0f * TOWORLD(SPRITE_SIZE / 2)};
    jointDef.localAnchorB = (b2Vec2){0.0f, 0.0f};
    b2CreateRevoluteJoint(world, &jointDef);
    return 1;
}

bool setJointCreationFunction(int id, int (**create)(JointCreationContext *)) {
    *create = &noCreation;
    switch (id) {
    case UNDEFINED:
        *create = &noCreation;
        break;
	case LONG_ONE_SIDED_208x16:
		*create = &anchorOnRightSide;
		return true;
		break;
    case JOINT_ONLY_RIGHT_208X16:
        *create = &anchorOnLeftSide;
        return true;
        break;
    case BIG_UPPER_BLOCK_64X112:
        *create = &anchorLeftDown;
        return true;
        break;
	case LONG_ONE_SIDED_240x16:
		*create = anchorPlatformLeft;
		return true;
		break;
    }
    return false;
}
