#include "include/textDraw.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define MAX_R 255
#define TIME_FULL_OPACITY 3.0f
#define TIME_END 6.0f

#define LVL_INDX(x) ((x)-1)

typedef struct TextDraw {
    Font font;
    Color color;
    float timer;
    bool userInputRegistered;
    int currentLevel;
    char *texts[9];
} TextDraw;

TextHandle text_init() {
    TextHandle text = malloc(sizeof(TextDraw));
    text->font = LoadFont("assets/pixelplay.png");
    text->timer = TIME_END + 1.0f;
    text->userInputRegistered = false;
    text->currentLevel = 1;

    text->color.r = DARKPURPLE.r;
    text->color.g = DARKPURPLE.g;
    text->color.b = DARKPURPLE.b;
    text->color.a = 0;

    text->texts[LVL_INDX(1)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(2)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(3)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(4)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(5)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(6)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(7)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(8)] = "For three days,\nI wander these depths";
    text->texts[LVL_INDX(9)] = "For three days,\nI wander these depths";

    return text;
}

bool text_active(TextHandle handle) {
    return handle->timer <= TIME_END;
}

void text_activateLevelText(TextHandle handle, int level) {
    handle->currentLevel = level;
    handle->timer = 0.0f;
    handle->color.a = 0;
    handle->userInputRegistered = false;
}

static unsigned char getOpacity(float timer) {
    unsigned int opa;
    if (timer < TIME_FULL_OPACITY) {
        opa = (unsigned int)(timer * MAX_R / TIME_FULL_OPACITY);
    } else {
        opa = (unsigned int)(((TIME_END - TIME_FULL_OPACITY) - (timer - TIME_FULL_OPACITY)) * MAX_R / (TIME_END - TIME_FULL_OPACITY));
    }

    return opa > MAX_R ? MAX_R : (unsigned char)opa;
}

void text_update(TextHandle handle) {
    if (handle->timer > TIME_END)
        return;
    if ((handle->timer > TIME_FULL_OPACITY && handle->userInputRegistered) ||
        handle->timer <= TIME_FULL_OPACITY) {
        handle->timer += GetFrameTime();
    } else if (handle->timer > TIME_FULL_OPACITY) {
        if (IsKeyPressed(KEY_ENTER)) {
            handle->userInputRegistered = true;
        }
    }

    handle->color.a = getOpacity(handle->timer);
}

void text_draw(TextHandle handle) {
    if (handle->timer > TIME_END)
        return;
    DrawTextEx(handle->font,
               handle->texts[LVL_INDX(handle->currentLevel)],
               (Vector2){0.0f, 0.0f},
               (float)handle->font.baseSize * 4.0f,
               2,
               handle->color);
}
