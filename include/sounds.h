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
/**
 * @brief Pause sounds of platforms for some time after 
 * calling this function
 */
void sound_playFootstep(void);
void sound_playJump(void);
void sound_landing(void);
void sound_platforms(void);
void sound_death(void);
void sound_platformsMoving(void);

#endif
