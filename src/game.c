#include "game.h"
#include "words.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>
#include <getopt.h>

typedef unsigned char u_char;
#include <bsd/stdlib.h> // random

#define true 1
#define false 0

typedef unsigned int uint;

/******************************************************************************
 * Timing functions and vars.
 *****************************************************************************/

static const float MSEC_ = 1000000.0f;
static struct timeval tv_begin, tv_user;

static inline void timerInit()
{
  gettimeofday(&tv_begin, NULL);
  gettimeofday(&tv_user, NULL);
} 

static inline void timerReset()
{
  gettimeofday(&tv_user, NULL);
}

double timerDiff()
{
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (tv_now.tv_sec - tv_user.tv_sec) + 
         ((tv_now.tv_usec - tv_user.tv_usec) / MSEC_);
}

double timerTotalTime()
{
  struct timeval tv_now;
  gettimeofday(&tv_now, NULL);
  return (tv_now.tv_sec - tv_begin.tv_sec) + 
         ((tv_now.tv_usec - tv_begin.tv_usec) / MSEC_);
}

/******************************************************************************
 * User Interface
 *****************************************************************************/

struct UserInterface
{
  int screenX, screenY;
  /*
   * We store the bounds of the UI here. They currently
   * are constant values. If the screen size changes, 
   * the UI won't scale with it. Put on the TODO list.
   */
  int boardL, boardR, boardT, boardB;
  enum COLORS boardC; // Not implemented 

  int errorY, errorX;
  enum COLORS errorC; // Not implemented

  int scoreY, scoreX;
  enum COLORS scoreC; // Not implemented

  int timeY,  timeX;
  enum COLORS timeC;
  
  /* Input area */
  int inputX, inputY; /* Location of input area */
  int inputS;         /* Size of input area */
  enum COLORS inputC; /* Color of area */

  /* Debug on */
  bool debug;
  int debugTickX, debugTickY;
  enum COLORS debugTickC;
}; 

struct UserInterface UI = {0, 0, 0, 0, 0, 0, COLOR_NONE, 0, 0, COLOR_NONE, 0, 0,
    COLOR_NONE, 0, 0, COLOR_NONE, 0, 0, 0, COLOR_NONE, false, 0, 0, COLOR_NONE };


static void setScoreSmall()
{
  // Set the score on the bottom 
  // We have 80 character positions to work with.
  // We are going to give the score 5 spaces
  char* score = "Score:";
  // Error is going to get 5 spaces
  char* error = "Error:";
  // 5 spaces, MM:SS
  char* time  = "Time :";

  UI.errorY = UI.scoreY = UI.timeY = UI.boardB;
  --UI.boardB; 

  // Set bottom 
  setColor(COLOR_MAG_ON_BLK);
  moveCursorTo(UI.boardB, UI.boardL + 1);
  for (int i = UI.boardL + 1; i <= UI.boardR; ++i) {
    writeCharacter('_');
  }

  int x;
  UI.timeC = COLOR_WHT_ON_BLK;
  x = UI.boardR - 1 - 6 - strlen(time);  
  UI.timeX = x + 1 + strlen(time);  
  moveCursorTo(UI.timeY, x);
  setColor(UI.timeC);
  for (unsigned int i = 0; i < strlen(time); ++i) {
    writeCharacter(time[i]);
  }  

  x = x - 1 - 6 - strlen(error);  
  UI.errorX = x + 1 + strlen(error);  
  moveCursorTo(UI.errorY, x);
  setColor(COLOR_RED_ON_BLK);
  for (unsigned int i = 0; i < strlen(error); ++i) {
    writeCharacter(error[i]);
  }  

  x = x - 1 - 6 - strlen(score);  
  UI.scoreX = x + 1 + strlen(score);  
  moveCursorTo(UI.scoreY, x);
  setColor(COLOR_GRN_ON_BLK);
  for (unsigned int i = 0; i < strlen(score); ++i) {
    writeCharacter(score[i]);
  }  

/*
  char debug;
  int debugTickX, debugTickY;
  enum COLORS debugTickC;
*/
} 

