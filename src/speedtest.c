/**
 * Written by Greg Hairfield
 * A speed typing test. 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Global def's
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
enum KEYMAP {
  CTRL_C = 3,   // ETX
  BACK   = 8,   // Backspace
  CR     = 13,  // Enter
  CTRL_Q = 17,  // Quit
  ESC    = 27,  // Escape 
  DEL    = 127  // Delete 
};

struct Row
{
  char* str;  // str = contents of the row
  int num;    // num = current line number
};

struct Screen 
{
  int szx, szy;     // Size of screen
  char* sl;         // Status line
  struct Row* rows; // y - STATUS_LINE_SIZE
  int curx;         // Position of the cursor on the status line
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Global's
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
static struct Screen scr;
static const int ttyfd = STDIN_FILENO;
static const int STATUS_LINE_SIZE = 1;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * IO stuff
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int readChar()
{
  int c;
  int count = read(ttyfd, &c, 1); 
  if (count < 0) return -1;
  if (count == 0) return 0;
  return c;
}

// Write the game words to the screen
void writeScreen()
{
  write(STDIN_FILENO, "\x1b[H", 3);
  for (int a = 0; a < scr.szy - STATUS_LINE_SIZE; ++a) {
    write(STDIN_FILENO, scr.rows[a].str, scr.szx);      
  }

  write(STDIN_FILENO, scr.sl, scr.szx);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Game stuff.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int run()
{
  float speed = 1.0;
  struct timeval start;
  gettimeofday(&start, NULL);
  int running = 1;
  int ch;

  while (running) {
    ch = readChar();
  
    switch (ch) {
      case CTRL_C:
      case CTRL_Q: running = 0; break; // Quit the game
      case CR: // Enter the string in the status line
      case ESC: break; // Currently not handling Escape
      case BACK:
      case DEL: if (scr.curx > 0) scr.sl[--scr.curx] = ' '; break;
      default:
        scr.sl[scr.curx++] = ch; 
    };

    writeScreen();
  }

  return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Setup stuff.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*  Code for turning on raw mode. Not mine, taken from 
 *  http://www.cs.uleth.ca/~holzmann/C/system/ttyraw.c
 */
static struct termios orig_termios;

/* put terminal in raw mode - see termio(7I) for modes */
int tty_raw(void)
{
  if ( !isatty(ttyfd)) return -1;

  struct termios raw;
  raw = orig_termios;  /* copy original and then modify below */
  if (tcgetattr(ttyfd,&orig_termios) < 0) {
    fprintf(stderr, "Could not get original terminal info.\n");
    return -1;
  }

  /* input modes - clear indicated ones giving: no break, no CR to NL, 
   no parity check, no strip char, no start/stop output (sic) control */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  /* output modes - clear giving: no post processing such as NL to CR+NL */
  raw.c_oflag &= ~(OPOST);

  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);

  /* local modes - clear giving: echoing off, canonical off (no erase with 
     backspace, ^U,...),  no extended functions, no signal chars (^Z,^C) */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN); // | ISIG);

  /* control chars - set return condition: min number of bytes and timer */
  raw.c_cc[VMIN] = 0; raw.c_cc[VTIME] = 1;   /* After .1 set */

  /* put terminal in raw mode after flushing */
  if (tcsetattr(ttyfd,TCSAFLUSH,&raw) < 0) {
    fprintf(stderr, "can't set raw mode");
    return -1;
  }

  return 0;
}
int tty_reset(void)
{
  /* flush and reset */
  if (tcsetattr(ttyfd,TCSAFLUSH,&orig_termios) < 0) return -1;
  return 0;
}
/* exit handler for tty reset */
void tty_atexit(void)  /* NOTE: If the program terminates due to a signal   */
{                      /* this code will not run.  This is for exit()'s     */
   tty_reset();        /* only.  For resetting the terminal after a signal, */
}                      /* a signal handler which calls tty_reset is needed. */

/* reset tty - useful also for restoring the terminal when this process
   wishes to temporarily relinquish the tty

 * 
 * End of copy pasta 
 */


int getWinSize()
{
  struct winsize w;
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) < 0) {
    fprintf(stderr, "Could not get terminal size.\n");
    return -1;
  } 
  scr.szy = w.ws_row;
  scr.szx = w.ws_col;

  return 0;
}

int init_screen()
{
  if (getWinSize() == -1) return -1;
  scr.rows = malloc(scr.szy * sizeof(struct Row));
  if ( !scr.rows) return -1;

  for (int a = 0; a < scr.szy - STATUS_LINE_SIZE; ++a) {
    scr.rows[a].str = malloc(scr.szx * sizeof(char));
    if ( !scr.rows[a].str) {
      // TODO: Should clean up what was created.
      return -1;
    }
    memset(scr.rows[a].str, ' ', scr.szx);
    scr.rows[a].num = a + 1;
  }

  scr.sl = malloc(scr.szx * sizeof(char));
  if ( !scr.sl) return -1;
  memset(scr.sl, ' ', scr.szx);

  sprintf(scr.sl, "Test your typing skills. ^Q or ^C to exit.");
  return tty_raw();
}

void destroy()
{
  for (int a = 0; a < scr.szy; ++a) free(scr.rows[a].str);
  free(scr.rows);
}

int init()
{
  if (init_screen() == -1) return -1;
  if (tty_raw() == -1) {
    fprintf(stderr, "Could not set raw mode.\n");
    return -1;
  }
  return 0;
}

int main()
{
  if (init() == -1) goto FAIL;

  run();

  tty_reset();
  destroy();
  return 0;
FAIL:
  return -1;
}
