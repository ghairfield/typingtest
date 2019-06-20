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
  EXIT   = -1   // Exit the program gracefully.
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

// A temporary string to copy to
char* cur_string;
const int cur_string_sz = 100;

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
  "A terminal typing test.",
  "Written by Greg Hairfield",
  "Options:                  ",
  "ESC       Go back a screen",
  "CTRL-C/Q              Quit",
  "L              Load a file",
  "S               Start game",
  "M     Settings/Leader-board",
  "C     Conways Game of Life" };
  
// Status line helpers
const char* sl_score = \
      "Score: %d / %d    Accuracy: %.2f %    Word Count: %d    WPM: %.2f";
const char* sl_time  = "Time: %ld s";
// Forward declare
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

  bool running;     /**< Is the game running? */

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
  int sc_rows;      /**< Row of the status line */
  int sc_cols;      /**< Offset of sl_score */
  int tm_rows;      /**< Row of the timer */
  int tm_cols;      /**< Offset of the p.start */
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

/* Set cursor board position
 *  Sets the cursor relative to the board */
void scbp(int x, int y)
{
  assert(x >= 0 && y >= 0);
  
  sprintf(cur_string, "\033[%d;%df", y + g.st_rows, x + g.st_cols);
  write(ttyout, cur_string, strlen(cur_string));
}

/* Set cursor window position
 *  Sets the cursor relative to the window */
void scwp(int x, int y)
{
  assert(x > 0 && y > 0);

  sprintf(cur_string, "\033[%d;%df", y, x);
  write(ttyout, cur_string, strlen(cur_string));
}

void display_welcome() 
{
  // One ginormous function to write the intro screen
  write(ttyout, "\033[2J", 4);
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
  write(ttyout, "\033[4m", 4); // Underline
  write(ttyout, welcome[2], strlen(welcome[2])); 
  write(ttyout, "\033[0m", 4);
  setColor(CYANONBLACK);

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

  offx = middlex - (strlen(welcome[6]) / 2);
  middley += 1;
  scbp(offx, middley);
  write(ttyout, welcome[6], strlen(welcome[6])); 

  offx = middlex - (strlen(welcome[7]) / 2);
  middley += 1;
  scbp(offx, middley);
  write(ttyout, welcome[7], strlen(welcome[7])); 

  offx = middlex - (strlen(welcome[8]) / 2);
  middley += 1;
  scbp(offx, middley);
  write(ttyout, welcome[8], strlen(welcome[8])); 

  setColor(NORMAL);
}

void display_game_settings()
{
  // One ginormous function to write the game info
  int startx = 0;
  int starty = 0;
  char string[100] = { '\0' };

  // Move cursor to starting position and clear window
  write(ttyout, "\033[2J", 4); 
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
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Input handling
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* We ignore most input that doesn't apply to this program. */
char getInput()
{
  char c_in = '\0';
  int bytes;

  bytes = read(ttyin, &c_in, 1);
  if (bytes == -1) return EXIT; // Failure

  if (c_in == CTRL_C || c_in == CTRL_Q) return EXIT; // Return to main menu
  if (c_in == ESC) return ESC; // Returns to the main menu 
  if (c_in == DEL || c_in == BACK) return DEL; // Deletes a character

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
  // Set the cursor back to the start
  scbp(0, 0);
}

/* Display the score status line */
void update_sl_score()
{
  char str[100] = { '\0' };
  // Hide cursor and move to status line position
  write(ttyout, "\033[s", 3);
  sprintf(str, "\033[%d;%df", sl.sc_rows, sl.sc_cols);
  write(ttyout, str, strlen(str));
  setColor(MAGENTAONBLACK);
  
  // Get the current time and compare to start time.
  time_t now = time(NULL);
  long diff = (long)now - (long)p.start;
  float wpm = ((float)p.words / (float)diff) * 60.0f;
  sprintf(str, sl_score, p.error, p.correct, 
         100.0f - ((float)p.error / (float)p.correct) * 100.0, p.words, wpm);
  write(ttyout, str, sizeof(str));
  
  // Set everyting back to normal
  setColor(NORMAL);
  write(ttyout, "\033[u", 3);
}

/* Display the time part of the status line
 * This is updated differently than the score, and
 * would be good if this didn't rely on the read
 * blocking call.
 */
void update_sl_time()
{
  char str[100] = { '\0' };
  time_t now = time(NULL);

  // Get to the correct spot
  write(ttyout, "\033[s", 3);
  sprintf(str, "\033[%d;%df", sl.tm_rows, sl.tm_cols);
  write(ttyout, str, strlen(str));

  sprintf(str, sl_time, (long)now - (long)p.start);
  setColor(BLUEONBLACK);
  write(ttyout, str, strlen(str));

  // Set everything back to normal
  setColor(NORMAL);
  write(ttyout, "\033[u", 3);
}

/* Runs the typing test game. */
int run()
{
  // Initialize 
  write(ttyout, "\033[2J", 4);   // Clear the screen
  write(ttyout, "\033[?25h", 6); // Show cursor
  display_lines();
  p.start = time(NULL);
  time_t now, past = 0, scoreupdate = 0;
  bool inword = false;
  update_sl_score();
  update_sl_time(); 

  char c = '\0';
  while (1) {
    c = getInput();
    now = time(NULL);

    // Update the clock
    if (now - past > 1) {
      update_sl_time();
      past = now;
    }
    if (now - scoreupdate > 1) {
      update_sl_score();
      scoreupdate = now;
    }

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
            write(ttyout, "\033[D", 3);
          }
        }
        else {
          // At some position in the line
          --g.lci;
          write(ttyout, "\033[D", 3);
          write(ttyout, &g.ln[g.lri]->ln[g.lci], 1);
          write(ttyout, "\033[D", 3);
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

        if (inword) {
          ++p.words;
          inword = false;
        }
        break;
      case '\0':
        break;
      default:
        // Check that the entered text matches and add appropriate color
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
        if (inword && c == ' ') {
          ++p.words;
          inword = false;
        }
        else if ( !inword && isalnum(c)) {
          inword = true;
        }

        if (g.lci < g.sz_cols) { 
          ++g.lci;
          scoreupdate = time(NULL);
          update_sl_score();
        }
    }; 
  }

  write(ttyout, "\033[?25l", 6); // Hide cursor
  g.running = false;
  return c;
}