static void setScoreLarge()
{
  /* Should check the size of the space.
  int x, y;
  getMaxYX(&y, &x);
  int space = y - UI.boardR;
  */
  char* score = "Score:";
  char* error = "Error:";
  char* time  = "Time :";

  UI.scoreY = 5;
  UI.scoreX = UI.boardR + strlen("Score:") + 3;
  moveCursorTo(5, UI.boardR + 2);
  setColor(COLOR_GRN_ON_BLK);
  for (unsigned int i = 0; i < strlen("Score:"); ++i) {
    writeCharacter(score[i]);
  }  

  UI.errorY = 7;
  UI.errorX = UI.boardR + strlen("Error:") + 3;
  moveCursorTo(7, UI.boardR + 2);
  setColor(COLOR_RED_ON_BLK);
  for (unsigned int i = 0; i < strlen("Error:"); ++i) {
    writeCharacter(error[i]);
  }  

  UI.timeY = 9;
  UI.timeX = UI.boardR + strlen("Time :") + 4;
  UI.timeC = COLOR_WHT_ON_BLK;
  moveCursorTo(9, UI.boardR + 2);
  setColor(UI.timeC);
  for (unsigned int i = 0; i < strlen(" Time:"); ++i) {
    writeCharacter(time[i]);
  }  

  UI.debugTickX = UI.debugTickY = 1;
  UI.debugTickC = COLOR_WHT_ON_BLK;
} 

static void setInputArea()
{
  UI.inputX = UI.boardR + 2;   
  UI.inputY = 11;   
  UI.inputS = 20;
  UI.inputC = COLOR_WHT_ON_BLU;

  moveCursorTo(UI.inputY, UI.inputX);
  setColor(UI.inputC);
  for (int i = 0; i < UI.inputS; ++i) {
    writeCharacter(' ');
  }  
}

static int userInterfaceInit()
{
  getMaxYX(&UI.screenY, &UI.screenX);
  int center = (UI.screenX - 1) / 2;
  char borderChar = '|';

  UI.boardL = center - 40;
  UI.boardR = center + 40;
  UI.boardT = 0;
  UI.boardB = UI.screenY;
  
  // We set the edge of the board in
  // | Score: XXXXX = 14 characters before the edge of the screen.
  if (UI.screenX < 94) {
    setScoreSmall();
  }
  else {
    setScoreLarge();
    setInputArea();
  }

  /* We want the board to be at least 80. Adding sides adds 2,
   * so we need at least 82 to write the sides. */
  if (UI.screenX > 81) {
    setColor(COLOR_BLU_ON_BLK);
    for (int i = 0; i < UI.boardB; ++i) {
      moveCursorTo(i, UI.boardL);
      writeCharacter(borderChar);
      moveCursorTo(i, UI.boardR);
      writeCharacter(borderChar);
    }
    setColor(COLOR_WHT_ON_BLK);
  }


  return 1;
} 

/******************************************************************************
 * Player information
 *****************************************************************************/
struct Player 
{
  int score;
  int error;
  int words;
  unsigned int nw;/* new word speed. Value 0..100. 0 being no new words 
                     appearing, 100 is a new word every game tick. */
  int l;          /* Player level. Not implemented */
  int r;          /* Number of correct words in a row 1..4 */  
  int lw;         /* Lost word. If a word runs off the bottom of the screen. */
  float spd;      /* Time multiplier. Value 0.0 .. inf. 0.0 being no game
                     game ticks, inf 4 being 1 tick per 1/4 sec. */
  char fn[256];   /* File name */
  bool errorOnIncomplete;
};

struct Player PLYR = {0, 0, 0, 50, 0, 0, 0, 1.f, { '\0' }, false };

static void playerInit()
{
  UI.scoreC = COLOR_GRN_ON_BLK;
  UI.errorC = COLOR_RED_ON_BLK;
}

static void adjustPlayerScore(float s, float e, float w)
{
  PLYR.score += s;
  PLYR.error += e;
  PLYR.words += w;
}

/******************************************************************************
 * Words on screen
 *****************************************************************************/

#define MAX_SCREEN_WORDS 75 
static struct Word * wordList[MAX_SCREEN_WORDS]; 

static void clearWord(int index)
{
  /* A struct Word with the correct coordinates to delete */
  int i;
  setColor(COLOR_WHT_ON_BLK);
  moveCursorTo(wordList[index]->y, wordList[index]->x);
  for (i = 0; i < wordList[index]->size; ++i)
    writeCharacter(' ');
}

