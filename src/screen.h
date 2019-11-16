#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <assert.h>

/*
 * COLORS
 * The colors are specified as 
 * FOREGROUND on BACKGROUND
 * each is a bit mask for the color setting.
 * It is possible to set the individual colors
 * by calling setForegroundColor and setBackgroundColor.
 *
 * Bit encoded as follows:
 * -FG- -BG- --CH AR--
 * 0000 0000 0000 0000
 */
#define FG_MASK 0xF000
#define BG_MASK 0x0F00
#define COLOR_MASK 0xFF00
#define CHAR_MASK 0x00FF

enum COLORS
{
  /* Black foregound on X */
  COLOR_BLK_ON_BLK = 0x0000,
  COLOR_BLK_ON_RED = 0x0100,
  COLOR_BLK_ON_GRN = 0x0200,
  COLOR_BLK_ON_YLW = 0x0300,
  COLOR_BLK_ON_BLU = 0x0400,
  COLOR_BLK_ON_MAG = 0x0500,
  COLOR_BLK_ON_CYN = 0x0600,
  COLOR_BLK_ON_WHT = 0x0700,
  /* Red foregounrd on X */ 
  COLOR_RED_ON_BLK = 0x1000,
  COLOR_RED_ON_RED = 0x1100,
  COLOR_RED_ON_GRN = 0x1200,
  COLOR_RED_ON_YLW = 0x1300,
  COLOR_RED_ON_BLU = 0x1400,
  COLOR_RED_ON_MAG = 0x1500,
  COLOR_RED_ON_CYN = 0x1600,
  COLOR_RED_ON_WHT = 0x1700,
  /* Green foregound on X */
  COLOR_GRN_ON_BLK = 0x2000,
  COLOR_GRN_ON_RED = 0x2100,
  COLOR_GRN_ON_GRN = 0x2200,
  COLOR_GRN_ON_YLW = 0x2300,
  COLOR_GRN_ON_BLU = 0x2400,
  COLOR_GRN_ON_MAG = 0x2500,
  COLOR_GRN_ON_CYN = 0x2600,
  COLOR_GRN_ON_WHT = 0x2700,
  /* Yellow foregound on X */
  COLOR_YLW_ON_BLK = 0x3000,
  COLOR_YLW_ON_RED = 0x3100,
  COLOR_YLW_ON_GRN = 0x3200,
  COLOR_YLW_ON_YLW = 0x3300,
  COLOR_YLW_ON_BLU = 0x3400,
  COLOR_YLW_ON_MAG = 0x3500,
  COLOR_YLW_ON_CYN = 0x3600,
  COLOR_YLW_ON_WHT = 0x3700,
  /* Blue foregound on X */ 
  COLOR_BLU_ON_BLK = 0x4000,
  COLOR_BLU_ON_RED = 0x4100,
  COLOR_BLU_ON_GRN = 0x4200,
  COLOR_BLU_ON_YLW = 0x4300,
  COLOR_BLU_ON_BLU = 0x4400,
  COLOR_BLU_ON_MAG = 0x4500,
  COLOR_BLU_ON_CYN = 0x4600,
  COLOR_BLU_ON_WHT = 0x4700,
  /* Magintia foreground on X */
  COLOR_MAG_ON_BLK = 0x5000,
  COLOR_MAG_ON_RED = 0x5100,
  COLOR_MAG_ON_GRN = 0x5200,
  COLOR_MAG_ON_YLW = 0x5300,
  COLOR_MAG_ON_BLU = 0x5400,
  COLOR_MAG_ON_MAG = 0x5500,
  COLOR_MAG_ON_CYN = 0x5600,
  COLOR_MAG_ON_WHT = 0x5700,
  /* Cyan foreground on X */
  COLOR_CYN_ON_BLK = 0x6000,
  COLOR_CYN_ON_RED = 0x6100,
  COLOR_CYN_ON_GRN = 0x6200,
  COLOR_CYN_ON_YLW = 0x6300,
  COLOR_CYN_ON_BLU = 0x6400,
  COLOR_CYN_ON_MAG = 0x6500,
  COLOR_CYN_ON_CYN = 0x6600,
  COLOR_CYN_ON_WHT = 0x6700,
  /* White foregound on X */
  COLOR_WHT_ON_BLK = 0x7000,
  COLOR_WHT_ON_RED = 0x7100,
  COLOR_WHT_ON_GRN = 0x7200,
  COLOR_WHT_ON_YLW = 0x7300,
  COLOR_WHT_ON_BLU = 0x7400,
  COLOR_WHT_ON_MAG = 0x7500,
  COLOR_WHT_ON_CYN = 0x7600,
  COLOR_WHT_ON_WHT = 0x7700
}; 
enum FG_COLOR
{
  FG_COLOR_BLK = 0x0000,
  FG_COLOR_RED = 0x1000,
  FG_COLOR_GRN = 0x2000,
  FG_COLOR_YLW = 0x3000,
  FG_COLOR_BLU = 0x4000,
  FG_COLOR_MAG = 0x5000,
  FG_COLOR_CYN = 0x6000,
  FG_COLOR_WHT = 0x7000
};
enum BG_COLOR
{
  BG_COLOR_BLK = 0x0000,
  BG_COLOR_RED = 0x0100,
  BG_COLOR_GRN = 0x0200,
  BG_COLOR_YLW = 0x0300,
  BG_COLOR_BLU = 0x0400,
  BG_COLOR_MAG = 0x0500,
  BG_COLOR_CYN = 0x0600,
  BG_COLOR_WHT = 0x0700
}; 

