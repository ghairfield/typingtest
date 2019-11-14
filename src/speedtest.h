/* * * * * * * * * 
 * Global def's
 * * * * * * * * */ 
#define true 1
#define false 0
#define ttyin STDIN_FILENO
#define ttyout STDOUT_FILENO

enum KEYMAP {
  CTRL_C = 3,   /* ETX - quit */
  BACK   = 8,   /* Backspace */
  CR     = 13,  /* Enter */
  CTRL_Q = 17,  /* Quit */
  ESC    = 27,  /* Escape */
  DEL    = 127, /* Delete */
  EXIT   = 0    /* Exit the program gracefuly. */
};

enum COLORS {
  NORMAL = 0, /* Black on white */
  GOOD, /* Green on black */
  WARN, /* Yellow on black */
  CRISIS /* Red on black */
};

struct Display
{
  int cols;
  int rows;
};

struct Player
{
  int correct;
  int error; 
  int x, y;
  double score;
  clock_t start; 
};
