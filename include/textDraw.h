#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include <stdbool.h>

typedef struct TextDraw* TextHandle;


TextHandle text_init();
bool text_active(TextHandle handle);
void text_activateLevelText(TextHandle handle, int level);
void text_update(TextHandle handle);
void text_draw(TextHandle handle);
void text_free(TextHandle handle);
void text_draw_credits(TextHandle handle);
void text_activateCredits(TextHandle handle);

#endif