int validateWord(const char *str, int len)
{
  int i;
  for (i = 0; i < MAX_SCREEN_WORDS; ++i) {
    if (wordList[i]->size == len && strncmp(wordList[i]->word, str, len) == 0) {
      // We have a match.
      // Update the player score and remove word.
      if (PLYR.r < 4) ++PLYR.r;
      clearWord(i);
      wordList[i]->onScreen = false;
      wordList[i]->complete = true;
      adjustPlayerScore(wordList[i]->size * PLYR.r, 0.0, 1);
      wordList[i] = get_next_word();

      return 0;
    }
  }

  adjustPlayerScore(0.0, 3.0 + (PLYR.lw * 1.3), 0);
  return 1;
}

int placeNewWord()
{
  int r, index;
  for (index = 0; index < MAX_SCREEN_WORDS; ++index) {
    if ( !wordList[index]->onScreen && !wordList[index]->complete)
      break;
  }

  if (index == MAX_SCREEN_WORDS) return 0;

  wordList[index]->y = 0;

  while (1) {
    /* Keep looping until we get a number that is:
        1) An x position between left boarder and right boarder minux word size.
        2) The x position is not taken by a "close" word. TODO 
    */
    r = arc4random_uniform(UI.boardR);
    if (r > UI.boardL && r < UI.boardR - wordList[index]->size) {
      wordList[index]->x = r;
      wordList[index]->onScreen = true;
      break;
    }
  }

  return 1;
}

static int writeWordsTick()
{
  int i;
  float vertpos;
  
  for (i = 0; i < MAX_SCREEN_WORDS; ++i) {
    // XXX Fix this -> shouldnt erase a new word
    // TODO This is not clearing words that end on the bottom of the 
    // screen.
    if (wordList[i]->onScreen) {
      clearWord(i);

      /* Remove a word if it hits the bottom of the screen */
      if (++wordList[i]->y > UI.boardB - 1) {
        PLYR.error += wordList[i]->size;
        PLYR.r = 0;
        ++PLYR.lw;
        wordList[i]->onScreen = false;
        wordList[i] = get_next_word();
      }

      /* Set color based on Y position */
      vertpos = (wordList[i]->y / (float)UI.boardB) * 100.0f;
      if (vertpos > 80.f) setColor(COLOR_RED_ON_BLK);
      else if (vertpos > 50.f) setColor(COLOR_YLW_ON_BLK);
      else setColor(COLOR_WHT_ON_BLK);

      moveCursorTo(wordList[i]->y, wordList[i]->x);
      writeString(wordList[i]->word, wordList[i]->size);
    }
  }

  return 0;
}

/******************************************************************************
 * Writing to the screen
 *****************************************************************************/

static const char debugTickStringL1[] = "(%d,%d) Tick:%u";
static const char debugTickStringL2[] = "M:%1.3f NW:%d";

static void writeGameTick(unsigned int t)
{
  char str[2][22] = { { '\0' }, { '\0' } };

  setColor(UI.debugTickC);
  moveCursorTo(UI.debugTickY, UI.debugTickX);
  sprintf(str[0], debugTickStringL1, UI.screenX, UI.screenY, t);
  writeString(str[0], strlen(str[0]));
  moveCursorTo(UI.debugTickY + 1, UI.debugTickX);
  sprintf(str[1], debugTickStringL2, PLYR.spd, PLYR.nw);
  writeString(str[1], strlen(str[1]));
}

static int writeTime()
{
  int ret = 0;
  char tm[7] = { '\0' };

  sprintf(tm, "%4.1f", timerTotalTime());
  moveCursorTo(UI.timeY, UI.timeX);
  setColor(UI.timeC);
  ret = writeString(tm, strlen(tm));

  return ret;
}

