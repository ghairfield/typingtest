#ifndef WORDS_H_
#define WORDS_H_

#define MAX_WORDS 200

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
