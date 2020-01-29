#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "screen.h"

/*
 * start_game
 *  Begins the speedtest game. Control is taken over
 *  from here until the player quits.
 */
void start_words(int argc, char* argv[]);

#endif /* Include guard */
