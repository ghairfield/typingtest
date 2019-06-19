/**
 * Greg Hairfield
 * A speed typing test for fun and profit. 
 * May 27, 2019
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
  GOOD,
  WARN,
  CRISIS,
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

const float game_width_factor  = 0.80f;
const float game_height_factor = 0.80f;
const int   max_lines          = 1000;
const int   line_spacing       = 2;
const char* game_settings[] = { \
    "Game Settings -",    \
    "Columns      : %d",  \
    "Rows         : %d",  \
    "Game Board Size  ",  \
    "        Cols : %d",  \
    "        Rows : %d",  \
    "Game Cols        ",  \
    "     Start   : %d",  \
    "     End     : %d",  \
    "Game Rows        ",  \
    "     Start   : %d",  \
    "     End     : %d",  \
    "Game Speed   : %.2f",\
    NULL };

struct Game
{
  int tot_cols;   /**< Total columns */
  int tot_rows;   /**< Total rows */
  int sz_cols;  /**< Size of game columns window */
  int sz_rows;
  int st_cols;
  int end_cols;
  int st_rows;
  int end_rows;

  struct Line** ln;
  int lsz;          /**< total lines that exist in ln */
  int lri;          /**< line row index */
  int lci;          /**< line column index  */

  char* fn;         /**< Current file name */
} g;

struct Player
{
  char** input;
  int row, col;

  int correct;
  int error; 
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
    case GOOD:
      write(ttyout, "\033[32;40m", 8);
      break;
    case WARN:
      write(ttyout, "\033[33;40m", 8);
      break;
    case CRISIS:
      write(ttyout, "\033[31;40m", 8);
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

  write(ttyout, CUR_HOME, strlen(CUR_HOME));
  sprintf(cur_string, CUR_DOWN, y + g.st_rows);
  write(ttyout, cur_string, strlen(cur_string)); 
  clear_cursor_string();
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
  sprintf(string, game_settings[6]);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[7], g.st_cols);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[8], g.end_cols);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[9]);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[10], g.st_rows);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty++);
  sprintf(string, game_settings[11], g.end_rows);
  write(ttyout, string, strlen(string)); 

  scbp(startx, starty);
  sprintf(cur_string, game_settings[12], 1.23f);
  write(ttyout, cur_string, strlen(string)); 
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

  write(ttyout, CUR_CLEAR, strlen(CUR_CLEAR));

  int starty = 0;
  int startl = g.lri;

  scbp(0, 0);
  while (starty + g.st_rows < g.end_rows && startl < g.lsz) {
    write(ttyout, g.ln[startl]->ln, g.ln[startl]->sz);  
    starty += line_spacing;
    scbp(0, starty);
    ++startl;
  }
}

/*
int run()
{
  // Initilize the user info
  p.start   = clock();

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
            g.lci = g.ln[g.lri]->sz;
            // TODO NEED TO SET THE BOARD POSITION, NOT THE WINDOW POSITION
          }
        }
        else {
          // At some position in the line
          write(ttyout, "\033[D", 3);
          // TODO write(ttyout, &words[p.y][--p.x], 1);
          write(ttyout, "\033[D", 3);
        } 
        break;
      case CR:
        // TODO: Verify word
        if (p.y < d.rows) {
          write(ttyout, "\033[B", 3);
          write(ttyout, "\033[1000D", 7);
          ++p.y;
          p.x = 0;
        }
        break;
      default:
        // TODO:
        if (c == words[p.y][p.x]) {
          setColor(GOOD);
          write(ttyout, &words[p.y][p.x], 1);
          setColor(NORMAL);
        }
        else {
          setColor(BAD_INPUT);
          write(ttyout, &words[p.y][p.x], 1);
          setColor(NORMAL);
        }

        if (p.x < d.cols) ++p.x;
    }; 
  }

  return c;
} */

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

  cur_string = malloc(cur_string_sz * sizeof(char));
  if ( !cur_string) return -1;
  memset(cur_string, '\0', 20);
  
  // Set up the player
  p.correct = 0;
  p.error   = 0;
  p.score   = 0.0f;
  p.row     = 0;
  p.col     = 0;

  p.input = malloc(g.sz_rows * sizeof(char *));
  if ( !p.input) return -1;
  for (int i = 0; i < g.sz_rows; ++i) {
    p.input[i] = malloc(g.sz_cols * sizeof(char));
    if ( !p.input[i]) return -1;
    memset(p.input[i], '\0', g.sz_cols);
  }

  raw_mode();

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

  display_lines();

  clean_up();
  // Close all file descripters for valgrind error summary.
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
  return 0;
} 
