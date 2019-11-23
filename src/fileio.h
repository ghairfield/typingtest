#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_LETTERS 256 /**< Max size of each word. */

/**
Each line read from the file is represented by struct Line.

@sz The size of the line. sz <= cols (from @read_file)
@ln Contents of the line.
*/
struct Line
{
  int sz;
  char* ln;
};

/**
Reads a file and creates a Line array with the contents.

Copies each of the words from the file and breaks them
into a set of words, without spacing or any non-printable text. Then
copies them into an array of struct Line until the entire file has been
read or number of lines equals size. 

@pre *line is already created, size > 0 && cols > 0
@post line is filled with either @size lines or the entire file

@param fname File name to open
@param line To be filled with line contents
@param size Size of line. *line should be allocated.
@param cols Max width of each string. 

@returns < 0 if an error occurred, or the number of lines created.
*/
int read_file(const char* fname, struct Line** line, int size, int cols);

/**
Deletes the memory allocated for the lines.

@pre an array Lines exist. 
@post All memory has been cleaned up.

@param lines A pointer to the lines
@param size Size of lines
*/
void destroy_lines(struct Line** lines, int size);
