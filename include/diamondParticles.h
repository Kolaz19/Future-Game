#ifndef DIAMOND_PARTICLES_H
#define DIAMOND_PARTICLES_H

typedef struct DiamondParticle* ParticleHandler;

ParticleHandler diap_init(int xStart, int yUpStart, int yDownStart);
void diap_free(ParticleHandler handler);
void diap_update(ParticleHandler handler, float lifetime, int diaPosX, int diaPosY);
void diap_drawParticles(ParticleHandler handler);
#endif
