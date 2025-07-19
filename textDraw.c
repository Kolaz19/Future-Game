#include "include/textDraw.h"
#include "include/raylib/raylib.h"
#include <stdlib.h>

#define AMOUNT_TEXTS 13
#define AMOUNT_TEXTS_CREDITS 4
#define MAX_R 255

#define FIRST_TEXT_DELAY 1.8f
#define TIME_FADE_IN_OPACITY 3.0f
#define TIME_FULL_OPACITY 3.0f
#define TIME_FADE_OUT_OPACITY 3.0f
#define TIME_COMBINED (TIME_FADE_IN_OPACITY + TIME_FULL_OPACITY + TIME_FADE_OUT_OPACITY)
#define TIME_CREDITS_DELAY 6.0f

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
    bool firstLevelDelayPassed;
    TextEntity texts[AMOUNT_TEXTS];
    TextEntity credits[AMOUNT_TEXTS_CREDITS];
    bool playCredits;
	bool showScore;
    int currentCreditsIndex;
    float endCreditsDelayTimer;
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
    case 7:
        pos->x = 0.05f;
        pos->y = 0.70f;
        break;
    case 8:
        pos->x = 0.15f;
        pos->y = 0.50f;
        break;
    case 11:
        pos->x = 0.70f;
        pos->y = 0.20f;
        break;
    }
}
static void setPosCredits(Vector2 *pos, int index) {
    switch (index) {
    case 0:
        pos->x = 0.55f;
        pos->y = 0.5f;
        break;
    case 1:
        pos->x = 0.4f;
        pos->y = 0.8f;
        break;
    case 2:
        pos->x = 0.8f;
        pos->y = 0.3f;
        break;
    case 3:
        pos->x = 0.25f;
        pos->y = 0.5f;
        break;
    }
}

TextHandle text_init() {
    TextHandle text = malloc(sizeof(TextDraw));
    text->font = LoadFont("assets/pixantiqua.png");
    text->timer = TIME_COMBINED + 1.0f;
    text->currentLevel = 0;
    text->firstLevelDelayPassed = false;

    text->playCredits = false;
    text->currentCreditsIndex = 0;
    text->endCreditsDelayTimer = 0;
	text->showScore = false;

    // text->color.r = DARKPURPLE.r;
    // text->color.g = DARKPURPLE.g;
    // text->color.b = DARKPURPLE.b;
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
    text->texts[LVL_INDX(7)].text = "I am getting\ncloser to\nthe core\nI can feel it";
    text->texts[LVL_INDX(8)].text = "It is getting colder\nand colder";
    text->texts[LVL_INDX(9)].text = "";
    text->texts[LVL_INDX(10)].text = "";
    text->texts[LVL_INDX(11)].text = "I am sorry";
    text->texts[LVL_INDX(12)].text = "";
    text->texts[LVL_INDX(13)].text = "";

    text->credits[0].text = "Finally";
    text->credits[1].text = "I want to\ngo home now...";
    text->credits[2].text = "...and sleep";
    text->credits[3].text = "Thanks for playing my little game!";

    for (int i = 0; i < AMOUNT_TEXTS; i++) {
        setPos(&(text->texts[i].pos), i + 1);
    }
    for (int i = 0; i < AMOUNT_TEXTS_CREDITS; i++) {
        setPosCredits(&(text->credits[i].pos), i);
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
    if (level > AMOUNT_TEXTS) return;
    handle->currentLevel = level;
    handle->timer = 0.0f;
    handle->color.a = 0;
}

void text_activateCredits(TextHandle handle) {
    if (handle->playCredits) return;

	handle->showScore = false;

    // Yellow
    handle->color.r = 242;
    handle->color.g = 202;
    handle->color.b = 38;

    handle->playCredits = true;
    handle->timer = 0.0f;
    handle->color.a = 0;
}

void text_activateScore(TextHandle handle, double timer) {
    if (handle->showScore) return;

	if (timer < 220) {
		handle->credits[0].text = "Wtf??? 0__0\nPlEaSE sloW DOwn a BIT!\nYoU are goiNG t00 fast!!!";
	}

    handle->showScore = true;
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
    if (handle->timer > TIME_COMBINED) {
        if (handle->playCredits && handle->endCreditsDelayTimer < TIME_CREDITS_DELAY) {
            handle->endCreditsDelayTimer += GetFrameTime();
        } else if (handle->playCredits && handle->currentCreditsIndex != AMOUNT_TEXTS_CREDITS - 1) {
            handle->endCreditsDelayTimer = 0.0f;
            handle->timer = 0.0f;
            handle->color.a = 0;
            handle->currentCreditsIndex++;
        }
        return;
    }

    handle->timer += GetFrameTime();
    if (!handle->firstLevelDelayPassed) {
        if (handle->timer > FIRST_TEXT_DELAY) {
            handle->firstLevelDelayPassed = true;
            handle->timer = 0.0f;
        }
        return;
    }

    handle->color.a = getOpacity(handle->timer);
}

void text_draw(TextHandle handle) {
    if (handle->timer > TIME_COMBINED || handle->playCredits)
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

void text_draw_credits(TextHandle handle) {
    if (!handle->playCredits || handle->timer > TIME_COMBINED)
        return;

    Vector2 pos = {.x = (float)GetScreenWidth() * handle->credits[handle->currentCreditsIndex].pos.x,
                   .y = (float)GetScreenHeight() * handle->credits[handle->currentCreditsIndex].pos.y};

    DrawTextEx(handle->font,
               handle->credits[handle->currentCreditsIndex].text,
               pos,
               (float)handle->font.baseSize * FONT_SIZE,
               2,
               handle->color);
}

void text_draw_score(TextHandle handle, double timer) {
	if (!handle->showScore) return;

	DrawText(TextFormat("%.2fs", timer), 5, 2, 10, RAYWHITE);
}
