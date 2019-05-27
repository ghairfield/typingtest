/**
 * Greg Hairfield
 * A speed typing test. 
 * May 6, 2019
 */
/*
#include <string.h>
#include <sys/ioctl.h>
*/
#include <stdio.h>
//#include <ctype.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

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

struct Player
{
  int correct;
  int error; 
  double score;
  clock_t start; 
} player;

char* words = NULL;
const int MAX_WORDS = 300;

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
/* Assumes set up has been done and all is checked. */
int run()
{
  // Initilize the user info
  player.correct = 0;
  player.error   = 0;
  player.score   = 0;
  player.start   = clock();

  /* Where are we????
   *
   * The single word display is working well, and I think it is bug free.
   * Next up, displaying words. First we should just display them on the
   * line above. Then we need to get a bigger list of words. Then move on
   * to the display moving. 
   */
  char c;
  int wordindex = 0;
  char word[] = { "something" };

  while (1) {
    c = getInput();

    switch (c) {
      case EXIT:
        return 0;
        break;
      case DEL:
        write(ttyout, "\033[D", 3);
        c = ' ';
        write(ttyout, &c, 1);
        write(ttyout, "\033[D", 3);
        (wordindex <= 0) ? wordindex = 0 : --wordindex;
        break;
      case CR:
        // TODO: Verify word
        write(ttyout, "\033[1000D", 7);
        write(ttyout, "\33[K", 3);
        wordindex = 0;
        break;
      default:
        if (c == word[wordindex]) {
          setColor(GOOD);
          write(ttyout, &c, 1);
          setColor(NORMAL);
        }
        else {
          setColor(BAD_INPUT);
          write(ttyout, &c, 1);
          setColor(NORMAL);
        }
        ++wordindex;
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

  if (words) free(words);
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
  atexit(clean_up); // declared in stdlib.h       

  struct termios term = orig_term;
  /* Trun off break (enter CBREAK)
   * Trun off CR to NL
   * Turn off parity check
   * Turn off strip char (8 bit characters)
   */
  term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | ISIG);
  /* Turn off echo, automattically writing to screen.
   * Turn off canonical mode. Read bytes instead of lines. 
   * Turn off Ctrl-Q and Ctrl-S, legacy terminal modes. */
  term.c_lflag &= ~(ECHO | ICANON | IXON); 
  /* Do not ost process NL to CR+NL */
  term.c_oflag &= ~(OPOST);  
  /*
   * Minimum bytes to read is 1, and no timer.
   */
  term.c_cc[VMIN] = 1;
  term.c_cc[VTIME] = 0;
  tcsetattr(ttyin, TCSAFLUSH, &term);
}

int read_file()
{
  FILE* f = fopen("data/11-0.txt", "r");
  if ( !f) return -1;

  int num_char = (7 * MAX_WORDS); // Assume on average each word is 7 letters
  words = malloc(num_char * sizeof(char));
  if ( !words) return -1;

  memset(words, '\0', num_char);

  int c;
  int count = 0;
  int index = 0;
  while ((c = fgetc(f)) != EOF && count < MAX_WORDS && index < num_char - 1) {
    if ((char)c == ' ') ++count;
    words[index++] = c;
  }

  fclose(f);
  return 0;
}

int main()
{
  if (raw_mode() == -1) return -1;
  if (read_file() == -1) return -1;

  run();

  return 0;
}
