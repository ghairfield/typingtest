#ifndef GAME_H_
#define GAME_H_

#include <stdint.h>
#include "screen.h"

uint16_t* makeString(const char* s, int sz, enum COLORS co);

/*
 * start_game
 *  Begins the speedtest game. Control is taken over
 *  from here until the player quits.
 */
void start_game();

#endif /* Include guard */