static int writeInput(char c)
{  
  static int pos = 0; /* Position of next write */
  static char input[20] = { '\0' };
  int i;
  int ret = 0;        /* Return value */

  // TODO: This should flash red. Need to add flash class.
  if (pos > UI.inputS) return 0;

  setColor(UI.inputC);
  if (c == DEL && pos > 0) {
    moveCursorTo(UI.inputY, UI.inputX + --pos);
    ret = writeCharacter(' ');
    input[pos] = c;
  }
  else if (isalpha(c) && pos < UI.inputS) {
    input[pos] = c;
    moveCursorTo(UI.inputY, UI.inputX + pos++);
    ret = writeCharacter(c);
  }
  else if (c == CR && pos > 0) {
    if (validateWord(input, pos) == 0) {
      // Clear the user input area
      moveCursorTo(UI.inputY, UI.inputX);
      pos = 0;
      setColor(UI.inputC);
      for (i = 0; i < UI.inputS; ++i) {
        ret += writeCharacter(' ');
        input[i] = ' ';
      }
    }
    else {
      PLYR.r = 0;
    }
  }

  return ret;
}

static void writeScore()
{
  char score[8], error[8];

  sprintf(score, "%5d|%d", PLYR.score, PLYR.r);
  sprintf(error, "%5d|%d", PLYR.error, PLYR.lw);

  moveCursorTo(UI.scoreY, UI.scoreX);
  setColor(UI.scoreC);
  writeString(score, 7);

  moveCursorTo(UI.errorY, UI.errorX);
  setColor(UI.errorC);
  writeString(error, 8);
}

/******************************************************************************
 * Main game loop
 *****************************************************************************/
/*
 * multi 
 *  Greater the number, faster the clock ticks. 
 *  multi = 1.0 which is 1 second
 */
void run()
{
  setColor(COLOR_WHT_ON_BLK);
  writeScreen();
  unsigned int tick = 0; 
  int c;
  double timediff = 0.0;
  float interval = 1.f;
  bool cont = true;
  timerInit();

  while (cont) {
    timediff = timerDiff();

    if (1) writeTime();
    
    // Game ticks here
    if ((timediff * PLYR.spd) > interval) {
      ++tick;

      if (UI.debug) writeGameTick(tick);

      if (arc4random_uniform(100) < PLYR.nw) { 
        placeNewWord();
      }

      writeWordsTick();
      timerReset();
    } 
    
    c = getInput();
    switch (c) {
      case ESC:     cont = false;     break;
          if (UI.debug) {
      case ARROW_U: PLYR.spd += 0.05; break;
      case ARROW_D: PLYR.spd -= 0.05; break;
      case ARROW_R: PLYR.nw  += 5;    break;
      case ARROW_L: PLYR.nw  -= 5;    break;
          }
      default:      writeInput(c);
    }

    writeScore();
    writeScreen();
  } 
} 

/*
Options to add ??
    -s      Scoring. If supplied, words that go off screen
            are do not end the game.
    -f      Force 80x20 layout ????
    -fx:y   Force XxY layout ????
*/
void parse_args(int argc, char* argv[])
{
  int c /*, opterr */;

  while ((c = getopt(argc, argv, "dsl:w:f:")) != -1) {
    switch (c) {
      case 'd':
        UI.debug = true;
        break;
      case 's':
        // not implemented
        break;
      case 'l':
        PLYR.l = atoi(optarg);
        break;
      case 'w':
        if (strlen(optarg) > 255) fprintf(stderr, "The file name is to large.\n");
        else strcpy(PLYR.fn, optarg);
        break;
      case 'f':
        // Not implemented 
        break;
      default:
        fprintf(stderr, "spd [d,s,l,w,f]\n"
                        "  -d\tdebug on. Also use arrow keys to control game speed.\n"
                        "  -s\tScoring (not implemented)\n"
                        "  -l\t[1..n] Starting level (not implemented)\n"
                        "  -w\tfile name of word list\n"
                        "  -f\tX:Y of screen (not implemented.)\n\n");

        exit(EXIT_FAILURE);
        break;
    }
  }
}

void start_words(int argc, char* argv[])
{
  int i;
  if (argc > 1) parse_args(argc, argv);

  screenInit();
  setCursorOff();
  clearScreen();
  userInterfaceInit();

  // Init word list
  (PLYR.fn[0]) ? init_word_list(PLYR.fn) : init_word_list("data/words0.txt"); 
  for (i = 0; i < MAX_SCREEN_WORDS; ++i) wordList[i] = get_next_word();
  if ( !wordList[0]) 
  {
    screenDestroy();
    fprintf(stderr, "Could not get word list.\n");
    exit(1);
  }

  playerInit();

  // Run game
  run();

  destroy_word_list();
  screenDestroy();
} 
