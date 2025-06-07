#include "include/textDraw.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define AMOUNT_TEXTS 6
#define MAX_R 255

#define TIME_FADE_IN_OPACITY 3.0f
#define TIME_FULL_OPACITY 3.0f
#define TIME_FADE_OUT_OPACITY 3.0f
#define TIME_COMBINED (TIME_FADE_IN_OPACITY + TIME_FULL_OPACITY + TIME_FADE_OUT_OPACITY)

#define BASE_WIDTH 1920.0f

#define LVL_INDX(x) ((x)-1)
#define FONT_SIZE ((float)GetScreenWidth() * 5.0f / BASE_WIDTH)

typedef struct TextEntity {
    char *text;
    /// Multiplier for screen position
    Vector2 pos;
} TextEntity;

typedef struct TextDraw {
    Font font;
    Color color;
    float timer;
    int currentLevel;
    TextEntity texts[AMOUNT_TEXTS];
} TextDraw;

static void setPos(Vector2 *pos, int lvl) {
    switch (lvl) {
    case 1:
        pos->x = 0.4f;
        pos->y = 0.05f;
        break;
    case 2:
        pos->x = 0.15f;
        pos->y = 0.65f;
        break;
    case 3:
        pos->x = 0.45f;
        pos->y = 0.80f;
        break;
    case 4:
        pos->x = 0.39f;
        pos->y = 0.53f;
        break;
    case 6:
        pos->x = 0.10f;
        pos->y = 0.70f;
        break;
    }
}

TextHandle text_init() {
    TextHandle text = malloc(sizeof(TextDraw));
    text->font = LoadFont("assets/pixantiqua.png");
    text->timer = TIME_COMBINED + 1.0f;
    text->currentLevel = 1;

    //text->color.r = DARKPURPLE.r;
    //text->color.g = DARKPURPLE.g;
    //text->color.b = DARKPURPLE.b;
    text->color.r = VIOLET.r;
    text->color.g = VIOLET.g;
    text->color.b = VIOLET.b;
    text->color.a = 0;

    text->texts[LVL_INDX(1)].text = "For three days now,\nI have been wandering these depths";
    text->texts[LVL_INDX(2)].text = "Metal and rust as my only friends\nThe smell makes me dizzy";
    text->texts[LVL_INDX(3)].text = "So much trust in that little figure";
    text->texts[LVL_INDX(4)].text = "Deep in the core, a glimmer of hope\nSadly not for me";
    text->texts[LVL_INDX(5)].text = "";
    text->texts[LVL_INDX(6)].text = "I wonder what they are doing on the surface right now";

    for (int i = 0; i < AMOUNT_TEXTS; i++) {
        setPos(&(text->texts[i].pos), i + 1);
    }

    return text;
}

void text_free(TextHandle handle) {
	UnloadFont(handle->font);
	free(handle);
}

bool text_active(TextHandle handle) {
    return handle->timer <= TIME_COMBINED;
}

void text_activateLevelText(TextHandle handle, int level) {
    handle->currentLevel = level;
    handle->timer = 0.0f;
    handle->color.a = 0;
}

/*
 * Build up opacity until TIME_FADE_IN_OPACITY
 * Stay at full opacity until TIME_FULL_OPACITY
 * Loose opacity until TIME_FADE_OUT_OPACITY
 */
static unsigned char getOpacity(float timer) {
    unsigned int opa;
    if (timer < TIME_FADE_IN_OPACITY) {
        opa = (unsigned int)(timer * MAX_R / TIME_FADE_IN_OPACITY);
    } else if (timer > TIME_FADE_IN_OPACITY + TIME_FULL_OPACITY) {
		opa = (unsigned int)((TIME_COMBINED - timer) * MAX_R / TIME_FADE_OUT_OPACITY);
    } else {
        opa = MAX_R;
    }

    return opa > MAX_R ? MAX_R : (unsigned char)opa;
}

void text_update(TextHandle handle) {
    if (handle->timer > TIME_COMBINED)
        return;

    handle->timer += GetFrameTime();

    handle->color.a = getOpacity(handle->timer);
}

void text_draw(TextHandle handle) {
    if (handle->timer > TIME_COMBINED)
        return;

    Vector2 pos = {.x = (float)GetScreenWidth() * handle->texts[LVL_INDX(handle->currentLevel)].pos.x,
                   .y = (float)GetScreenHeight() * handle->texts[LVL_INDX(handle->currentLevel)].pos.y};

    DrawTextEx(handle->font,
               handle->texts[LVL_INDX(handle->currentLevel)].text,
               pos,
               (float)handle->font.baseSize * FONT_SIZE,
               2,
               handle->color);
}
