#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "screen.h"

#define true 1
#define false 0

#define ttyin STDIN_FILENO
#define ttyout STDOUT_FILENO

void clearBuffer();

struct FrameBuffer {
  enum COLORS co; /* The color of the selected position */
  char ch;        /* Character that goes there */
  char dirty;     /* Has the location been changed since last write */
};

struct Display {
  struct FrameBuffer** fb;  /* Frame buffer */
  enum COLORS curColor;     /* Current color to write */
  int writeX, writeY;       /* Current location in frame buffer 
                               Since the screen is (1,1) to (X,Y) we 
                               need to decrement each by 1 to write to
                               the framebuffer.*/
  int maxCols, maxRows;     /* Size of the terminal, X, Y */
  char wrap;
};

struct Display d;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Helpers
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void moveCursorTo(int y, int x)
{
  if (y >= 0 && y < d.maxRows && x >= 0 && x < d.maxCols) {
    d.writeX = x;
    d.writeY = y;
  }
}

void getMaxYX(int *y, int *x)
{
  *x = d.maxCols;
  *y = d.maxRows;
}

void setColor(enum COLORS c)
{
  d.curColor = c;
} 

enum COLORS getCurrentColor()
{
  return d.curColor;
}

void setCursorOn()
{
  write(STDOUT_FILENO, "\033[?25h", 6);
} 

void setCursorOff()
{
  write(STDOUT_FILENO, "\033[?25l", 6);
} 

void setTextWrapOn() 
{
  d.wrap = true;
}

void setTextWrapOff()
{
  d.wrap = false;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Write
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void writeColorProfile(enum COLORS c)
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
    case COLOR_NONE: break; /* Do nothing, supress compilier warning */
  }; 
} 

void clearScreen()
{
  write(ttyout, "\033[2J", 4);
  clearBuffer();
} 

int writeCharacter(char content)
{
  if (d.writeX >= 0 && d.writeX < d.maxCols && 
      d.writeY >= 0 && d.writeY < d.maxRows) 
  {
    d.fb[d.writeY][d.writeX].ch = content;
    d.fb[d.writeY][d.writeX].co = d.curColor;
    d.fb[d.writeY][d.writeX].dirty = true;

    // Overflow on or off
    if (d.wrap && d.writeX >= d.maxRows) {
      d.writeX = 0; 
      ++d.writeY;
    }
    else 
      ++d.writeX;
    return 1;
  }
  return 0;
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

void writeScreen()
{
  char w[20] = { '\0' };
  enum COLORS curC = d.curColor;
  writeColorProfile(curC);

  for (int y = 0; y < d.maxRows; ++y) {
    for (int x = 0; x < d.maxCols; ++x) {
      if (d.fb[y][x].dirty) {
        sprintf(w, "\033[%d;%dH", y + 1, x + 1);
        write(ttyout, w, strlen(w));  // Move the cursor to position
        
        if (curC != d.fb[y][x].co) {
          curC = d.fb[y][x].co;
          writeColorProfile(curC);
        }

        d.fb[y][x].dirty = false;
        write(ttyout, &d.fb[y][x].ch, 1);
        memset(w, '\0', 20);
      } 
    } 
  }
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
 * Frame Buffer setup
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Used with clearScreen() */
void clearBuffer()
{
  for (int y = 0; y < d.maxRows; ++y) {
    for (int x = 0; x < d.maxCols; ++x) {
      d.fb[y][x].ch = ' ';
      d.fb[y][x].co = d.curColor;
      d.fb[y][x].dirty = false;
    }
  } 
  /* We move to the home position as does the VT100 clear screen */
  d.writeX = 0;
  d.writeY = 0;
} 

void bufferInit()
{
  int i;

  d.fb = malloc(sizeof(struct FrameBuffer *) * d.maxRows);
  if ( !d.fb) {
    fprintf(stderr, "Could not allocated memory for frame buffer column.\n");
    return;
  }

  for (i = 0; i < d.maxRows; ++i) {
    d.fb[i] = malloc(sizeof(struct FrameBuffer) * d.maxCols);
    if ( !d.fb[i]) {
      fprintf(stderr, "Could not allocated memory for frame buffer row.\n");
      return;
    }
  } 

} 

void bufferDestroy()
{
  if (d.fb) {
    for (int i = 0; i < d.maxRows; ++i) free(d.fb[i]);

    free(d.fb);   
    d.fb = NULL;
  } 
} 

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Setup
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

struct termios orig_term;

void screenDestroy()
{
  /* Flush the terminal upon exit, and reset the original terminal settings. */
  bufferDestroy();
  write(ttyout, "\033[2J", 4);
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
  d.fb = NULL;

  if ( !isatty(ttyin)) {
    fprintf(stderr, "This is not a tty.\n");
    exit(EXIT_FAILURE);
  }
  /* Copy the original terminal settings so they can be set upon exit */
  tcgetattr(ttyin, &orig_term); 
//  atexit(screenDestroy); 

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
  bufferInit();
  clearBuffer();

  d.curColor = COLOR_WHT_ON_BLK;
  d.writeX = 0;
  d.writeY = 0;
  d.wrap   = false;
}
