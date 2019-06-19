/**
 * Greg Hairfield
 * A speed typing test for fun and profit. 
 * June 18, 2019
 */
#include "fileio.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Global def's
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
enum KEYMAP {
  CTRL_C = 3,   // ETX
  BACK   = 8,   // Backspace
  CR     = 13,  // Enter
  CTRL_Q = 17,  // Quit
  ESC    = 27,  // Escape 
  DEL    = 127, // Delete 
  EXIT   = 0    // Exit the program gracefuly.
};

enum COLORS {
  NORMAL = 0,
  GREENONBLACK,
  REDONBLACK,
  YELLOWONBLACK,
  BAD_INPUT,
  COMPLETE
};

#define ttyin STDIN_FILENO
#define ttyout STDOUT_FILENO

// Basic cursor movements
const char* CUR_HOME  = "\033[H";
const char* CUR_LEFT  = "\033[%dD";
const char* CUR_RIGHT = "\033[%dC";
const char* CUR_UP    = "\033[%dA";
const char* CUR_DOWN  = "\033[%dB";
// Single movements
const char* CUR_CLEAR     = "\033[2J";
const char* CUR_LEFT_ONE  = "\033[D";
const char* CUR_RIGHT_ONE = "\033[C";
const char* CUR_UP_ONE    = "\033[A";
const char* CUR_DOWN_ONE  = "\033[B";
char* cur_string;
const int cur_string_sz = 40;

// Global constants
const float game_width_factor  = 0.80f;
const float game_height_factor = 0.80f;
const int   max_lines          = 1000;
const int   line_spacing       = 2;
const char* game_settings[] = { \
/* 0  */    "Game Settings -",    \
/* 1  */    "Columns      : %d",  \
/* 2  */    "Rows         : %d",  \
/* 3  */    "Game Board Size  ",  \
/* 4  */    "        Cols : %d",  \
/* 5  */    "        Rows : %d",  \
/* 6  */    " Factor Cols : %.2f",\
/* 7  */    " Factor Rows : %.2f",\
/* 8  */    "Columns            ",\
/* 9  */    "     Start   : %d",  \
/* 10 */    "     End     : %d",  \
/* 11 */    "Rows             ",  \
/* 12 */    "     Start   : %d",  \
/* 13 */    "     End     : %d",  \
/* 14 */    "Speed        : %.2f",\
/* 15 */     NULL };

struct Game
{
  int tot_cols;     /**< Window size X */
  int tot_rows;     /**< Window size Y */
  int sz_cols;      /**< Board size X */
  int sz_rows;      /**< Board size Y */
  int st_cols;      /**< First column of the board */
  int end_cols;     /**< Last column of the board */
  int st_rows;      /**< First row of the board */
  int end_rows;     /**< Last row of the board */

  int rl;           /**< The line which the current input line is. Middle of the screen. */

  struct Line** ln; /**< Test material */
  int lsz;          /**< Line size - number of ln's */
  int lri;          /**< Line row index */
  int lci;          /**< Line column index  */

  char* fn;         /**< Current file name */
} g;

struct Player
{
  char** input;     /**< The contents the user entered TODO: Not sure we are going to need this. */

  int correct;      /**< Number of correct keystrokes */
  int error;        /**< Number of error keystrokes */
  int words;        /**< Number of words entered */
  double score;
  clock_t start; 
} p;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Helpers
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void clear_cursor_string()
{
  memset(cur_string, '\0', cur_string_sz);
}

void setColor(enum COLORS c)
{
  switch (c) {
    case NORMAL:
      write(ttyout, "\033[37;40m", 8);
      break;
    case GREENONBLACK:
      write(ttyout, "\033[32;40m", 8);
      break;
    case REDONBLACK:
      write(ttyout, "\033[31;40m", 8);
      break;
    case YELLOWONBLACK:
      write(ttyout, "\033[33;40m", 8);
      break;
    case BAD_INPUT:
      write(ttyout, "\033[30;41m", 8);
      break;
    case COMPLETE:
      write(ttyout, "\033[1;40m", 7);
      break;
  };
}

void scbp(int x, int y)
{
  assert(x >= 0 && y >= 0);

  // Move home
  write(ttyout, CUR_HOME, strlen(CUR_HOME));
  // Copy -Y position to string and write to terminal
  sprintf(cur_string, CUR_DOWN, y + g.st_rows);
  write(ttyout, cur_string, strlen(cur_string)); 
  clear_cursor_string();
  // Copy X position to string and write to terminal
  sprintf(cur_string, CUR_RIGHT, x + g.st_cols);
  write(ttyout, cur_string, strlen(cur_string)); 
  clear_cursor_string();
}

