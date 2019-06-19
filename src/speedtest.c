/**
 * Greg Hairfield
 * A speed typing test for fun and profit. 
 * June 18, 2019
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <stdbool.h>
#include "fileio.h"

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
  BLUEONBLACK,
  CYANONBLACK,
  MAGENTAONBLACK,
  BLACKONRED,
  REDONRED,
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

const char* game_settings[] = { 
/* 0  */    "Game Settings -",    
/* 1  */    "Columns      : %d",  
/* 2  */    "Rows         : %d",  
/* 3  */    "Game Board Size  ",  
/* 4  */    "        Cols : %d",  
/* 5  */    "        Rows : %d",  
/* 6  */    " Factor Cols : %.2f",
/* 7  */    " Factor Rows : %.2f",
/* 8  */    "Columns            ",
/* 9  */    "     Start   : %d",  
/* 10 */    "     End     : %d",  
/* 11 */    "Rows             ",  
/* 12 */    "     Start   : %d",  
/* 13 */    "     End     : %d",  
/* 14 */    "Speed        : %.2f",
/* 15 */     NULL };

const char* welcome[] = {
  "Another typing test - written in c99 with ASCII terminal codes.",
  "Written by Greg Hairfield",
  "Commands:",
  "ESC - go back a screen                                 Q - Quit",
  "L - Load a file                              S - Start the game", 
  "M - Game settings                            CTRL-C - Quit game" };

void clean_up();

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
  float score;      /**< Score */
  clock_t start;    /**< Time the game was started */
} p;

struct StatusLine
{
  char* ln;         /**< This should be g.sz_cols in size */
  int sz;           /**< Size of ln */

  int st_cols;      /**< Column status line starts on */
  int st_rows;      /**< Row of the status line */
  int tm;           /**< X position of the p.start */
  int correct;      /**< X posistion of the p.correct */
  int error;        /**< X position of p.error */
  int words;        /**< X position of p.words */
  float score;      /**< X position of p.score */
} sl;

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
    case BLACKONRED:
      write(ttyout, "\033[30;41m", 8);
      break;
    case REDONRED:
      write(ttyout, "\033[31;41m", 8);
      break;
    case BLUEONBLACK:
      write(ttyout, "\033[34;40m", 8);
      break;
    case CYANONBLACK:
      write(ttyout, "\033[36;40m", 8);
      break;
    case MAGENTAONBLACK:
      write(ttyout, "\033[35;40m", 8);
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

void display_welcome() 
{
  write(ttyout, CUR_CLEAR, strlen(CUR_CLEAR));
  int middley = g.sz_rows / 3;
  int middlex = g.sz_cols / 2;
  int offx    = middlex - (strlen(welcome[0]) / 2);
  scbp(offx, middley);
  setColor(MAGENTAONBLACK);
  write(ttyout, welcome[0], strlen(welcome[0])); 

  offx = middlex - (strlen(welcome[1]) / 2);
  middley += 2;
  scbp(offx, middley);
  write(ttyout, welcome[1], strlen(welcome[1])); 
  
  setColor(CYANONBLACK);
  offx = middlex - (strlen(welcome[2]) / 2);
  middley += 2;
  scbp(offx, middley);
  write(ttyout, welcome[2], strlen(welcome[2])); 

  offx = middlex - (strlen(welcome[3]) / 2);
  middley += 1;
  scbp(offx, middley);
  write(ttyout, welcome[3], strlen(welcome[3])); 

  offx = middlex - (strlen(welcome[4]) / 2);
  middley += 1;
  scbp(offx, middley);
  write(ttyout, welcome[4], strlen(welcome[4])); 

  offx = middlex - (strlen(welcome[5]) / 2);
  middley += 1;
  scbp(offx, middley);
  write(ttyout, welcome[5], strlen(welcome[5])); 

  setColor(NORMAL);
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
  if (c_in == ESC) return ESC;  
  if (c_in == DEL || c_in == BACK) return DEL; // Backspace

  return c_in; // Regular character
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Game
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void display_lines()
{
  if ( !g.ln) return;
  // g.lri is the line that should appear in the middle of the screen.
  int startline = g.lri;
  scbp(0, 0);
  while (startline * line_spacing < g.sz_rows && startline < g.lsz) {
    write(ttyout, g.ln[startline]->ln, g.ln[startline]->sz);  
    scbp(0, ++startline * line_spacing);
  }
}

int run()
{
  // Initilize 
  p.start = clock();
  write(ttyout, CUR_CLEAR, strlen(CUR_CLEAR)); 
  display_lines();
  scbp(0, 0); 

  char c;
  while (1) {
    c = getInput();

    switch (c) {
      case EXIT:
        return 0;
        break;
      case ESC:
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
          // The input is not correct, display red letter
          ++p.error;
          p.input[g.lri][g.lci] = c;

          if (g.ln[g.lri]->ln[g.lci] == ' ')
            setColor(REDONRED);
          else
            setColor(REDONBLACK);

          write(ttyout, &g.ln[g.lri]->ln[g.lci], 1);
          setColor(NORMAL);
        }

        if (g.lci < g.sz_cols) ++g.lci;
    }; 
  }

  return c;
}

