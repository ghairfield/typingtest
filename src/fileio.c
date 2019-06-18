#include "fileio.h"

struct Word
{
  int size;
  char* w;
  struct Word* next;
};

struct Word* words = NULL;
//struct Line** lines = NULL;

void copy_word(char* l, int start, char* word, int size)
{
  for (int i = 0; i < size; ++i) {
    l[start + i] = word[i];
  }
}

struct Word* write_line(struct Line* l, struct Word* start, int cols)
{
  if (start == NULL) {
    l->ln = NULL;
    l->sz = 0;
    return NULL;
  }

  ++cols; // Make room for NULL
  l->ln = malloc(cols * sizeof(char));
  if ( !l->ln) {
    fprintf(stderr, "%s:%d - Could not allocate memory.\n", __FILE__, __LINE__);
    return NULL;
  } 
  memset(l->ln, '\0', cols);
  --cols;

  struct Word* tmp = start;
  struct Word* end = start;
  int count    = 0; // words
  int size     = 0; // size of words
  while (size + count < cols && tmp) {
    ++count;
    size += tmp->size;
    end  =  tmp;
    tmp  =  tmp->next;
  }

  // Previous is the end of the words we are copying
  --count;
  size -= end->size;
  tmp  =  start;
  int index = 0;
  while (tmp != end) {
    // Copy the word, add a space
    printf("Index: %d - Temp Word: %s\n\tString: %s\n\n", index, tmp->w, l->ln);
    copy_word(l->ln, index, tmp->w, tmp->size);
    //strcat(&(l->ln[index]), tmp->w);
    index += tmp->size;    
    tmp = tmp->next;
    copy_word(l->ln, index++, " ", 1);
  }
  strcpy(&(l->ln[index]), end->w);
  
  return end;
} 

void add_word(char word[], int count)
{
  assert(count > 0);
  
  // Allocate memory for the word
  struct Word* w = malloc(sizeof(struct Word));
  if ( !w){
    fprintf(stderr, "%s:%d - Could not allocate memory.\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  }

  w->w = malloc((count + 1) * sizeof(char));
  if ( !w->w) {
    fprintf(stderr, "%s:%d - Could not allocate memory.\n", __FILE__, __LINE__);
    exit(EXIT_FAILURE);
  } 

  w->size = count;
  strcpy(w->w, word);
  w->next = NULL;

  if ( !words) {
    // Create new word list.
    words = w;
  } 
  else {
    struct Word* tmp = words;
    while (tmp->next) tmp = tmp->next;
    
    tmp->next = w;
  } 
} 

void destroy_words()
{
  struct Word* tmp = words;
  struct Word* prev = words;
  while (tmp) {
    tmp = tmp->next;
    free(prev->w);
    free(prev);
    prev = tmp;
  }
}

void destroy_lines(struct Line*** lines, int size)
{
  struct Line** l = *lines;
  for(int i = 0; i < size; ++i) {
    free (l[i]->ln);
    free (l[i]);  
  }
}

int read_file_(const char* fname)
{
  FILE* f = fopen(fname, "r");
  if ( !f) {
    fprintf(stderr, "%s:%d - Could not open file <%s>\n", __FILE__, __LINE__, fname);
    return -1;
  }
  
  bool inWord = false;
  char word[MAX_LETTERS] = { '\0' };
  int  wordIndex = 0;
  int  c;
  // TODO: What is our other ending option? What if the file is 100000 words?
  while ((c = fgetc(f)) != EOF) {
    if ( !inWord && (char)c != ' ' && isprint(c)) {
      // Begining of a word.
      word[wordIndex++] = (char)c;
      inWord = true;
    }
    else if (inWord && isprint(c) && (char)c != ' ') {
      // In a word, add character to word
      word[wordIndex++] = (char)c;
    }
    else if (inWord && ((char)c == ' ' || (char)c == '\n')) {
      // At the end of a word. We ignore the space since we are going to 
      // implement our own spacing scheme.
      add_word(word, wordIndex);
      inWord = false;
      wordIndex = 0;
      memset(word, '\0', 100);
    }

    if (wordIndex >= MAX_LETTERS - 1) {
      fprintf(stderr, "%s:%d - Word buffer has been exceeded.\n", __FILE__, __LINE__);
      //TODO: Delete memory created.
      return -1;
    }     
  } 

  fclose(f);
  return 0;
} 

void print_words_()
{
  struct Word* w = words;
  while (w) {
    printf("%s %d\n", w->w, w->size);
    w = w->next;
  } 
}

int read_file(const char* fname, struct Line*** line, int size, int cols)
{
  struct Line** l = *line;
  if (read_file_(fname) != 0) return -1;
  struct Word* w = words;
  int i = 0;

  for (i = 0; i < size; ++i) {
    l[i] = malloc(sizeof(struct Line));
    if (!l[i]) {
      fprintf(stderr, "%s:%d - Could not create a line.\n", __FILE__, __LINE__);
      return -1;
    }
    w = write_line(l[i], w, cols);
    if (w == NULL) return i;
    w = w->next;
  }

  destroy_words();
  return i;
}