/* Basic game main menu. Hitting ESC brings the program here. */
int start_game()
{
  display_welcome();

  char c;
  bool exit = false;
  bool mainmenu = true; // Are we displaying the main menu
  do {
    c = getInput();

    switch(c) {
      // CTRL-C/CTRL-Q
      case EXIT:
        exit = true;
        break;
      // Escape  
      case ESC:
        if (!mainmenu) {
          // Is the main window showing?
          display_welcome();
          mainmenu = true;
        }
        break;
      // Load a typing file TODO
      case 'L':
      case 'l':
        break;
      // Show game internal info
      case 'M':
      case 'm':
        display_game_settings();
        mainmenu = false;
        break;
      // Quit the game
      case 'Q':
      case 'q':
        exit = true;
        break;
      // Run the game
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
  // Move cursor to bottom of screen on quit.
  write(ttyout, "\033[1000B", 7);
  write(ttyout, "\033[1000D", 7);
   /* Flush the terminal upon exit, and reset the original terminal settings. */
  tcsetattr(ttyin, TCSAFLUSH, &orig_term);

  // Clean up all the memory
  if (g.ln) {
    destroy_lines(g.ln, g.lsz);
    free(g.ln);
  }

  if (g.fn) free(g.fn);

  if (cur_string) free(cur_string);

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
  /* Turn off break (enter CBREAK)
   * Turn off CR to NL
   * Turn off parity check
   * Turn off strip char (8 bit characters) 
   * Catch Ctrl-C */
  term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | ISIG);
  /* Turn off echo, automatically writing to screen.
   * Turn off canonical mode. Read bytes instead of lines. 
   * Turn off Ctrl-Q and Ctrl-S, legacy terminal modes. */
  term.c_lflag &= ~(ECHO | ICANON | IXON); 
  /* Do not post process NL to CR+NL */
  term.c_oflag &= ~(OPOST);  
  
  term.c_cc[VMIN]  = 0;  // Read a minimum of 1 bytes and return
  term.c_cc[VTIME] = 10; // 1 sec wait to return
  tcsetattr(ttyin, TCSAFLUSH, &term);
}

int init_game()
{
  // Get the window size
  struct winsize ws;
  ioctl(ttyout, TIOCGWINSZ, &ws);
  g.tot_rows = ws.ws_row;
  g.tot_cols = ws.ws_col;

  // This game uses a 'board' where all of the text is written
  // to. Find the middle of the screen and use the game_*_factor
  // values to get the actual size of the board window. We can 
  // also align the board to be centered.
  int middlecols = g.tot_cols / 2;
  int middlerows = g.tot_rows / 2;

  // Set up the game offsets
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
  g.running = false;

  // A general string for cursor movement
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

  // Set the row of the status line at the top
  if (g.tot_rows - g.end_rows < 2) sl.sc_rows = g.end_rows + 1;
  else sl.sc_rows = (g.tot_rows - g.end_rows) / 2;
  sl.sc_cols = g.st_cols;
  sl.tm_rows = g.end_rows + 2; // TODO: This needs to change to address 20 rows
  sl.tm_cols = g.end_cols - 11;
  
  // Enter raw mode if all memory was allocated.
  raw_mode();
  setColor(NORMAL);
  write(ttyout, "\033[?25l", 6); // Hide curse

  return 0;
}

int main()
{
  // If we get CTRL-C unexpected, clean up memory.
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
  clean_up();
  
  // Set the cursor at the bottom left of the screen on exit.
  // Close all file descriptors for valgrind error summary.
  fclose(stdin);
  fclose(stdout);
  fclose(stderr);
  
  return 0;
} 
