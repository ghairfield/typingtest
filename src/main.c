#include "game.h"
#include "screen.h"

#include <string.h>

void test_displayScreenCoords();

int main(int argc, char* argv[])
{
  start_words(argc, argv); 
//  test_displayScreenCoords();

  return 0;
}

void test_displayScreenCoords()
{
  screenInit(); 

  int x, y, sz;
  getMaxYX(&y, &x);
  setCursorOff();
  clearScreen();
  char topL[]   = "0,0";
  char topR[10] = { '\0' };
  char botL[10] = { '\0' };
  char botR[10] = { '\0' };
  sprintf(topR, "%d,%d", x - 1, 0);
  sprintf(botL, "%d,%d", 0, y - 1);
  sprintf(botR, "%d,%d", x - 1, y - 1);

  moveCursorTo(0, 0);
  writeString(topL, strlen(topL));

  sz = strlen(topR);
  moveCursorTo(0, x - sz);
  writeString(topR, sz);
  
  moveCursorTo(y - 1, 0);
  writeString(botL, strlen(botL));

  sz = strlen(botR);
  moveCursorTo(y - 1, x - sz);
  writeString(botR, sz);

  char msg[] = "C_c to exit";
  sz         = strlen(msg);
  int halfx  = x / 2;
  int halfy  = y / 2;
  moveCursorTo(halfy, halfx - (sz / 2));
  writeString(msg, sz);

  writeScreen();

  char c;
  while ((c = getInput()) != EXIT)
    ;
} 
