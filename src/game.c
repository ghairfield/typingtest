#include "game.h"
#include "screen.h"

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

/******************************************************************************
 * Timing functions and vars.
 *****************************************************************************/
static const float MSEC_ = 1000000.0f;
static struct timeval tv_begin, tv_user;

void timerInit()
{
  gettimeofday(&tv_begin, NULL);
  gettimeofday(&tv_user, NULL);
} 

void timerReset()
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
  float interval = 0.1;
  double timedif = 0.0;
  clearScreen();

  char sent[] = "UPDATING IN: ";
  int sentSz = strlen(sent);
  moveCursorTo(1,1);
  writeString(sent, sentSz);
  char tmstr[] = "%0.5f";
  timerInit();

  while (cont) {
    moveCursorTo(1, sentSz + 1);
    char wrt[15] = { '\0' };
    timedif = timerDiff();
    sprintf(wrt, tmstr, timedif);
    writeString(wrt, strlen(wrt));
    
    if (timedif > interval) {
      // Move characters to the right. There might need to
      // be a refersh between timing so characters are 
      // highlighted correctly.
      
      moveCursorTo(y++, 0);
      writeString("Updating....", strlen("Updating...."));

      timerReset();
    } 
    
    if ((c = getInput()) == EXIT) cont = 0;
  } 
} 

void start_game()
{
  screenInit();
  setCursorOff();
//  clearScreen();

  // get word list

  // Data structure
  
  // Need player structure

  // timer
  
  // Run game
  run(1.0);


  screenDestroy();
} 
