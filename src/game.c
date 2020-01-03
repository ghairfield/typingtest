#include "game.h"
#include "words.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <ctype.h>

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

/*******************************************************************************
 * Helpers
 ******************************************************************************/

static inline int setEven(int v)
{
  return ((v % 2) == 0) ? v : ++v;
}

/******************************************************************************
 * User Interface
 *****************************************************************************/

struct UserInterface
{
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
  char debug;
  int debugTickX, debugTickY;
  enum COLORS debugTickC;
} UI; 

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
  UI.timeX = UI.boardR + strlen("Time :") + 3;
  moveCursorTo(9, UI.boardR + 2);
  setColor(COLOR_WHT_ON_BLK);
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
  UI.inputS = 15;
  UI.inputC = COLOR_WHT_ON_BLU;

  moveCursorTo(UI.inputY, UI.inputX);
  setColor(UI.inputC);
  for (int i = 0; i < UI.inputS; ++i) {
    writeCharacter(' ');
  }  
}

static int userInterfaceInit()
{
  int x, y;
  getMaxYX(&y, &x);
  int center = x / 2;
  char borderChar = '|';

  UI.boardL = center - 40;
  UI.boardR = center + 40;
  UI.boardT = 1;
  UI.boardB = y;
  
  // We set the edge of the board in
  // | Score: XXXXX = 14 characters before the edge of the screen.
  if (x < 94) {
    setScoreSmall();
  }
  else {
    setScoreLarge();
    setInputArea();
  }

  /* We want the board to be at least 80. Adding sides adds 2,
   * so we need at least 82 to write the sides. */
  if (x > 81) {
    setColor(COLOR_BLU_ON_BLK);
    for (int i = 1; i <= UI.boardB; ++i) {
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
 * Words on screen
 *****************************************************************************/
static struct wordContainer **words;
static struct wordContainer wCont;
char **wordList;

int placeNewWord(const char *w)
{
  srand(time(NULL));
  initWordContainerString(&wCont, w); 
  int r;

  while (1) {
    /* Keep looping until we get a number that is:
        1) An x position between left boarder and right boarder minux word size.
        2) The x position is not taken by a "close" word. TODO 
    */

    r = rand() % (UI.boardR);
    if (r > UI.boardL && r < UI.boardR - wCont.size) {
      // Set the values
      break;
    }
  }

  wCont.x = r;
  wCont.y = 0;
  return 1;
}


/**
    User Input area

    Good practice to always reset the color when exiting a 
    function? This increases the function calls, but also
    makes for a hetrogenous screen call.
*/
static void clearWord(struct wordContainer *wc)
{
  /* A wordContainer with the correct coordinates to delete */
  enum COLORS co = getCurrentColor();
  int i;
  setColor(COLOR_WHT_ON_BLK);
  moveCursorTo(wc->y, wc->x);
  for (i = 0; i < wc->size; ++i)
    writeCharacter(' ');
  setColor(co);
}

static int writeWordsTick()
{
  enum COLORS co = getCurrentColor();
  clearWord(&wCont);
  ++wCont.y;
  // TODO If this move pushed the word past UI.boardB, add
  // an error or something.
  moveCursorTo(wCont.y, wCont.x);

  // Set color based on Y
  float vertpos = (wCont.y / (float)UI.boardB) * 100.0f;
  if (vertpos > 80.0) setColor(COLOR_RED_ON_BLK);
  else if (vertpos > 50.0) setColor(COLOR_YLW_ON_BLK);
  else setColor(COLOR_WHT_ON_BLK);

  writeString(wCont.word, wCont.size);
  setColor(co);

  return 0;
}

static const char debugTickString[] = "%u|%1.2f";
static int writeGameTick(unsigned int t)
{
  int ret = 0;
  char str[15] = { '\0' };
  enum COLORS co = getCurrentColor();

  float vertpos = (wCont.y / (float)UI.boardB) * 100.0f;
  
  setColor(UI.debugTickC);
  moveCursorTo(UI.debugTickY, UI.debugTickX);
  sprintf(str, debugTickString, t, vertpos);
  ret = writeString(str, strlen(str));

  setColor(co);
  return ret;
}

static int writePlayerTime()
{
  enum COLORS co = getCurrentColor();
  setColor(UI.timeC);
  int ret = 0;
  char tm[7] = { '\0' };

  sprintf(tm, "%4.1f", timerTotalTime());
  moveCursorTo(UI.timeY, UI.timeX);
  ret = writeString(tm, strlen(tm));

  setColor(co);
  return ret;
}

static char input[15] = { '\0' };
static int writeInput(char c)
{  
  static int pos = 0; /* Position of next write */

  // TODO: This should flash red. Need to add flash class.
  if (pos > UI.inputS) return 0;
  
  int ret = 0;        /* Return value */
  enum COLORS co = getCurrentColor();

  setColor(UI.inputC);
  if (c == DEL && pos > 0) {
    moveCursorTo(UI.inputY, UI.inputX + --pos);
    ret = writeCharacter(' ');
    input[pos] = c;
  }
  else if (isalpha(c) && pos < UI.inputS) {
    input[pos] = c;
    moveCursorTo(UI.inputY, UI.inputX + pos++);
    ret =  writeCharacter(c);
  }
  else if (c == CR && pos > 0) {
    // TODO: Check if the word is correct.

    // Clear the input area (assuming word is correct).
    if (1) {
      moveCursorTo(UI.inputY, UI.inputX);
      pos = 0;
      for (int i = 0; i < UI.inputS; ++i) {
        ret += writeCharacter(' ');
        input[i] = ' ';
      }
    }
  }

  setColor(co);
  return ret;
}

/*
 * multi 
 *  Greater the number, faster the clock ticks. 
 *  multi = 1.0 which is 1 second
 */
void run(float multi)
{
  setColor(COLOR_WHT_ON_BLK);
  writeScreen();
  unsigned int tick = 0;
  double timediff = 0.0;
  float interval = 0.8;
  char cont = 1;
  timerInit();
  placeNewWord("Tacos");

  while (cont) {
    char c = 0;
    timediff = timerDiff();

    if (1) writePlayerTime();
    
    // Game ticks here
    if ((timediff * multi) > interval) {
      ++tick;

      if (1) {
        writeGameTick(tick);
      }

      writeWordsTick();
      timerReset();
    } 
    
    if ((c = getInput()) == EXIT) cont = 0;
    writeInput(c); 
    writeScreen();
  } 
} 

/*
Options to add
    -d      Debug (turn on game timer/fps etc.)
    -s      Scoring. If supplied, words that go off screen
            are do not end the game.
    -lX     Level provided by X
    -f      Force 80x20 layout
    -fx:y   Force XxY layout
    -w      Personal word list?
*/
void start_game()
{
  screenInit();
  setCursorOff();
  clearScreen();
  userInterfaceInit();
/*
  // get word list
  char **wordList = NULL;
  int ct = get_word_list(&wordList, "data/words.txt");
  int i;

  printf("We got %d words back, the first 10 are:\n", ct);
  for (i = 0; i < 10; ++i)
    printf("%s\n", wordList[i]);
*/
  // Data structure
  
  // Need player structure

  // timer
  
  // Run game
  run(1.0f);


 // destroy_word_list(&wordList);
  screenDestroy();
} 
