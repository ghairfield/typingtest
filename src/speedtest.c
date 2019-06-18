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
  // TODO: Need to create the loop to use a line instread of letters.
  write(ttyout, "\033[2J", 4); // Clear the screen, move home.
  write(ttyout, "\033[H", 3);  
  for (int y = 0; y < d.rows - 1; ++y) {
    for (int x = 0; x < d.cols; ++x) 
      //write(ttyout, , 1);

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
  int linesize = 0; //strlen(words[0]);
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
            linesize = 0;//strlen(words[p.y]);
            p.x = linesize;
            --p.y;
            write(ttyout, "\033[A", 3);
            sprintf(position, "\033[%dC", p.x);
            write(ttyout, position, strlen(position));
            //TODO: write(ttyout, &words[p.y][p.x], 1);
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
        /* TODO:
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
        */
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

/*
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
  for (int a = 0; a < MAX_LINES; ++a) 
    *(words + a) = NULL;
  
   * Input sanatization
   * 1) Remove all non-print letters
   * 2) No more than 1 space between words
   *
   * Getting the line
   * 1. Get a line that is size of cols
   * 2. Work backwards and find the last word
   *    a) The last word occurs at the end of the line
   *    b) The last word is broken between 2 lines
   *      1) Back up to space, remove rest
   *    c) There is a space between the first column and first word?
   e 3. Cut the line off at the last space before a 1/2 word
   * 4. Copy line to program 
   * 5. repeat
  
  char* line = malloc(d.cols * sizeof(char));
  memset(line, '\0', d.cols);
  int index = 0;
  int inword = false;
  char c;
  for (int a = 0; a < MAX_LINES; ++a) {
    //Get the line
    while ((c = fgetc(f)) != EOF && index < d.cols) {
      // We have a print character, check word
      if (isprint((char)c)) {

        if ((char)c == ' ' && inword) {
          // At the end of a word.  
          inword = false;
          line[index++] = (char)c;
        }
        else if ((char)c == ' ' && !inword) {
          // Not in a word, and an extra space
          continue;
        }
        else {
          if ( !inword) inword = true;
          line[index++] = (char)c;
        }
      }
      else if ((char)c == '\n' && inword) {
        // Removing new lines from input file
        line[index++] = ' ';
      }
    }
      // Process line
      --index;
      int end = d.cols - 1;
      if (inword) {
        while (line[index--] != ' ') 
          ;
        
        words[a] = malloc(d.cols * sizeof(char));
        memset(words[a], '\0', d.cols);
        strncpy(words[a], line, index);
      } 
      printf("%s", words[a]);
      index = 0;
      memset(line, '\0', d.cols);
      fseek(f, end - index, SEEK_CUR);
  }

  fclose(f);
  return 0;
} */

/* int main()
{
  //raw_mode();
  struct winsize ws;
  ioctl(ttyout, TIOCGWINSZ, &ws);
  d.rows = ws.ws_row;
  d.cols = ws.ws_col;

  if (read_file() == -1)  return -1;
  //screen();
  //run();

  return 0;
} */

int main()
{
  struct Line** lines;
  int size = 100;
  lines = malloc(size * sizeof(struct Line *));
  for (int i = 0; i < size; ++i)
    lines[i] = NULL;

  int cols = 80;
  int ln = read_file("data/test.txt", &lines, size, cols);
  if (ln < 0) exit(EXIT_FAILURE);
  for (int i = 0; i < ln; ++i)
    printf("%s\n", lines[i]->ln);
  printf("We got %d lines back.\n", ln);

  destroy_lines(&lines, ln);
  free(lines);

  return 0;
} 