void display_game_settings()
{
  int startx = 0;
  int starty = 0;
  char string[100] = { '\0' };

  // Move cursor to starting position
  write(ttyout, CUR_CLEAR, strlen(CUR_CLEAR)); 
  // Write first line
  scbp(startx, starty++);
  write(ttyout, game_settings[0], strlen(game_settings[0]));
  startx += 6;

  scbp(startx, starty++);
  sprintf(string, game_settings[1], g.tot_cols);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[2], g.tot_rows);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[3]);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[4], g.sz_cols);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[5], g.sz_rows);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[6], game_width_factor);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[7], game_height_factor);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[8]);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[9], g.st_cols);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[10], g.end_cols);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[11]);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[12], g.st_rows);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[13], g.end_rows);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty);
  sprintf(cur_string, game_settings[14], 1.23);
  write(ttyout, cur_string, strlen(string)); 

  write(ttyout, "\033[1000B", 7);
  write(ttyout, "\033[1000D", 7);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Input handling
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* We ignore most input that doesn't apply to this program. */
char getInput()
{
  char c_in;
  int bytes;

  bytes = read(ttyin, &c_in, 1);
  if (bytes == -1) return EXIT; // Failure

  if (c_in == CTRL_C || c_in == CTRL_Q) return EXIT; // User quit

  if (c_in == DEL || c_in == BACK) return DEL; // Backspace

  return c_in; // Regular character
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Game
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void display_lines()
{
  if ( !g.ln) return;
  /**
   * g.rl should be the line we want in the middle of the screen.
   * g.lri is the line that should appear there.
   */
  int startline = g.lri;
  scbp(0, 0);
  while (startline * line_spacing < g.sz_rows && startline < g.lsz) {
    write(ttyout, g.ln[startline]->ln, g.ln[startline]->sz);  
    scbp(0, ++startline * line_spacing);
  }
}

int run()
{
  // Initilize the user info
  p.start = clock();
  scbp(0, 0); 

  char c;
  while (1) {
    c = getInput();

    switch (c) {
      case EXIT:
        return 0;
        break;
      case DEL:
        if (g.lci <= 0) {
          // We are at the left most position on the screen
          if (g.lri > 0) {
            --g.lri;
            g.lci = g.ln[g.lri]->sz - 1;
            scbp(g.lci, g.lri * line_spacing);
            write(ttyout, &g.ln[g.lri]->ln[g.lci], 1);
            write(ttyout, CUR_LEFT_ONE, strlen(CUR_LEFT_ONE));
          }
        }
        else {
          // At some position in the line
          --g.lci;
          write(ttyout, CUR_LEFT_ONE, strlen(CUR_LEFT_ONE));
          write(ttyout, &g.ln[g.lri]->ln[g.lci], 1);
          write(ttyout, CUR_LEFT_ONE, strlen(CUR_LEFT_ONE));
        } 
        break;
      case CR:
        if (g.lri + 1 < g.lsz) {
          // We can move down a line.
          if (g.lci < g.ln[g.lri]->sz) {
            // Mark the rest of the line as error, and add errors to the player
            setColor(REDONBLACK);
            for (int i = g.lci; i < g.ln[g.lri]->sz; ++i) {
              write(ttyout, &g.ln[g.lri]->ln[i], 1);
              ++p.error;
            }
            setColor(NORMAL);
          }
          ++g.lri;
          g.lci = 0;
          scbp(g.lci, g.lri * line_spacing);
        }
        break;
      default:
        // Check that the entered text matches and add approiate color
        p.input[g.lri][g.lci] = c;
        ++p.correct;
        if (c == g.ln[g.lri]->ln[g.lci]) {
          setColor(GREENONBLACK);
          write(ttyout, &g.ln[g.lri]->ln[g.lci], 1);
          setColor(NORMAL);
        }
        else {
          ++p.error;
          setColor(REDONBLACK);
          write(ttyout, &g.ln[g.lri]->ln[g.lci], 1);
          setColor(NORMAL);
        }

        if (g.lci < g.sz_cols) ++g.lci;
    }; 
  }

  return c;
} 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Setup
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct termios orig_term;

void clean_up()
{
  /* Flush the terminal upon exit, and reset the original terminal settings. */
  tcsetattr(ttyin, TCSAFLUSH, &orig_term);

  if (g.ln) {
    destroy_lines(g.ln, g.lsz);
    free(g.ln);
  }
  if (g.fn) free(g.fn);
  free(cur_string);

  if (p.input) {
    for (int i = 0; i < g.sz_rows; ++i) {
      free(p.input[i]);
    }
    free(p.input);
  }

}

void raw_mode()
{
  /*
   * We can turn on raw mode with
   *      int cfmakeraw(struct termios *termios-p)
   * is equal to
   *  termios-p->c_iflag &= ~(IGNBRK | BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
   *  termios-p->coiflag &= ~OPOST;
   *  termios-p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
   *  termios-p->c_cflag &= ~(CSIZE|PARENB);
   *  termios-p->c_flag |= CS8;
   */
  if ( !isatty(ttyin)) {
    fprintf(stderr, "This is not a tty.\n");
    exit(EXIT_FAILURE);
  }
  // Copy the original terminal settings so they can be set upon exit
  tcgetattr(ttyin, &orig_term); 

  struct termios term = orig_term;
  /* Trun off break (enter CBREAK)
   * Trun off CR to NL
   * Turn off parity check
   * Turn off strip char (8 bit characters) */
  term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | ISIG);
  /* Turn off echo, automattically writing to screen.
   * Turn off canonical mode. Read bytes instead of lines. 
   * Turn off Ctrl-Q and Ctrl-S, legacy terminal modes. */
  term.c_lflag &= ~(ECHO | ICANON | IXON); 
  /* Do not ost process NL to CR+NL */
  term.c_oflag &= ~(OPOST);  
  
  term.c_cc[VMIN] = 1;  // Read a mimium of 1 bytes
  term.c_cc[VTIME] = 0; // No wait
  tcsetattr(ttyin, TCSAFLUSH, &term);
}

