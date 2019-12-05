#include "game.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

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

uint16_t* makeString(const char* s, int sz, enum COLORS co)
{
  uint16_t* str = malloc(sizeof(uint16_t) * sz + 1);
  if ( !str) return NULL;
  
  int i = 0;
  for (; i < sz; ++i) {
    str[i] = makeCharacter(s[i], co);
  } 
  str[i] = '\0';

  return str;
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
  int errorY, errorX;
  int scoreY, scoreX;
  int timeY,  timeX;
} UI; 

static void setScoreSmall()
{
  // Set the score on the bottom 
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
  writeColorProfile(COLOR_GRN_ON_BLK);
  for (unsigned int i = 0; i < strlen("Score:"); ++i) {
    writeCharacter(score[i]);
  }  

  UI.errorY = 7;
  UI.errorX = UI.boardR + strlen("Error:") + 3;
  moveCursorTo(7, UI.boardR + 2);
  writeColorProfile(COLOR_RED_ON_BLK);
  for (unsigned int i = 0; i < strlen("Error:"); ++i) {
    writeCharacter(error[i]);
  }  

  UI.timeY = 9;
  UI.timeX = UI.boardR + strlen("Time :") + 3;
  moveCursorTo(9, UI.boardR + 2);
  writeColorProfile(COLOR_WHT_ON_BLK);
  for (unsigned int i = 0; i < strlen(" Time:"); ++i) {
    writeCharacter(time[i]);
  }  
} 

static int userInterfaceInit()
{
  int x, y;
  getMaxYX(&y, &x);
  int center = x / 2;
  //uint16_t borderChar = makeCharacter('|', COLOR_BLU_ON_BLK); 
  char borderChar = '|';

  UI.boardL = center - 60;
  UI.boardR = center + 60;
  UI.boardT = 1;
  UI.boardB = y;
  
  if (x < 90) {
    setScoreSmall();
  }
  else {
    setScoreLarge();
  }

  /* Draw the board. */
  writeColorProfile(COLOR_BLU_ON_BLK);
  for (int i = 1; i <= UI.boardB; ++i) {
    moveCursorTo(i, UI.boardL);
    writeCharacter(borderChar);
    moveCursorTo(i, UI.boardR);
    writeCharacter(borderChar);
  }
  writeColorProfile(COLOR_WHT_ON_BLK);


  return 1;
} 

/*
 * multi 
 *  Greater the number, faster the clock ticks. 
 *  multi = 1.0 which is 1 second
 */
void run(float multi)
{
  int y = 2;
  char c;
  unsigned char cont = 1;
  float interval = 0.8;
  double timedif = 0.0;

  // TODO
  // TODO
  // TODO TODO TODO
  // Either
  //  1) Make string
  //  2) Alias uint16_t to dblch <-- this one

  char sent[] = "Timer: ";
  int sentSz = strlen(sent);
  moveCursorTo(1,1);
  writeString(sent, sentSz);
  char tmstr[] = "%0.2f";
  timerInit();

  while (cont) {
    moveCursorTo(1, sentSz + 1);
    char wrt[15] = { '\0' };
    char tmply[15] = { '\0' };
    timedif = timerDiff();
    sprintf(wrt, tmstr, timedif);
    writeString(wrt, strlen(wrt));
    
    sprintf(tmply, "%4.2f", timerTotalTime());
    moveCursorTo(UI.timeY, UI.timeX);
    writeString(tmply, strlen(tmply));
    
    // Game ticks here
    if (timedif > interval) {
      timerReset();
    } 
    
    if ((c = getInput()) == EXIT) cont = 0;
  } 
} 

void start_game()
{
  screenInit();
  setCursorOff();
  clearScreen();
  userInterfaceInit();

  // get word list

  // Data structure
  
  // Need player structure

  // timer
  
  // Run game
  run(1.0);


  screenDestroy();
} 
