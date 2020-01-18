#include "words.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct Word wl[MAX_WORDS];
static unsigned int currentIndex = 0; /* Index of next word */
static bool initHappened = false; /* Has the user init'd the words? */

static void init_words()
{
  for (int i = 0; i < MAX_WORDS; ++i) {
    wl[i].word = NULL;
    wl[i].size = 0;
    wl[i].x    = 0;
    wl[i].y    = 0;
    wl[i].seen = false;
    wl[i].onScreen = false;
  }
}

void destroy_word_list()
{
  for (int i = 0; i < MAX_WORDS; ++i) 
    if (wl[i].word) free (wl[i].word);
}

static int copy_word(int index, char *word, int sz)
{
  wl[index].word = malloc (sizeof(char) * (sz + 1));
  if ( !wl[index].word) {
    fprintf(stderr, "Could not allocate memory for a word. (%s:%d)\n", __FILE__, __LINE__);
    return -1;
  }

  strncpy(wl[index].word, word, sz);
  wl[index].size = sz;

  return 1;
}

static int get_words(FILE *fp)
{
  if ( !fp) return -1;
  
  char wordarr[256] = { '\0' }, c;
  int index = 0, count = 0, r = 0;

  while((c = fgetc(fp)) != EOF && count < 256) {
    if (c == '\n') {
      r = copy_word(index++, wordarr, count);
      if (r < 0) return -1; 
      count = 0;
      memset(wordarr, '\0', 256);
    }
    else {
      wordarr[count++] = c;
    }
  }

  return index;
}

int init_word_list(const char *fn)
{
  init_words();

  FILE *fp = fopen(fn, "r");
  if ( !fp) {
    perror ("Could not open the file!\n");
    return 0;
  }

  int ct = get_words(fp);
  if (ct > 0) initHappened = true;
  
  fclose(fp);
  return ct;
}

struct Word * get_next_word()
{
  if (initHappened && wl[currentIndex].word) return &wl[currentIndex++];
  else return NULL;
}
