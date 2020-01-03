#ifndef WORDS_H_
#define WORDS_H_

#define MAX_WORDS 200

/**
 * This is glue between the game and word list. The game needs the additional
 * information about the word then the file contains. It would be good in the 
 * future to refactor this away from game.c
 */
struct wordContainer
{
  char *word;
  int  size;
  int  x, y;
};

/**
initWordContainer
  Initilizes the word container. It is possible to init one with a
  string, see @initWordContainerString(). Everything is set to default
  NULL or 0.
@param wc Word container to initialize
returns None
*/
void initWordContainer(struct wordContainer *wc);

/**
initWordContainerString
  Initialiazes a word container with the given string. The size is set and 
  everything else is set to 0.
@param wc Container to initalize
@param word Word to initialize @wc to. Must be NULL terminated.
returns 0 on success, < 1 on failure.
*/
int initWordContainerString(struct wordContainer *wc, const char *word);

/**
 * get_word_list
 *    Reads a file and copies the words in to an array. The words
 *    should be seperated by a newline in the file. 
 *  param wl NULL pointer. Will contain the word list afterwards.
 *  param fn Valid file name
 *  returns Number of words copied in to @wl
 */
int get_word_list(char ***wl, char *fn);

/**
 * destroy_word_list
 *    Deletes the memory allocated from get_word_list. Should be
 *    a 2x array.
 *  param wl Word list to free
 *  returns Nothing
 */
void destroy_word_list(char ***wl);

#endif /* Include guard */
