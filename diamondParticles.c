#include "include/diamondParticles.h"
#include "include/raylib/raylib.h"
#include "include/raylib/raymath.h"
#include <stdlib.h>

#define NUMBER_OF_PARTICLES 20
#define SPAWN_DELAY_MAX 0.5f
#define PARTICLE_SPEED 1.0f

#define MAX_OFFSET_X 10
#define MAX_OFFSET_Y 25

#define MIN_PARTICLE_SIZE 200
#define MAX_PARTICLE_SIZE 400

#define PURPLE_OPAQUE \
    CLITERAL(Color) { 200, 122, 255, 200 } // Purple

typedef enum Direction {
    UP,
    DOWN
} Direction;

typedef struct DiamondParticle {
    Vector2 pos;
    Vector2 targetOffset;
    Direction dir;
    float startLife;
    Color color;
    float size;
    bool active;
} DiaParticle;

ParticleHandler diap_init(int xStart, int yUpStart, int yDownStart) {
    ParticleHandler particles = malloc(sizeof(struct DiamondParticle) * NUMBER_OF_PARTICLES);
    float time = 0.0f;
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        time += (float)GetRandomValue(0, (int)(SPAWN_DELAY_MAX * 100)) / 100.0f;
        particles[i].startLife = time;

        particles[i].pos.x = (float)xStart;
        particles[i].pos.y = (float)GetRandomValue(yUpStart, yDownStart);
        particles[i].dir = GetRandomValue(0, 1) == 0 ? UP : DOWN;
        particles[i].active = false;

        particles[i].targetOffset.x = (float)GetRandomValue(MAX_OFFSET_X * -1, MAX_OFFSET_X);
        particles[i].targetOffset.y = (float)GetRandomValue(MAX_OFFSET_Y * -1, MAX_OFFSET_Y);
        particles[i].color = PURPLE_OPAQUE;
        particles[i].size = (float)GetRandomValue(MIN_PARTICLE_SIZE, MAX_PARTICLE_SIZE) / 100.0f;
    }
    return particles;
}

void diap_free(ParticleHandler handler) {
    free(handler);
}

void diap_update(ParticleHandler handler, float lifetime, int diaPosX, int diaPosY) {
    Vector2 diamond = {(float)diaPosX, (float)diaPosY};
    Vector2 diamondOffset;
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        if (handler[i].startLife <= lifetime) {
            handler[i].active = true;
            diamondOffset = Vector2Add(diamond, handler[i].targetOffset);

            // Stayed in same x-place since last frame
            if (diamondOffset.x == handler[i].pos.x) {
                if (handler[i].color.a != 0) {
                    handler[i].color.a--;
                }
            }
            handler[i].pos = Vector2MoveTowards(handler[i].pos, diamondOffset, PARTICLE_SPEED);
        }
    }
}

int diap_percentageFinished(ParticleHandler handler) {
	int amount = 0;
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        if (handler[i].color.a == 0) {
			amount++;
		}
	}
	return (int)(amount * 100 / NUMBER_OF_PARTICLES);
}

void diap_drawParticles(ParticleHandler handler) {
    for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
        if (handler[i].active) {
            DrawCircle((int)handler[i].pos.x, (int)handler[i].pos.y, handler[i].size, handler[i].color);
        }
    }
}
