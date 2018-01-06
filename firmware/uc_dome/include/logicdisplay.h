/**
 * @file logicdisplay.h
 * @author Denise Ratasich
 * @date 2017-11-07
 *
 * @brief Header of logic display driver.
 */

#ifndef __LOGICDISPLAY_H__
#define __LOGICDISPLAY_H__

typedef enum {
    LOGICDISPLAY_RANDOM = 0, // normal operation
    LOGICDISPLAY_CHAR, // e.g., for debuging
    LOGICDISPLAY_NUM_MODES
} logicdisplay_mode_t;

/** Initialize and start the logic displays. */
void logicdisplay_init(void);

/** Change display mode. */
void logicdisplay_mode(logicdisplay_mode_t new_mode);

/** Sets the character to display.
 * @note The mode needs to be changed explicitely to display the characters. */
void logicdisplay_print(const char *front_up, const char *front_lo,
                        const char *rear);

#endif