int init_game()
{
  struct winsize ws;
  ioctl(ttyout, TIOCGWINSZ, &ws);
  g.tot_rows = ws.ws_row;
  g.tot_cols = ws.ws_col;

  int middlecols = g.tot_cols / 2;
  int middlerows = g.tot_rows / 2;

  // Set up the game 
  g.sz_cols = g.tot_cols * game_width_factor;
  g.sz_rows = g.tot_rows * game_height_factor;

  g.st_cols  = g.tot_cols - (middlecols + (g.sz_cols / 2));
  g.end_cols = g.tot_cols - (middlecols - (g.sz_cols / 2));
  g.st_rows  = g.tot_rows - (middlerows + (g.sz_rows / 2));
  g.end_rows = g.tot_rows - (middlerows - (g.sz_rows / 2));

  g.ln = NULL;
  g.lsz = 0;
  g.lri = 0;
  g.lci = 0;
  g.fn = NULL;
  g.rl = (g.sz_rows / line_spacing) / 2;

  cur_string = malloc(cur_string_sz * sizeof(char));
  if ( !cur_string) return -1;
  memset(cur_string, '\0', 20);
  
  // Set up the player
  p.correct = 0;
  p.error   = 0;
  p.words   = 0;
  p.score   = 0.0f;

  p.input = malloc(g.sz_rows * sizeof(char *));
  if ( !p.input) return -1;
  for (int i = 0; i < g.sz_rows; ++i) {
    p.input[i] = malloc(g.sz_cols * sizeof(char));
    if ( !p.input[i]) return -1;
    memset(p.input[i], '\0', g.sz_cols);
  }

  raw_mode();
  setColor(NORMAL);
  write(ttyout, CUR_CLEAR, strlen(CUR_CLEAR));

  return 0;
}

int main()
{
   init_game();
// Set the file information.
  char* fn = "data/alice.txt";
  g.fn = malloc(strlen(fn) * sizeof(char) + 1);
  strcpy(g.fn, fn);
// Set up the lines struct
  g.ln = malloc(max_lines * sizeof(struct Line *));
  memset(g.ln, '\0', max_lines);

  g.lsz = read_file(g.fn, g.ln, max_lines, g.sz_cols);
  if (g.lsz <= 0) exit(EXIT_FAILURE);

//  display_lines();
//  run();
  display_game_settings();

  clean_up();
  // Close all file descripters for valgrind error summary.
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
  return 0;
} 
