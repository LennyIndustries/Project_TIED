/*
 * Created by Leander on 07/05/2021.
 * TIED Library Header
 */

#ifndef PROJECT_TIED_TIEDLIB_H
#define PROJECT_TIED_TIEDLIB_H

// Libraries
#include "lilib.h"
/*
 * This header contains the following headers:
 * - stdio.h
 * - stdlib.h
 * - time.h
 * - stdarg.h
 * - string.h
 * - ctype.h
 */
#include <unistd.h>

// Definitions

// Functions
/*
 * Counts the character in a file.
 * @param: char * (fileName) the name of the file.
 * @return: int (counter) the total counted characters
 */
unsigned int fileCharacters(char *fileName);

/*
 * Prints help output.
 * @param: void
 * @return: void
 */
void printHelp(void);

/*
 * TBD
 */
char checkFile(char *fileName, char *fileExtension, char terminate, char existing);

#endif //PROJECT_TIED_TIEDLIB_H
