#include "include/sounds.h"
#include "include/raylib/raylib.h"

#define FOOTSTEPS_WAIT 0.4f
/**
 * Set between 1 and 100
 * Pitch is set randomly between that and 100 
 */
#define FOOTSTEP_RAND_PERC_START_PITCH 90

/**
 * @brief Manage sounds during game
 * @details Instead of handling the sound with opaque pointer
 * the sound is kept global in this file because it is used
 * in so many files
 */
typedef struct SoundManager {
    Sound footsteps;
	Sound jump;
	Sound landing;
    float footstepsTimer;
} SoundManager;

static SoundManager manager;

void sound_init(void) {
	InitAudioDevice();
    manager.footsteps = LoadSound("assets/sounds/metal_footstep.mp3");
	SetSoundVolume(manager.footsteps, 0.3f);
    manager.jump = LoadSoundAlias(manager.footsteps);
	SetSoundVolume(manager.jump, 0.3f);
	SetSoundPitch(manager.jump, 0.7f);
    manager.landing = LoadSoundAlias(manager.footsteps);
	SetSoundVolume(manager.landing, 0.3f);
	SetSoundPitch(manager.landing, 1.5f);
	manager.footstepsTimer = 0.0f;
}

void sound_free(void) {
    UnloadSound(manager.footsteps);
	CloseAudioDevice();
}

void sound_playFootstep(void) {
    manager.footstepsTimer += GetFrameTime();
    if (manager.footstepsTimer >= FOOTSTEPS_WAIT) {
		int rand = GetRandomValue(FOOTSTEP_RAND_PERC_START_PITCH, 100);
		SetSoundPitch(manager.footsteps, 1.0f *((float)rand/100));
        PlaySound(manager.footsteps);
		manager.footstepsTimer = 0.0f;
    }
}

void sound_resetFootstep(void) {
	manager.footstepsTimer = FOOTSTEPS_WAIT * 0.75f;
}

void sound_playJump(void) {
	PlaySound(manager.jump);
}

void sound_landing(void) {
	PlaySound(manager.landing);
}
