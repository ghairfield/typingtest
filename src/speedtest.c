/**
 * Greg Hairfield
 * A speed typing test. 
 * May 27, 2019
 */
/*
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>

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

#define true 1
#define false 0
#define ttyin STDIN_FILENO
#define ttyout STDOUT_FILENO

struct Display
{
  int cols;
  int rows;
} d;

struct Player
{
  int correct;
  int error; 
  int x, y;
  double score;
  clock_t start; 
} p;

char** words = NULL;
int fsize = 0;
const int MAX_LINES = 500;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Helpers
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

void moveTo(int y, int x)
{
  char pos[20] = { '\0' };
  sprintf(pos, "\033[%d;%dH", y, x);
  write(ttyout, pos, strlen(pos));
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
int screen()
{
  write(ttyout, "\033[2J", 4); // Clear the screen, move home.
  write(ttyout, "\033[H", 3);  
  for (int y = 0; y < d.rows - 1; ++y) {
    for (int x = 0; x < d.cols; ++x) 
      write(ttyout, &words[y][x], 1);

    write(ttyout, "\033[B", 3);
    write(ttyout, "\033[1000D", 7);
  }

  write(ttyout, "\033[H", 3); // Move home
  return 0;
}

/* Assumes set up has been done and all is checked. */
int run()
{
  // Initilize the user info
  p.correct = 0;
  p.error   = 0;
  p.x       = 0;
  p.y       = 0;
  p.score   = 0;
  p.start   = clock();

  char c;
  int linesize = strlen(words[0]);
  while (1) {
    c = getInput();

    switch (c) {
      case EXIT:
        return 0;
        break;
      case DEL:
        if (p.x <= 0) {
          // We are at the left most position on the screen
          if (p.y > 0) {
            char position[12] = { '\0' };
            linesize = strlen(words[p.y]);
            p.x = linesize;
            --p.y;
            write(ttyout, "\033[A", 3);
            sprintf(position, "\033[%dC", p.x);
            write(ttyout, position, strlen(position));
            write(ttyout, &words[p.y][p.x], 1);
          }
        }
        else {
          // At some position in the line
          write(ttyout, "\033[D", 3);
          write(ttyout, &words[p.y][--p.x], 1);
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
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Setup
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
struct termios orig_term;

void clean_up()
{
  /* Flush the terminal upon exit, and reset the original terminal settings. */
  tcsetattr(ttyin, TCSAFLUSH, &orig_term);

  for (int a = 0; a < MAX_LINES; ++a) {
    if (words[a]) free(words[a]);
  }

  free(words);
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
  struct winsize ws;
  ioctl(ttyout, TIOCGWINSZ, &ws);
  d.rows = ws.ws_row;
  d.cols = ws.ws_col;

  if ( !isatty(ttyin)) {
    fprintf(stderr, "This is not a tty.\n");
    exit(EXIT_FAILURE);
  }
  // Copy the original terminal settings so they can be set upon exit
  tcgetattr(ttyin, &orig_term); 
  atexit(clean_up); // declared in stdlib.h       

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

int read_file()
{
  FILE* f = fopen("data/test.txt", "r");
  if ( !f) {
    perror("Could not open file.");
    return -1;
  }

  words = malloc(MAX_LINES * sizeof(char*));
  if ( !words) {
   perror("Could not allocate memory.");
   return -1;
  }
  for (int a = 0; a < MAX_LINES; ++a) *(words + a) = NULL;

  int c;
  int index;
  for (int a = 0; a < MAX_LINES; ++a) {
    *(words + a) = malloc(d.cols * sizeof(char));
    memset(words[a], ' ', d.cols);
    index = 0;
    while ((c = fgetc(f)) != EOF && index < d.cols) {
      if (isprint((char)c)) {
        words[a][index++] = (char)c;
      } else {
        ++index;
      }
    }
  }

  fclose(f);
  return 0;
}

int main()
{
  raw_mode();
  if (read_file() == -1)  return -1;
  screen();
  run();

  return 0;
}
