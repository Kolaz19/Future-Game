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
    PLAYER = 99,
};

#endif
