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

#define MAX_SCREEN_WORDS 25 // TODO: 25 max words on screen?
static struct Word * wordList[MAX_SCREEN_WORDS]; 

static void clearWord(int index)
{
  /* A struct Word with the correct coordinates to delete */
  enum COLORS co = getCurrentColor();
  int i;
  setColor(COLOR_WHT_ON_BLK);
  moveCursorTo(wordList[index]->y, wordList[index]->x);
  for (i = 0; i < wordList[index]->size; ++i)
    writeCharacter(' ');
  setColor(co);
}

int validateWord(const char *str, int len)
{
  int i;
  for (i = 0; i < MAX_SCREEN_WORDS; ++i) {
    if (strncmp(wordList[i]->word, str, len) == 0) {
      // We have a match.
      // Update the player score and remove word.
      clearWord(i);
      wordList[i]->onScreen = false;
      return 1;
    }
  }
  return 0;
}

int placeNewWord(int index)
{
  int r;
  wordList[index]->y = 0;

  while (1) {
    /* Keep looping until we get a number that is:
        1) An x position between left boarder and right boarder minux word size.
        2) The x position is not taken by a "close" word. TODO 
    */
    r = rand() % (UI.boardR);
    if (r > UI.boardL && r < UI.boardR - wordList[index]->size) {
      // Set the values
      wordList[index]->x = r;
      wordList[index]->seen = true;
      wordList[index]->onScreen = true;
      break;
    }
  }

  return 1;
}

static int writeWordsTick()
{
  enum COLORS co = getCurrentColor();
  int i;
  float vertpos;
  
  for (i = 0; i < MAX_SCREEN_WORDS; ++i) {
    // XXX Fix this -> shouldnt erase a new word
    // TODO This is not clearing words that end on the bottom of the 
    // screen.
    if (wordList[i]->onScreen) {
      clearWord(i);

      if (++wordList[i]->y > UI.boardB) {
        // TODO Error here.
        wordList[i]->onScreen = false;
      };

      // Set color based on Y
      vertpos = (wordList[i]->y / (float)UI.boardB) * 100.0f;
      if (vertpos > 80.0) setColor(COLOR_RED_ON_BLK);
      else if (vertpos > 50.0) setColor(COLOR_YLW_ON_BLK);
      else setColor(COLOR_WHT_ON_BLK);

      // Write word
      moveCursorTo(wordList[i]->y, wordList[i]->x);
      writeString(wordList[i]->word, wordList[i]->size);
    }
  }

  setColor(co);

  return 0;
}

static const char debugTickString[] = "%u";
static int writeGameTick(unsigned int t)
{
  int ret = 0;
  char str[15] = { '\0' };
  enum COLORS co = getCurrentColor();

  setColor(UI.debugTickC);
  moveCursorTo(UI.debugTickY, UI.debugTickX);
  sprintf(str, debugTickString, t);
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

static int writeInput(char c)
{  
  static int pos = 0; /* Position of next write */
  static char input[15] = { '\0' };

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
    // Clear the input area (assuming word is correct).
    if (validateWord(input, pos - 1) == 1) {
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
  int wordIndex = 0, wordCount = 0;
  double timediff = 0.0;
  float interval = 1.f;
  char cont = 1;
  timerInit();

  srand(time(NULL));

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

      if (rand() % 3 == 0 && wordCount < MAX_SCREEN_WORDS){
        ++wordCount;
        placeNewWord(wordIndex++);
      }

      writeWordsTick();
      timerReset();
    } 
    
    if ((c = getInput()) == ESC) cont = 0;
    writeInput(c); 
    writeScreen();
  } 
} 

void dummy()
{
  //debug only
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

  // Init word list
  init_word_list("data/words.txt");
  int i = 0;
  for (; i < MAX_SCREEN_WORDS; ++i) wordList[i] = get_next_word();
 
  // Need player structure

  // Run game
  run(1.f);

  dummy();

  destroy_word_list();
  screenDestroy();
} 
