#ifndef DRAW_DYN_PLAT_H
#define DRAW_DYN_PLAT_H
#include "raylib/raylib.h"

typedef struct Texture *PlatformTextureHandle;
PlatformTextureHandle platTex_createPlatformTextureHandle(void);
void platTex_drawPlatform(PlatformTextureHandle handle, int id, Rectangle *dest, float rot);
void platTex_free(PlatformTextureHandle handle);

#endif
