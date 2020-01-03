#include "words.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initWordContainer(struct wordContainer *wc)
{
  wc = malloc (sizeof (struct wordContainer));
  if ( !wc) {
    perror ("Could not allocate memory for wordContainer\n");
    wc = NULL;
    return;
  }

  wc->size = 0;
  wc->x    =  0;
  wc->y    =  0;
}

int initWordContainerString(struct wordContainer *wc, const char *word)
{
  if ( !wc) {
    initWordContainer(wc);
  }

  wc->size = strlen(word);
  wc->word = malloc (sizeof (char) * wc->size + 1);
  if ( !wc->word) {
    perror ("Could not allocate memory for Container word..\n");
    if (wc) free (wc);
    wc = NULL;
    return -1;
  }

  strncpy(wc->word, word, wc->size);
  return 0;
}

static int copy_word(char **wl, char *word, int sz)
{
  *wl = malloc (sizeof(char) * (sz + 1));
  if ( !*wl) {
    perror ("Could not malloc a word.\n");
    return -1;
  }
  strncpy(*wl, word, sz);

  return 1;
}

/* Requires a valid file pointer */
static int get_words(char **wl, FILE *fp)
{
  if ( !fp) return -1;
  
  char wordarr[256] = { '\0' }, c;
  int index = 0, count = 0;

  while((c = fgetc(fp)) != EOF && index < 256) {
    if (c == '\n') {
      int r = copy_word(wl + count, wordarr, index);
      if (r < 0) return -1; 
      else  {
        ++count;
        index = 0;
      }
    }
    else {
      wordarr[index++] = c;
    }
  }

  return count;
}

void destroy_word_list(char ***wl)
{
  char **w = *wl;
  if (w) {
    int i;
    for (i = 0; i < MAX_WORDS; ++i) { 
      if (w[i]) free(w[i]);
    }
    free(*wl);
  }
}

int get_word_list(char ***wl, char *fn)
{
  FILE *fp = fopen(fn, "r");
  if ( !fp) {
    perror ("Could not open the file!\n");
    return 0;
  }

  *wl = malloc (sizeof (char *) * MAX_WORDS);
  if ( !wl) {
    perror ("Could not create list.\n");
    return 0;
  }
    
  memset(*wl, '\0', MAX_WORDS);
  int ct = get_words(*wl, fp);
  
  fclose(fp);
  return ct;
}
