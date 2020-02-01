#include "game.h"
#include "disp/screen.h"

#include <string.h>

void test_displayScreenCoords();
void test_menu();

int main(int argc, char* argv[])
{
 start_words(argc, argv); 
//  test_displayScreenCoords();
//  test_menu();

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

void test_menu() 
{
  screenInit(); 
  int x, y;
  getMaxYX(&y, &x);
  setCursorOff();
  clearScreen();

  const int top = (y / 2) - 20;
  const int right = (x / 2) + 40;
  const int bot = top + 40;
  const int left = right - 80;

  moveCursorTo(top, left);
  writeCharacter('+');
  moveCursorTo(top, right);
  writeCharacter('+');
  moveCursorTo(bot, left);
  writeCharacter('+');
  moveCursorTo(bot, right);
  writeCharacter('+');

  for (int i = left + 1; i < right; ++i) {
    moveCursorTo(top, i);
    writeCharacter('-');
    moveCursorTo(bot, i);
    writeCharacter('-');
  }
  for (int i = top + 1; i < bot; ++i) {
    moveCursorTo(i, left);
    writeCharacter('|');
    moveCursorTo(i, right);
    writeCharacter('|');
  }

  moveCursorTo(top + 1, left + 1);
  setColor(COLOR_BLK_ON_GRN);
  for (int i = left + 1; i < right; ++i) {
    writeCharacter(' ');
  }
  moveCursorTo(top + 1, (x/2)-2);
  writeString("Menu", 4);
  moveCursorTo(top + 2, left + 2);
  setColor(COLOR_WHT_ON_BLK);
  writeString("1. This is the first thing.", 27);
  writeScreen();

  char c;
  while ((c = getInput()) != EXIT)
    ;
  screenDestroy();
}


