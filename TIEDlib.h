/*
 * Created by Leander on 07/05/2021.
 * TIED Library Header
 */

#ifndef PROJECT_TIED_TIEDLIB_H
#define PROJECT_TIED_TIEDLIB_H

// Libraries
/*
 * This header contains the following headers:
 * - stdio.h
 * - stdlib.h
 * - time.h
 * - stdarg.h
 * - string.h
 * - ctype.h
 */
#include "lilib.h"
#include <unistd.h>

// Definitions
// Use this to call the caesarCipherF function, it will add the size for you. If you use malloc, be sure to fill the memory with something not null!
// Arrays
#define caesarCipherA(input, output, encrypt, key) caesarCipherF(input, sizeOfArray(input), output, sizeOfArray(output), encrypt, key)
// Strings
#define caesarCipherS(input, output, encrypt, key) caesarCipherF(input, strlen(input), output, strlen(output), encrypt, key)

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

/*
 * TBD
 */
char caesarCipherF(char *input, unsigned int inputLength, char *output, unsigned int outputLength, unsigned char encrypt, unsigned char key);

#endif //PROJECT_TIED_TIEDLIB_H
