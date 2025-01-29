#include "include/utest.h"
#include "include/physicsWorld.h"

UTEST_STATE();

UTEST(phy, objects) {
	WorldHandle world = phy_createWorld();
	phy_addPlatform(world, (Rectangle){30.0f, 30.0f, 50.0f, 10.0f});
	phy_addPlatform(world, (Rectangle){30.0f, 30.0f, 50.0f, 10.0f});
	phy_addPlatform(world, (Rectangle){30.0f, 30.0f, 50.0f, 10.0f});
	BodyRectReference bodyReferences[BAG_SIZE] = {NULL};
	int amountPlatforms = phy_getBodyReferences(world, bodyReferences, STATIC_PLATFORM);
	//Check amount objects produced
	ASSERT_EQ(amountPlatforms, 3);

	int amountPlatformsCheck = 0;
	for(int i = 0; i < BAG_SIZE; i++) {
		if(bodyReferences[i].rectangle != NULL) {
			amountPlatformsCheck++;
		}
	}
	//Check amount objects produced is reflected in bag
	ASSERT_EQ(amountPlatforms, amountPlatformsCheck);
}

UTEST(phy, character) {
	//Check if character creation and retrieval works
	WorldHandle world = phy_createWorld();
	BodyIdReference ref = phy_getCharacterBodyReference(world);
	ASSERT_TRUE(ref == NULL);
	phy_addPlayer(world);
	ASSERT_FALSE(ref != NULL);
}
