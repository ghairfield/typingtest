#ifndef SCREEN_H_
#define SCREEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

enum COLORS
{
  /* Black foregound on X */
  COLOR_BLK_ON_RED = 0x0100,
  COLOR_BLK_ON_GRN = 0x0200,
  COLOR_BLK_ON_YLW = 0x0300,
  COLOR_BLK_ON_BLU = 0x0400,
  COLOR_BLK_ON_MAG = 0x0500,
  COLOR_BLK_ON_CYN = 0x0600,
  COLOR_BLK_ON_WHT = 0x0700,
  /* Red foregounrd on X */ 
  COLOR_RED_ON_BLK = 0x1000,
  COLOR_RED_ON_GRN = 0x1200,
  COLOR_RED_ON_YLW = 0x1300,
  COLOR_RED_ON_BLU = 0x1400,
  COLOR_RED_ON_MAG = 0x1500,
  COLOR_RED_ON_CYN = 0x1600,
  COLOR_RED_ON_WHT = 0x1700,
  /* Green foregound on X */
  COLOR_GRN_ON_BLK = 0x2000,
  COLOR_GRN_ON_RED = 0x2100,
  COLOR_GRN_ON_YLW = 0x2300,
  COLOR_GRN_ON_BLU = 0x2400,
  COLOR_GRN_ON_MAG = 0x2500,
  COLOR_GRN_ON_CYN = 0x2600,
  COLOR_GRN_ON_WHT = 0x2700,
  /* Yellow foregound on X */
  COLOR_YLW_ON_BLK = 0x3000,
  COLOR_YLW_ON_RED = 0x3100,
  COLOR_YLW_ON_GRN = 0x3200,
  COLOR_YLW_ON_BLU = 0x3400,
  COLOR_YLW_ON_MAG = 0x3500,
  COLOR_YLW_ON_CYN = 0x3600,
  COLOR_YLW_ON_WHT = 0x3700,
  /* Blue foregound on X */ 
  COLOR_BLU_ON_BLK = 0x4000,
  COLOR_BLU_ON_RED = 0x4100,
  COLOR_BLU_ON_GRN = 0x4200,
  COLOR_BLU_ON_YLW = 0x4300,
  COLOR_BLU_ON_MAG = 0x4500,
  COLOR_BLU_ON_CYN = 0x4600,
  COLOR_BLU_ON_WHT = 0x4700,
  /* Magintia foreground on X */
  COLOR_MAG_ON_BLK = 0x5000,
  COLOR_MAG_ON_RED = 0x5100,
  COLOR_MAG_ON_GRN = 0x5200,
  COLOR_MAG_ON_YLW = 0x5300,
  COLOR_MAG_ON_BLU = 0x5400,
  COLOR_MAG_ON_CYN = 0x5600,
  COLOR_MAG_ON_WHT = 0x5700,
  /* Cyan foreground on X */
  COLOR_CYN_ON_BLK = 0x6000,
  COLOR_CYN_ON_RED = 0x6100,
  COLOR_CYN_ON_GRN = 0x6200,
  COLOR_CYN_ON_YLW = 0x6300,
  COLOR_CYN_ON_BLU = 0x6400,
  COLOR_CYN_ON_MAG = 0x6500,
  COLOR_CYN_ON_WHT = 0x6700,
  /* White foregound on X */
  COLOR_WHT_ON_BLK = 0x7000,
  COLOR_WHT_ON_RED = 0x7100,
  COLOR_WHT_ON_GRN = 0x7200,
  COLOR_WHT_ON_YLW = 0x7300,
  COLOR_WHT_ON_BLU = 0x7400,
  COLOR_WHT_ON_MAG = 0x7500,
  COLOR_WHT_ON_CYN = 0x7600,
}; 

enum KEYMAP {
  EXIT   = 0,   /* Exit the program gracefuly. */
  CTRL_C = 3,   /* ETX - quit */
  BACK   = 8,   /* Backspace */
  CR     = 13,  /* Enter */
  CTRL_Q = 17,  /* Quit */
  ESC    = 27,  /* Escape */
  DEL    = 127, /* Delete */

  /* Program defined mappings */
  ARROW_U = 1000,
  ARROW_D = 1001,
  ARROW_L = 1002,
  ARROW_R = 1003,
};

/**
setCursorOn
Turn on the cursor on the screen. This is really only good when
not writing to the screen. If cursor is on when a write occurs,
a blip will float around the screen. Good to keep off while the
game is running.
*/
static inline void setCursorOn()
{
  write(STDOUT_FILENO, "\033[?25h", 6);
} 

/**
setCursorOff
Turns the cursor off.
*/
static inline void setCursorOff()
{
  write(STDOUT_FILENO, "\033[?25l", 6);
} 

/*
getMaxYX
Gets the size of the screen when init was called. 
@y size of screen Y 
@x size of screen X
TODO: We should update the screen size if the screen size
is changed.
 */
void getMaxYX(int *y, int *x);

/*
moveCursorPositionTo
Moves the cursor position to the x, y coordinates. Note 
 the coordinates are switched, to be in line with ncurses.

@y Y position
@x Y position
 */
void moveCursorTo(int y, int x);

/**
writeScreen
Writes the framebuffer to the screen.
*/
void writeScreen();

/*
clearScreen
Clears the screen and moved the cursor to the home position.
 */
void clearScreen();

/*
 * screenInit : Must be called first!
 *  Initializes the screen. Saves the current terminal enviroment
 *  and enters raw mode. This enables cursor and color control using
 *  VT100 codes. 
 */
void screenInit();

/*
 * screenDestroy
 *  Must be called once done with screen. Returns the terminal to its
 *  previous state, deletes all memory.
 */
void screenDestroy();

/**
getInput
Get the next input from the user. This function does not block as
  it immeditally returns even if no input has been entered. See rawMode()
  term.c_cc[VMIN] and term.c_cc[VTIME] for setup.

returns Character user enterd, some value in enum KEYMAP or nothing.
 */
char getInput();

/**
setColor
Sets the current write color to c. Any writing after this keeps this profile. 
*/
void setColor(enum COLORS c);

/**
writeString
Write a string to the framebuffer. It will write exactly where the cursor
  is and advance the cursor by 1. 

@content String to be written
@size Size of string
returns Number of bytes written to screen. 
 */
int writeString(const char* content, unsigned int size);
int writeCharacter(char content);

#endif /* Include guard */
