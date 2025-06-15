#include "include/sounds.h"
#include "include/raylib/raylib.h"

#define FOOTSTEPS_WAIT 0.4f
#define RESET_SEC_WAIT 1
#define MAX_PLATFORM_SOUNDS 10
/**
 * Set between 1 and 100
 * Pitch is set randomly between that and 100
 */
#define FOOTSTEP_RAND_PERC_START_PITCH 90
#define PLATFORM_RAND_PERC_START_PITCH 80
#define PLATFORM_RAND_PERC_END_PITCH 90

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
	Sound death;
    Sound platforms[MAX_PLATFORM_SOUNDS];
    int curPlatformSoundIndex;
    float footstepsTimer;
	double timeStamp;
} SoundManager;

static SoundManager manager;

void sound_init(void) {
    InitAudioDevice();
    manager.footsteps = LoadSound("assets/sounds/metal_footstep.wav");
    SetSoundVolume(manager.footsteps, 0.8f);
    manager.jump = LoadSound("assets/sounds/metal_jump.wav");
    SetSoundVolume(manager.jump, 3.0f);
    SetSoundPitch(manager.jump, 1.0f);
    manager.landing = LoadSound("assets/sounds/metal_land.wav");
    SetSoundVolume(manager.landing, 0.2f);
    SetSoundPitch(manager.landing, 0.9f);
    manager.death = LoadSound("assets/sounds/death.wav");

    for (int i = 0; i < MAX_PLATFORM_SOUNDS; i++) {
        if (i == 0) {
            manager.platforms[i] = LoadSound("assets/sounds/metalhard.mp3");
        } else {
            manager.platforms[i] = LoadSoundAlias(manager.platforms[0]);
        }
    }
    manager.curPlatformSoundIndex = 0;
    manager.footstepsTimer = 0.0f;
	manager.timeStamp = GetTime();
}

void sound_free(void) {
    UnloadSound(manager.footsteps);
    CloseAudioDevice();
}

void sound_playFootstep(void) {
    manager.footstepsTimer += GetFrameTime();
    if (manager.footstepsTimer >= FOOTSTEPS_WAIT) {
        int rand = GetRandomValue(FOOTSTEP_RAND_PERC_START_PITCH, 100);
        SetSoundPitch(manager.footsteps, 1.0f * ((float)rand / 100));
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
void sound_death(void) {
    PlaySound(manager.death);
}


void sound_platforms(void) {
	if (GetTime() - manager.timeStamp <= RESET_SEC_WAIT) {
		return;
	}
    int randPitch = GetRandomValue(PLATFORM_RAND_PERC_START_PITCH, PLATFORM_RAND_PERC_END_PITCH);
    SetSoundPitch(manager.platforms[manager.curPlatformSoundIndex], 1.0f * ((float)randPitch / 100));
    PlaySound(manager.platforms[manager.curPlatformSoundIndex]);
    manager.curPlatformSoundIndex =
        MAX_PLATFORM_SOUNDS - 1 == manager.curPlatformSoundIndex ? 0 : manager.curPlatformSoundIndex + 1;
}
