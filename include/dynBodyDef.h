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
	BIG_UPPER_BLOCK_64X112 = 9,
	SLIDER_128X16 = 10,
	ANKERED_160x16 = 11,
	ANKERED_144x16 = 12,
	STANDING_CAGE_16x176 = 13,
	ENERGY_BOXC_20x20 = 14,
    PLAYER = 99,
};


#endif
