#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

enum KEYMAP {
  CTRL_C = 3,   /* ETX - quit */
  BACK   = 8,   /* Backspace */
  CR     = 13,  /* Enter */
  CTRL_Q = 17,  /* Quit */
  ESC    = 27,  /* Escape */
  DEL    = 127, /* Delete */
  EXIT   = 0    /* Exit the program gracefuly. */
};

enum COLORS {
  NORMAL = 0, /* Black on white */
  GOOD,       /* Green on black */
  WARN,       /* Yellow on black */
  DANGER,     /* Red on black */
  INPUTERR    /* White on red */
};

struct Display
{
  int cols;
  int rows;
  /*
   * TODO ADD:
   * maxCols
   * maxRows
   * x,y location of input
   * x,y location of score
   * x,y location of timer
   * size of board
   */
};

struct Player
{
  int correct;
  int error; 
  int x, y;
  double score;
  clock_t start; 
};

/*
 * setColor
 *  Set the color of the cursor. This only supports the 
 *  colors set for the game and not all color combinations.
 *
 *  @c COLORS
 *  @see enum COLORS
 */
void setColor(enum COLORS c);

/*
 * moveCursorPositionTo
 * Moves the cursor position to the x, y coordinates. Note 
 * the coordinates are switched, to be in line with ncurses.
 *
 * @y Y position
 * @x Y position
 */
void moveCursorPositionTo(int y, int x);

/*
 * clearScreen
 * Clears the screen and moved the cursor to the home position.
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
