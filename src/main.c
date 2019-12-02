#include "game.h"
#include "screen.h"

#include <string.h>

void test_displayScreenCoords();

int main()
{
  start_game(); 
//  test_displayScreenCoords();

  return 0;
}

void test_displayScreenCoords()
{
  screenInit(); 

  int x, y;
  getMaxYX(&y, &x);
  setCursorOff();
  clearScreen();
  char topL[] = "0,0";
  char topR[10] = { '\0' };
  char botL[10] = { '\0' };
  char botR[10] = { '\0' };
  sprintf(topR, "%d,%d", x, 0);
  sprintf(botL, "%d,%d", 0, y);
  sprintf(botR, "%d,%d", x, y);

  moveCursorTo(1, 1);
  writeString(topL, strlen(topL));

  int s;
  s = strlen(topR);
  moveCursorTo(0, x - s + 1);
  writeString(topR, strlen(topR));
  
  s = strlen(botL);
  moveCursorTo(y, 0);
  writeString(botL, strlen(botL));

  s = strlen(botR);
  moveCursorTo(y, x - s + 1);
  writeString(botR, strlen(botR));

  char msg[] = "C_c to exit";
  int size = strlen(msg);
  int halfx = x / 2;
  int halfy = y / 2;
  moveCursorTo(halfy, halfx - (size / 2));
  writeString(msg, size);

  char c;
  while ((c = getInput()) != EXIT)
    ;
} 
