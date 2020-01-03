#include "words.h"

#include "../list/src/list/list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initWordContainer(struct wordContainer **wc)
{
  *wc = malloc (sizeof (struct wordContainer));
  if ( !*wc) {
    perror ("Could not allocate memory for wordContainer\n");
    *wc = NULL;
    return;
  }

  (*wc)->size = 0;
  (*wc)->x    = 0;
  (*wc)->y    = 0;
}

int initWordContainerString(struct wordContainer **wc, const char *word)
{
  if ( !*wc) initWordContainer(wc);
  if ((*wc)->word) free ((*wc)->word);

  (*wc)->size = strlen(word);
  (*wc)->word = malloc (sizeof (char) * (*wc)->size + 1);
  if ( !(*wc)->word) {
    perror ("Could not allocate memory for Container word..\n");
    if (*wc) free (*wc);
    *wc = NULL;
    return -1;
  }

  strncpy((*wc)->word, word, (*wc)->size);
  return 0;
}

/* Requires a valid file pointer */
static int get_words(List *wl, FILE *fp, int cnt)
{
  if ( !fp) return -1;
  
  char wordarr[256] = { '\0' };
  int count = 0;
  struct wordContainer *wc_p = NULL;

  while (fgets(wordarr, 256, fp) != NULL && count < cnt) {
    if (initWordContainerString(&wc_p, wordarr) == 0) {
      list_push(wl, wc_p, sizeof (wc_p), 1);
      ++count;
      free (wc_p);
    }
    else return -1;
  }

  return count;
}

void destroy_word_list(List *wl)
{
  list_destroy(wl);
}

int get_word_list(List *wl, char *fn, int cnt)
{
  FILE *fp = fopen(fn, "r");
  if ( !fp) {
    perror ("Could not open the file!\n");
    return 0;
  }

  int ct = get_words(wl, fp, cnt);
  
  fclose(fp);

  return ct;
}

int main()
{
  List *l = list_init();
  int cnt = get_word_list(l, "data/words.txt", 200);

  printf("We received %d words:\n", cnt);
  struct wordContainer *wc;
  while ((wc = list_pop(l)) != NULL) {
    printf ("we received: \"%s\"|%d", wc->word, wc->size);
    free(wc->word);
    free(wc);
  }

  return 0;
}
