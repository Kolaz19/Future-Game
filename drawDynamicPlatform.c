#include "include/drawDynamicPlatform.h"
#include "include/dynBodyDef.h"
#include <stdlib.h>

#define ID_INDX(x) (x - 1)

typedef struct TextureName {
    int id;
    char *name;
} TextureName;

static void loadTexture(PlatformTextureHandle handle, int id, char *fileName) {
    *(handle + ID_INDX(id)) = LoadTexture(fileName);
}

PlatformTextureHandle platTex_createPlatformTextureHandle(void) {

	//Numbers have to match name of object in Dynamic layer
    TextureName textureNames[] = {
        {CIRCLES_32X16, "assets/dynamicPlatforms/Plat1.png"},
        {BASIC_96X16, "assets/dynamicPlatforms/Plat2.png"},
        {CIRCLES_BROKEN_32X32, "assets/dynamicPlatforms/Plat3.png"},
        {THINNER_BROKEN_112X16, "assets/dynamicPlatforms/Plat4.png"},
        {THIN_NO_END_80X16, "assets/dynamicPlatforms/Plat5.png"},
        {THIN_END_112X16, "assets/dynamicPlatforms/Plat6.png"},
        {THIN_END_144X16, "assets/dynamicPlatforms/Plat7.png"}
    };
    int noElements = (sizeof(textureNames) / sizeof(TextureName));

    Texture *handle = malloc((unsigned long)noElements * sizeof(Texture));
    for (int i = 0; i < noElements; i++) {
        loadTexture(handle, textureNames[i].id, textureNames[i].name);
    }

    return handle;
}

void platTex_drawPlatform(PlatformTextureHandle handle, int id, Rectangle *dest, float rot) {
    Rectangle source = {0.0f, 0.0f, (float)handle[ID_INDX(id)].width, (float)handle[ID_INDX(id)].height};
    Rectangle destination = {dest->x + dest->width / 2, dest->y + dest->height / 2, dest->width, dest->height};
    DrawTexturePro(handle[ID_INDX(id)], source, destination, (Vector2){dest->width / 2.0f, dest->height / 2.0f}, rot, RAYWHITE);
}
