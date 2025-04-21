#ifndef DYN_BODY_DEF_H
#define DYN_BODY_DEF_H

/*
 * Numbers in dynamic body layer of map
 * UNDEFINED and PLAYER are exceptions
 * and are not defined in tilemap
 */
enum DynbBodyDef {
    UNDEFINED = 0,
    CIRCLES_32X16 = 1,
	BASIC_96X16 = 2,
	CIRCLES_BROKEN_32X32 = 3,
	THINNER_BROKEN_112X16 = 4,
	THIN_NO_END_80X16 = 5,
	THIN_END_112X16 = 6,
	THIN_END_144X16 = 7,
	JOINT_ONLY_RIGHT_208X16 = 8,
    PLAYER = 99,
};


#endif
