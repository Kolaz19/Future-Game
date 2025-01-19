#include "include/box2dWrapper.h"
#include <assert.h>

#include "include/box2d/box2d.h"
#include "include/box2d/collision.h"
#include "include/box2d/math_functions.h"
#include "include/box2d/types.h"


#define CONV_VAL 20
#define TOWORLD(x) ((x) / CONV_VAL)
#define TOPIXEL(x) ((x) * CONV_VAL)

static void addToList(ObjectBag *bag, b2BodyId *entry) {
    assert(bag->currentEmptyIndex < BAG_SIZE);
    bag->bodies[bag->currentEmptyIndex].revision = entry->revision;
    bag->bodies[bag->currentEmptyIndex].world0 = entry->world0;
    bag->bodies[bag->currentEmptyIndex].index1 = entry->index1;
    bag->currentEmptyIndex++;
}

void phy_initBag(ObjectBag *bag, BodyType type) {
	bag->currentEmptyIndex = 0;
	bag->type = type;
}

b2WorldId phy_createWorld(void) {
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 50.0f};

    b2WorldId worldId = b2CreateWorld(&worldDef);
    return worldId;
}

void phy_addPlatform(b2WorldId *world, ObjectBag *bag, Platform plat) {
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){TOWORLD(plat.x + plat.width / 2), TOWORLD(plat.y + plat.height / 2)};

    b2BodyId groundId = b2CreateBody(*world, &groundBodyDef);
    b2Polygon groundBox = b2MakeBox(TOWORLD(plat.width / 2), TOWORLD(plat.height / 2));

    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    addToList(bag, &groundId);
}
