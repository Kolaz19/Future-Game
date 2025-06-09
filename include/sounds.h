#ifndef SOUNDS_H
#define SOUNDS_H


/**
 * @brief Load all sounds needed during the game
 */
void sound_init(void);
/**
 * @brief Unload all sounds used during the game
 */
void sound_free(void);

void sound_playFootstep(void);
void sound_resetFootstep(void);
void sound_playJump(void);
void sound_landing(void);

#endif
