#ifndef WORDS_H_
#define WORDS_H_

#include <stdbool.h>

#define MAX_WORDS 1000

struct Word
{
  char *word;
  int  size;
  int  x, y;

  bool seen;      /* Has the game seen this word? Set by get_next_word */
  bool complete;  /* The word has been typed in correctly, set by user */
  bool onScreen;  /* Is the word currently on screen. Set by user. */
};

/**
get_word_list
  Reads a file and copies the words in to an array. The words
    should be seperated by a newline in the file. 
  param fn Valid file name
  returns Number of words copied in to word list 
*/
int init_word_list(const char *fn);

/**
destroy_word_list
  Deletes the memory allocated from init_word_list.  
  returns Nothing
*/
void destroy_word_list();

/**
get_next_word
  Returns a pointer to the next available word in the word list.
  returns Pointer to a valid word.
*/
struct Word * get_next_word();

#endif /* Include guard */
