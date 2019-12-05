#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "screen.h"

#define true 1
#define false 0
#define ttyin STDIN_FILENO
#define ttyout STDOUT_FILENO



struct Display
{
  /* Size of the terminal */
  int maxCols, maxRows;
} d;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Helpers
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void moveCursorTo(int y, int x)
{
  char pos[20] = { '\0' };
  sprintf(pos, "\033[%d;%dH", y, x);
  write(ttyout, pos, strlen(pos));
}

void getMaxYX(int *y, int *x)
{
  extern struct Display d;
  *x = d.maxCols;
  *y = d.maxRows;
}

uint16_t* setString(const char * s, size_t sz, enum COLORS c)
{
  uint16_t *str = malloc(sizeof(uint16_t) * (sz + 1));
  memset(str, '\0', sz + 1);
  if (!str) {
    // TODO Some error code or something
    return NULL;
  }

  for (size_t i = 0; i < sz; ++i) {
    str[i] |= c | s[i]; 
  } 

  return str;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Write
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void writeColorProfile(enum COLORS c)
{
  switch (c)
  {
    /* Black foregound on X */
    case COLOR_BLK_ON_RED: write(ttyout, "\033[30;41m", 8); break;
    case COLOR_BLK_ON_GRN: write(ttyout, "\033[30;42m", 8); break;
    case COLOR_BLK_ON_YLW: write(ttyout, "\033[30;43m", 8); break;
    case COLOR_BLK_ON_BLU: write(ttyout, "\033[30;44m", 8); break;
    case COLOR_BLK_ON_MAG: write(ttyout, "\033[30;45m", 8); break;
    case COLOR_BLK_ON_CYN: write(ttyout, "\033[30;46m", 8); break;
    case COLOR_BLK_ON_WHT: write(ttyout, "\033[30;47m", 8); break;
    /* Red foregound on X */
    case COLOR_RED_ON_BLK: write(ttyout, "\033[31;40m", 8); break;
    case COLOR_RED_ON_GRN: write(ttyout, "\033[31;42m", 8); break;
    case COLOR_RED_ON_YLW: write(ttyout, "\033[31;43m", 8); break;
    case COLOR_RED_ON_BLU: write(ttyout, "\033[31;44m", 8); break;
    case COLOR_RED_ON_MAG: write(ttyout, "\033[31;45m", 8); break;
    case COLOR_RED_ON_CYN: write(ttyout, "\033[31;46m", 8); break;
    case COLOR_RED_ON_WHT: write(ttyout, "\033[31;47m", 8); break;
    /* Green foregound on X */
    case COLOR_GRN_ON_BLK: write(ttyout, "\033[32;40m", 8); break;
    case COLOR_GRN_ON_RED: write(ttyout, "\033[32;40m", 8); break;
    case COLOR_GRN_ON_YLW: write(ttyout, "\033[32;40m", 8); break;
    case COLOR_GRN_ON_BLU: write(ttyout, "\033[32;40m", 8); break;
    case COLOR_GRN_ON_MAG: write(ttyout, "\033[32;40m", 8); break;
    case COLOR_GRN_ON_CYN: write(ttyout, "\033[32;40m", 8); break;
    case COLOR_GRN_ON_WHT: write(ttyout, "\033[32;40m", 8); break;
    /* Yellow foregound on X */
    case COLOR_YLW_ON_BLK: write(ttyout, "\033[33;40m", 8); break;
    case COLOR_YLW_ON_RED: write(ttyout, "\033[33;40m", 8); break;
    case COLOR_YLW_ON_GRN: write(ttyout, "\033[33;40m", 8); break;
    case COLOR_YLW_ON_BLU: write(ttyout, "\033[33;40m", 8); break;
    case COLOR_YLW_ON_MAG: write(ttyout, "\033[33;40m", 8); break;
    case COLOR_YLW_ON_CYN: write(ttyout, "\033[33;40m", 8); break;
    case COLOR_YLW_ON_WHT: write(ttyout, "\033[33;40m", 8); break;
    /* Blue foregound on X */ 
    case COLOR_BLU_ON_BLK: write(ttyout, "\033[34;40m", 8); break;
    case COLOR_BLU_ON_RED: write(ttyout, "\033[34;40m", 8); break;
    case COLOR_BLU_ON_GRN: write(ttyout, "\033[34;40m", 8); break;
    case COLOR_BLU_ON_YLW: write(ttyout, "\033[34;40m", 8); break;
    case COLOR_BLU_ON_MAG: write(ttyout, "\033[34;40m", 8); break;
    case COLOR_BLU_ON_CYN: write(ttyout, "\033[34;40m", 8); break;
    case COLOR_BLU_ON_WHT: write(ttyout, "\033[34;40m", 8); break;
    /* Magintia foreground on X */
    case COLOR_MAG_ON_BLK: write(ttyout, "\033[35;40m", 8); break;
    case COLOR_MAG_ON_RED: write(ttyout, "\033[35;40m", 8); break;
    case COLOR_MAG_ON_GRN: write(ttyout, "\033[35;40m", 8); break;
    case COLOR_MAG_ON_YLW: write(ttyout, "\033[35;40m", 8); break;
    case COLOR_MAG_ON_BLU: write(ttyout, "\033[35;40m", 8); break;
    case COLOR_MAG_ON_CYN: write(ttyout, "\033[35;40m", 8); break;
    case COLOR_MAG_ON_WHT: write(ttyout, "\033[35;40m", 8); break;
    /* Cyan foreground on X */
    case COLOR_CYN_ON_BLK: write(ttyout, "\033[36;40m", 8); break;
    case COLOR_CYN_ON_RED: write(ttyout, "\033[36;40m", 8); break;
    case COLOR_CYN_ON_GRN: write(ttyout, "\033[36;40m", 8); break;
    case COLOR_CYN_ON_YLW: write(ttyout, "\033[36;40m", 8); break;
    case COLOR_CYN_ON_BLU: write(ttyout, "\033[36;40m", 8); break;
    case COLOR_CYN_ON_MAG: write(ttyout, "\033[36;40m", 8); break;
    case COLOR_CYN_ON_WHT: write(ttyout, "\033[36;40m", 8); break;
    /* White forground on X */
    case COLOR_WHT_ON_BLK: write(ttyout, "\033[37;40m", 8); break;
    case COLOR_WHT_ON_RED: write(ttyout, "\033[37;41m", 8); break;
    case COLOR_WHT_ON_GRN: write(ttyout, "\033[37;42m", 8); break;
    case COLOR_WHT_ON_YLW: write(ttyout, "\033[37;43m", 8); break;
    case COLOR_WHT_ON_BLU: write(ttyout, "\033[37;44m", 8); break;
    case COLOR_WHT_ON_MAG: write(ttyout, "\033[37;45m", 8); break;
    case COLOR_WHT_ON_CYN: write(ttyout, "\033[37;46m", 8); break;
  }; 
} 

void clearScreen()
{
  write(ttyout, "\033[2J", 4);
} 

int writeCharacter(char content)
{
  return write(ttyout, &content, 1);
} 

int writeString(const char* content, unsigned int size)
{
  int a = 0;
  for (unsigned int i = 0; i < size; ++i) 
  {
    a += writeCharacter(content[i]);
  } 
 
  return a; 
} 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Input handling
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

char getInput()
{
  char c_in = -1;
  int bytes;

  /* We ignore most input that doesn't apply to this program. */
  bytes = read(ttyin, &c_in, 1);
  if (bytes == -1) return EXIT; /* Failure */

  if (c_in == CTRL_C || c_in == CTRL_Q) return EXIT; /* User quit */

  if (c_in == DEL || c_in == BACK) return DEL; /* Backspace */

  return c_in; /* Regular character */
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Setup
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct termios orig_term;

void screenDestroy()
{
  /* Flush the terminal upon exit, and reset the original terminal settings. */
  clearScreen();
  write(ttyout, "\033[37;40m", 8);
  tcsetattr(ttyin, TCSAFLUSH, &orig_term);
}

static void rawMode()
{
  /*
   * We can turn on raw mode with
   *      int cfmakeraw(struct termios *termios-p)
   * is equal to
   *  termios-p->c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
   *  termios-p->coiflag &= ~OPOST;
   *  termios-p->c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
   *  termios-p->c_cflag &= ~(CSIZE|PARENB);
   *  termios-p->c_flag |= CS8;
   */
  struct winsize ws;
  ioctl(ttyout, TIOCGWINSZ, &ws);
  d.maxRows = ws.ws_row;
  d.maxCols = ws.ws_col;

  if ( !isatty(ttyin)) {
    fprintf(stderr, "This is not a tty.\n");
    exit(EXIT_FAILURE);
  }
  /* Copy the original terminal settings so they can be set upon exit */
  tcgetattr(ttyin, &orig_term); 
  atexit(screenDestroy); 

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
  
  term.c_cc[VMIN] = 0;  /* Read a mimium of 0 bytes */
  term.c_cc[VTIME] = 0; /* No wait for input */
  tcsetattr(ttyin, TCSANOW, &term);
}

void screenInit()
{
  rawMode();
}
