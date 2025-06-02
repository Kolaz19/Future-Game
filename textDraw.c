#include "include/textDraw.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define AMOUNT_TEXTS 9
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
        pos->x = 0.05f;
        pos->y = 0.05f;
        break;
    }
}

TextHandle text_init() {
    TextHandle text = malloc(sizeof(TextDraw));
    text->font = LoadFont("assets/pixantiqua.png");		//2
    text->timer = TIME_COMBINED + 1.0f;
    text->currentLevel = 1;

    text->color.r = DARKPURPLE.r;
    text->color.g = DARKPURPLE.g;
    text->color.b = DARKPURPLE.b;
    text->color.a = 0;

    text->texts[LVL_INDX(1)].text = "For three days,\nI wander these depths";

    for (int i = 0; i < AMOUNT_TEXTS; i++) {
        setPos(&(text->texts[i].pos), i + 1);
    }

    return text;
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