int start_game()
{
  display_welcome();

  char c;
  bool exit = false;
  bool mainmenu = true; // Are we displaying the main menu
  do {
    c = getInput();

    switch(c) {
     case EXIT:
        exit = true;
        break;
      case ESC:
        if (!mainmenu) {
          display_welcome();
          mainmenu = true;
        }
        break;
      case 'M':
      case 'm':
        display_game_settings();
        mainmenu = false;
        break;
      case 'Q':
      case 'q':
        exit = true;
        clean_up();
        break;
      case 'S':
      case 's':
        run();
        display_welcome();
        break;
    };
  } while ( !exit);

  return 0;
}
  
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Setup
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct termios orig_term;

void clean_up()
{
  write(ttyout, "\033[1000B", 7);
  write(ttyout, "\033[1000D", 7);
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
   * Turn off strip char (8 bit characters) 
   * Catch Ctrl-C */
  term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | ISIG);
  /* Turn off echo, automattically writing to screen.
   * Turn off canonical mode. Read bytes instead of lines. 
   * Turn off Ctrl-Q and Ctrl-S, legacy terminal modes. */
  term.c_lflag &= ~(ECHO | ICANON | IXON); 
  /* Do not post process NL to CR+NL */
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

  // The left,top, right and bottom bounds of the game board
  g.st_cols  = g.tot_cols - (middlecols + (g.sz_cols / 2));
  g.end_cols = g.tot_cols - (middlecols - (g.sz_cols / 2));
  g.st_rows  = g.tot_rows - (middlerows + (g.sz_rows / 2));
  g.end_rows = g.tot_rows - (middlerows - (g.sz_rows / 2));

  // Memory 
  g.ln = NULL;
  g.lsz = 0;
  g.lri = 0;
  g.lci = 0;
  g.fn = NULL;

  // A general string for curser movement
  cur_string = malloc(cur_string_sz * sizeof(char));
  if ( !cur_string) return -1;
  memset(cur_string, '\0', cur_string_sz);
  
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

  // Set up the status line
  sl.ln = malloc(g.sz_cols * sizeof(char));
  if ( !ln) return -1;
  sl.sz = g.sz_cols;
  sl.st_cols = g.st_cols;

  // Set the row of the status line
  if (g.tot_rows - g.end_rows < 2) sl.st_rows = g.end_rows + 1;
  else sl.st_rows = (g.tot_rows - g.end_rows) / 2;


struct StatusLine
{
  char* ln;         /**< This should be g.sz_cols in size */
  int sz;           /**< Size of ln */

  int st_cols;      /**< Column status line starts on */
  int st_rows;      /**< Row of the status line */
  int tm;           /**< X position of the p.start */
  int correct;      /**< X posistion of the p.correct */
  int error;        /**< X position of p.error */
  int words;        /**< X position of p.words */
  float score;      /**< X position of p.score */
} sl;

  raw_mode();
  setColor(NORMAL);
  write(ttyout, CUR_CLEAR, strlen(CUR_CLEAR));

  return 0;
}

int main()
{
  signal(SIGINT, clean_up);
  init_game();

  // ************************************************
  // TODO: Move this to a load file function. 
  char* fn = "data/alice.txt";
  g.fn = malloc(strlen(fn) * sizeof(char) + 1);
  strcpy(g.fn, fn);

  // Set up the lines struct
  g.ln = malloc(max_lines * sizeof(struct Line *));
  memset(g.ln, '\0', max_lines);

  g.lsz = read_file(g.fn, g.ln, max_lines, g.sz_cols);
  if (g.lsz <= 0) exit(EXIT_FAILURE);
  // ************************************************

  start_game();
  
  // Set the cursor at the bottom left of the screen on exit.
  // Close all file descripters for valgrind error summary.
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
  
  return 0;
} 
