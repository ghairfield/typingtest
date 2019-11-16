#include "screen.h"
#include <string.h>

int main()
{
  screenInit();
  char st[] = "Hello world!!!";
  char c;

  clearScreen();
  moveCursorTo(0, 0);
  writeString(st, strlen(st));

  uint16_t f;
  f = setColor(0, COLOR_WHT_ON_RED);
  f = setCharacter(f, 'w');


  moveCursorTo(10, 15);
  writeCharacter(f);
  while ((c = getInput()) != EXIT)
    ;
  screenDestroy();

  return 0;
}
