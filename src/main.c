#include "screen.h"

#include <string.h>

void test_displayScreenCoords();

int main()
{
  screenInit();
  setCursorOff();
  test_displayScreenCoords();

  char c;
  while ((c = getInput()) != EXIT)
    ;
  screenDestroy();

  return 0;
}

void test_displayScreenCoords()
{
  int x, y;
  getMaxYX(&y, &x);
  clearScreen();
  char topL[] = "0,0";
  char topR[10] = { '\0' };
  char botL[10] = { '\0' };
  char botR[10] = { '\0' };
  sprintf(topR, "%d,%d", x, 0);
  sprintf(botL, "%d,%d", 0, y);
  sprintf(botR, "%d,%d", x, y);

  moveCursorTo(0, 0);
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
} 
