#include "include/diamondParticles.h"
#include "include/raylib/raylib.h"
#include "include/raylib/raymath.h"
#include <stdlib.h>

#define NUMBER_OF_PARTICLES 30
#define SPAWN_DELAY_MAX 1.5f
#define PARTICLE_SPEED 0.3f

#define MAX_OFFSET_X 10
#define MAX_OFFSET_Y 25

#define PURPLE_OPAQUE     CLITERAL(Color){ 200, 122, 255, 100 }   // Purple

typedef enum Direction {
    UP,
    DOWN
} Direction;

typedef struct DiamondParticle {
    Vector2 pos;
	Vector2 targetOffset;
    Direction dir;
    float startLife;
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
    }
    return particles;
}

void diap_free(ParticleHandler handler) {
    free(handler);
}

void diap_update(ParticleHandler handler, float lifetime, int diaPosX, int diaPosY) {
	Vector2 diamond = { (float)diaPosX, (float)diaPosY };
	Vector2 diamondOffset;
	for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
		if (handler[i].startLife <= lifetime) {
			handler[i].active = true;
			diamondOffset = Vector2Add(diamond, handler[i].targetOffset);
			handler[i].pos = Vector2MoveTowards(handler[i].pos, diamondOffset, PARTICLE_SPEED);
		}
	}
}

void diap_drawParticles(ParticleHandler handler) {
	for (int i = 0; i < NUMBER_OF_PARTICLES; i++) {
		if (handler[i].active) {
			DrawCircle((int)handler[i].pos.x, (int)handler[i].pos.y, 4, PURPLE_OPAQUE);
		}
	}
}