static inline uint16_t setColor(uint16_t n, enum COLORS c)
{
  n &= CHAR_MASK;
  n |= c;
  return n;
} 

static inline uint16_t setForegroundColor(uint16_t n, enum FG_COLOR c) 
{
  n &= BG_MASK | CHAR_MASK;
  n |= c;
  return n;
} 

static inline uint16_t setBackgroundColor(int16_t n, enum BG_COLOR c)
{
  n &= FG_MASK | CHAR_MASK;
  n |= c;
  return n;
} 

static inline uint16_t setCharacter(uint16_t n, char c)
{
  n &= COLOR_MASK;
  n |= c & CHAR_MASK;
  return n;
} 

static inline char getCharacter(uint16_t n)
{
  return n & CHAR_MASK;
} 

static inline enum FG_COLOR getForegroundColor(uint16_t n)
{
  return n & FG_MASK;
} 

static inline enum BG_COLOR getBackgroundColor(uint16_t n)
{
  return n & BG_MASK;
} 

static inline enum COLORS getColor(uint16_t n)
{
  return n & 0xFF00;
} 

/* TODO
 * Thinking about this.....
 *
struct Screen
{
  uint16_t ** s;
  struct Display d;
}; 
*/

enum KEYMAP {
  CTRL_C = 3,   /* ETX - quit */
  BACK   = 8,   /* Backspace */
  CR     = 13,  /* Enter */
  CTRL_Q = 17,  /* Quit */
  ESC    = 27,  /* Escape */
  DEL    = 127, /* Delete */
  EXIT   = 0    /* Exit the program gracefuly. */
};

struct Display
{
  /* Size of the terminal */
  uint16_t maxCols, maxRows;
  /* Location of game window. 
   * It runs the width of the screen. */
  uint16_t textWinStartY, textWinEndY;
  /* Location of the input section. */
  uint16_t inputX, inputY;
  /* Location of user score */
  uint16_t scoreX, scoreY;
  /* Location of timer */
  uint16_t timerX, timerY;
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
 * moveCursorPositionTo
 * Moves the cursor position to the x, y coordinates. Note 
 * the coordinates are switched, to be in line with ncurses.
 *
 * @y Y position
 * @x Y position
 */
void moveCursorTo(int y, int x);

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

/*
 * getInput
 *  Get the next input from the user. This function does not block as
 *  it immeditally returns even if no input has been entered. See rawMode()
 *  term.c_cc[VMIN] and term.c_cc[VTIME] for setup.
 *
 *  returns Character user enterd, some value in enum KEYMAP or nothing.
 */
char getInput();

/*
 * writeString
 *  Write a string to the screen. It will write exactly where the cursor
 *  is. 
 *  @content String to be written
 *  @size Size of string
 *  returns Number of bytes written to screen. 
 */
int writeString(const char* content, unsigned int size);
int writeCharacter(uint16_t content);
